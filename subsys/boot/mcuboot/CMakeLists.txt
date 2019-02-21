# Build a second bootloader image

# TODO: Make configurable like OT until west is supported.
set(MCUBOOT_BASE "${ZEPHYR_BASE}/../mcuboot")
assert_exists(MCUBOOT_BASE)

zephyr_add_executable(mcuboot)
add_subdirectory("${MCUBOOT_BASE}/boot/zephyr" ${CMAKE_CURRENT_BINARY_DIR}/mcuboot)
# TODO: Assert that the bootloader and image use the same key.

set(SIGNED_IMAGE ${CMAKE_CURRENT_BINARY_DIR}/signed_by_mcuboot_zephyr.hex)

get_property(SECURE_IMAGE_HEX_FILES_TO_MERGE_TARGET
	GLOBAL PROPERTY
	SECURE_IMAGE_HEX_FILES_TO_MERGE_TARGET
)

add_custom_command(
  OUTPUT
  ${SIGNED_IMAGE}
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${MCUBOOT_BASE}/scripts/imgtool.py
  sign
  --key ${MCUBOOT_BASE}/${CONFIG_BOOT_SIGNATURE_KEY_FILE}
  --header-size 0x200 # TODO: Configurable?
  --align 8           # TODO: Configurable?
  --version 1.0       # TODO: Configurable?
  --slot-size 0x32000 # TODO: Configurable?
  ${CMAKE_BINARY_DIR}/zephyr/merged_secure_fw.hex # TODO: Enforce that this will be present through Kconfig and make sure only to do the merged_Secure_fw.hex when SPM is enabled
  ${SIGNED_IMAGE}
  DEPENDS
  merged_secure_fw
  )

add_custom_target(signed_image_target DEPENDS ${SIGNED_IMAGE} )

set_property(GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${SIGNED_IMAGE}
  )
set_property(GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE_TARGET
  signed_image_target
  ${logical_target_for_zephyr_elf}
  )
