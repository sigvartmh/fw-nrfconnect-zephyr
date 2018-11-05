# Create linker flags for zephyr bootloader
set(bl_zephyr_lnk
  ${LINKERFLAGPREFIX},-Map=${PROJECT_BINARY_DIR}/bootloader.map
  -u_OffsetAbsSyms
  -u_ConfigAbsSyms
  # We need all symbols defined in the bootloader library,
  # Even those that are not used anywhere.
  # Acheive this by using the '--whole-archive' flag.
  ${LINKERFLAGPREFIX},--whole-archive
  bootloader # include bootloader library
  ${LINKERFLAGPREFIX},--no-whole-archive
  ${OFFSETS_O_PATH}
  ${LIB_INCLUDE_DIR}
  ${TOOLCHAIN_LIBS}
  )

# Create custom linker script, will be named 'bootloader.cmd'
# and will be called with define
# '-DLINKER_OUTPUT_NAME_bootloader'
# The function below generates the command which is then passed to
# 'add_custom_command'.
construct_add_custom_command_for_linker_pass(bootloader custom_command)
add_custom_command(
  ${custom_command}
  )

# Create custom target for generated linker script 'bootloader.cmd'
# This is done since CMake does not handle dependencies to files very
# well, so this is done to be able to represent a dependency to the
# generated 'bootloader.cmd' file.
add_custom_target(
  bootloader_script
  DEPENDS
  ${ALIGN_SIZING_DEP}
  bootloader.cmd
  offsets_h
  )

# Associate include directories to linker script target.
# This needs to be done since preprocessing is done in linker script.
set_property(TARGET
  bootloader_script
  PROPERTY INCLUDE_DIRECTORIES
  ${ZEPHYR_INCLUDE_DIRS}
  )

# Create the executable target with a dummy file.
add_executable(bootloader_elf misc/empty_file.c)

# Link executable target with custom bootloader linker script 'bootloader.cmd' and the linker flags in 'bl_zephyr_lnk'
target_link_libraries(bootloader_elf ${TOPT} ${PROJECT_BINARY_DIR}/bootloader.cmd ${bl_zephyr_lnk})

# Create dependency between elf target and linker script.
set_property(TARGET bootloader_elf PROPERTY LINK_DEPENDS ${PROJECT_BINARY_DIR}/bootloader.cmd)

# Since CMake does not handle file dependencies very well, create dependency to 'bootloader_script'
# target, as well as standard stuff.
add_dependencies(bootloader_elf ${ALIGN_SIZING_DEP} ${PRIV_STACK_DEP} bootloader_script offsets)

# Create custom command for creating hex file from bootloader elf file.
add_custom_command(
  OUTPUT bootloader.hex
  COMMAND ${CMAKE_OBJCOPY}
  -S
  -Oihex
  -R .comment
  -R COMMON
  -R .eh_frame
  bootloader_elf.elf
  bootloader.hex
  DEPENDS bootloader_elf
  )
# Create target with the generated custom command.
# This is needed since CMake does not represent denpendencies to files in a good manner.
add_custom_target(bootloader_hex DEPENDS bootloader.hex)

# Create list of all files to be merged.
list(APPEND my_hex_files ${SIGNED_KERNEL_HEX_NAME})
list(APPEND my_hex_files bootloader.hex)

# Create a list of the targets of the files to be merged.
list(APPEND my_hex_targets zephyr_hex)
list(APPEND my_hex_targets bootloader_hex)

# Create custom command for merging all files in the list.
add_custom_command(
  OUTPUT merged.hex
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${ZEPHYR_BASE}/scripts/mergehex.py
  -i ${my_hex_files}
  -o merged.hex
  DEPENDS ${my_hex_targets} sign
  )

add_custom_target(merged_hex ALL DEPENDS merged.hex)

# Check if PEM file is specified by user, if not, create one.

# First, check environment variables. Only use if not specified in command line.
if (DEFINED ENV{SB_VALIDATION_PEM_PATH} AND NOT SB_VALIDATION_PEM_PATH)
  if (NOT EXISTS "$ENV{SB_VALIDATION_PEM_PATH}")
    message(FATAL_ERROR "ENV points to non-existing PEM file '$ENV{SB_VALIDATION_PEM_PATH}'")
  else()
    message("SET in ENV")
    set(CONFIG_SB_VALIDATION_PEM_PATH $ENV{SB_VALIDATION_PEM_PATH})
  endif()
endif()

# Next, check command line arguments
if (DEFINED SB_VALIDATION_PEM_PATH)
  message("SET IN COMMAND LINE")
  set(CONFIG_SB_VALIDATION_PEM_PATH ${SB_VALIDATION_PEM_PATH})
endif()

# Lastly, check if it is not set anywhere, it which case a new
# PEM file will be genareted.
if( "${CONFIG_SB_VALIDATION_PEM_PATH}" STREQUAL "")
  set(CONFIG_SB_VALIDATION_PEM_PATH ${PROJECT_BINARY_DIR}/GENERATED_NON_SECURE_PEM.pem)

  set(cmd
    ${CMAKE_COMMAND} -E env
    PYTHONPATH=${ZEPHYR_BASE}/scripts/
    ${PYTHON_EXECUTABLE}
    ${ZEPHYR_BASE}/scripts/keygen.py
    --output ${CONFIG_SB_VALIDATION_PEM_PATH}
    DEPENDS ${logical_target_for_zephyr_elf}
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    )

  # Set this, so that the 'sign' target depends on it.
  set(KEYGEN_TARGET keygen)
  add_custom_target(keygen
    ALL
    COMMAND
    ${cmd}
    COMMENT
    "Generating temporary PEM file ${CONFIG_SB_VALIDATION_PEM_PATH}"
    USES_TERMINAL
    )
else()
  if (NOT EXISTS "${CONFIG_SB_VALIDATION_PEM_PATH}")
    message(FATAL_ERROR "Config points to non-existing PEM file '${CONFIG_SB_VALIDATION_PEM_PATH}'")
  endif()
  add_custom_target(keygen)  # Dummy target TODO can we get around this somehow?
endif()


set(SIGNED_KERNEL_HEX_NAME signed_${KERNEL_HEX_NAME})

set(cmd
  ${CMAKE_COMMAND} -E env
  PYTHONPATH=${ZEPHYR_BASE}/scripts/
  ${PYTHON_EXECUTABLE}
  ${ZEPHYR_BASE}/scripts/sign.py
  --input ${PROJECT_BINARY_DIR}/${KERNEL_HEX_NAME}
  --output ${SIGNED_KERNEL_HEX_NAME}
  --offset ${CONFIG_SB_VALIDATION_METADATA_OFFSET}
  --pem ${CONFIG_SB_VALIDATION_PEM_PATH}
  --magic-value 1122334455667788
  DEPENDS ${logical_target_for_zephyr_elf} ${KEYGEN_TARGET}
  WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
  )

add_custom_target(sign
  ALL
  COMMAND
  ${cmd}
  COMMENT
  "Creating validation for ${KERNEL_HEX_NAME}, storing to ${SIGNED_KERNEL_HEX_NAME}"
  USES_TERMINAL
  )

if(CONFIG_OUTPUT_PRINT_MEMORY_USAGE)
  set(option ${LINKERFLAGPREFIX},--print-memory-usage)
  string(MAKE_C_IDENTIFIER check${option} check)

  set(SAVED_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${option}")
  zephyr_check_compiler_flag(C "" ${check})
  set(CMAKE_REQUIRED_FLAGS ${SAVED_CMAKE_REQUIRED_FLAGS})

  target_link_libraries_ifdef(${check} bootloader_elf ${option})
endif()
