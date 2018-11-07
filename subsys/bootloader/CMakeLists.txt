# Do not add using zephyr cmake extensions.
# This because our library should not be added to the 'ZEPHYR_LIBS' list,
# since it should not be linked to the zephyr elf itself.

add_library(bootloader bootloader.c)

target_link_libraries(bootloader PRIVATE bl0_crypto)

add_subdirectory(c_runtime_setup)
target_link_libraries(bootloader PRIVATE c_runtime_setup)

add_subdirectory(provision)
target_link_libraries(bootloader PRIVATE provision)
target_include_directories(bootloader PRIVATE provision)

add_subdirectory_ifdef(CONFIG_SECURE_BOOT_DEBUG libc)
target_link_libraries_ifdef(SECURE_BOOT_DEBUG bootloader PRIVATE libc)

add_subdirectory_ifdef(CONFIG_SB_FLASH_LOCKDOWN lockdown)
target_link_libraries_ifdef(CONFIG_SB_FLASH_LOCKDOWN bootloader PRIVATE lockdown)

add_subdirectory_ifdef(CONFIG_SB_DEBUG_PORT_UART uart)
target_link_libraries_ifdef(CONFIG_SB_DEBUG_PORT_UART bootloader PRIVATE uart)

add_subdirectory_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT rtt)
target_link_libraries_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT bootloader PRIVATE segger_rtt)

add_dependencies(bootloader offsets_h)
target_link_libraries(bootloader PRIVATE zephyr_interface)


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



