# Build a second bootloader image

# TODO: Use another mechanism to communicate that the we are not
# processing the first image?
set(MULTI_IMAGE 1)

set(IMAGE 1_)

add_subdirectory("${CONFIG_MCUBOOT_CMAKELISTS_DIR}" ${CMAKE_CURRENT_BINARY_DIR}/mcuboot)
