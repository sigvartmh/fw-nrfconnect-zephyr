# Build a second bootloader image
zephyr_add_multi_image()
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../../../mcuboot/boot/zephyr" ${CMAKE_CURRENT_BINARY_DIR}/mcuboot)
