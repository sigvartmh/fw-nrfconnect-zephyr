# Build a second bootloader image

# TODO: Make configurable like OT until west is supported.
set(MCUBOOT_BASE "${ZEPHYR_BASE}/../mcuboot")
assert_exists(MCUBOOT_BASE)

zephyr_add_multi_image(mcuboot)
add_subdirectory("${MCUBOOT_BASE}/boot/zephyr" ${CMAKE_CURRENT_BINARY_DIR}/mcuboot)

# TODO: Assert that the bootloader and image use the same key.

set(SIGNED_IMAGE signed_by_mcuboot_${KERNEL_HEX_NAME})

set_property(GLOBAL APPEND PROPERTY
  extra_post_build_commands
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${MCUBOOT_BASE}/scripts/imgtool.py
  sign
  --key ${MCUBOOT_BASE}/${CONFIG_BOOT_SIGNATURE_KEY_FILE}
  --header-size ${CONFIG_TEXT_SECTION_OFFSET}
  --align 8           # TODO: Configurable?
  --version 1.0       # TODO: Configurable?
  --slot-size 0x69000 # TODO: Configurable?
  ${PROJECT_BINARY_DIR}/${KERNEL_HEX_NAME}  # TODO: Enforce that this will be present through Kconfig
  ${PROJECT_BINARY_DIR}/${SIGNED_IMAGE}
  )

set_property(GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${SIGNED_IMAGE}
  )
set_property(GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE_TARGET
  ${logical_target_for_zephyr_elf}
  )
