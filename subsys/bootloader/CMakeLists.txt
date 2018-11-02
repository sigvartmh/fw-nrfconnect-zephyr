# Do not add using zephyr cmake extensions.
# This because our library should not be added to the 'ZEPHYR_LIBS' list,
# since it should not be linked to the zephyr elf itself.

add_library(bootloader bootloader.c)

add_subdirectory(c_runtime_setup)
add_subdirectory(libc)
add_subdirectory(uart)
add_subdirectory(rtt)
add_dependencies(bootloader offsets_h)

target_link_libraries(bootloader PRIVATE zephyr_interface)
target_link_libraries(bootloader PRIVATE c_runtime_setup)
target_link_libraries(bootloader PRIVATE libc)
target_link_libraries(bootloader PRIVATE uart)
target_link_libraries(bootloader PRIVATE segger_rtt)


if (CONFIG_SB_FLASH_LOCKDOWN)
  target_include_directories(bootloader PRIVATE lockdown/include)
  # Find out which lockdown should be used
  if (CONFIG_HAS_HW_NRF_SPU)
    set(LOCKDOWN_SRC lockdown/lockdown_spu.c)
  elseif (CONFIG_HAS_HW_NRF_ACL)
    set(LOCKDOWN_SRC lockdown/lockdown_acl.c)
  elseif (CONFIG_HAS_HW_BPROT)
    set(LOCKDOWN_SRC lockdown/lockdown_bprot.c)
  else()
    message(FATAL_ERROR "No lockdown backend selected.")
  endif()

  target_sources(
    bootloader
    PRIVATE
    ${LOCKDOWN_SRC})
endif ()


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



