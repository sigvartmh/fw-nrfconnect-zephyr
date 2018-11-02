# Do not add using zephyr cmake extensions.
# This because our library should not be added to the 'ZEPHYR_LIBS' list,
# since it should not be linked to the zephyr elf itself.

add_library(bootloader bootloader.c)

add_subdirectory(c_runtime_setup)
add_subdirectory(libc)
add_subdirectory(uart)
add_subdirectory(rtt)
add_subdirectory(lockdown)
add_dependencies(bootloader offsets_h)

target_link_libraries(bootloader PRIVATE zephyr_interface)
target_link_libraries(bootloader PRIVATE c_runtime_setup)
target_link_libraries(bootloader PRIVATE libc)
target_link_libraries(bootloader PRIVATE uart)
target_link_libraries(bootloader PRIVATE segger_rtt)
target_link_libraries(bootloader PRIVATE lockdown)

target_include_directories_ifdef(CONFIG_SB_FLASH_LOCKDOWN
  bootloader 
  PRIVATE lockdown/include
  )

# SEGGER files for DEBUG
target_include_directories_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT
  bootloader
  PRIVATE
  rtt
  )

target_include_directories_ifdef(CONFIG_SB_DEBUG_PORT_UART
  bootloader
  PRIVATE
  uart
  )



