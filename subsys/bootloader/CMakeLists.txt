# Do not add using zephyr cmake extensions.
# This because our library should not be added to the 'ZEPHYR_LIBS' list,
# since it should not be linked to the zephyr elf itself.

# TODO: Add conditionals for debugging and SystemInit

add_library(bootloader 
	bootloader.c 
	)

set(EXTS ${ZEPHYR_BASE}/ext)
set(HAL ${EXTS}/hal)
set(SEGGER ${EXTS}/debug/segger)
set(STD_LIB_C ${ZEPHYR_BASE}/lib/libc/minimal)
set(STD_SOURCE ${STD_LIB_C}/source)
set(STD_INCLUDE ${STD_LIB_C}/include)

add_dependencies(bootloader offsets_h)

target_link_libraries(bootloader zephyr_interface)

target_sources_ifdef(CONFIG_SB_CUSTOM_BOOT
  bootloader
  PRIVATE
  startup.c
  )

if (CONFIG_SB_FLASH_LOCKDOWN)
  # Find out which lockdown should be used
  if (CONFIG_HAS_HW_NRF_ACL)
    set(LOCKDOWN_SRC lockdown/lockdown_acl.c)
  elseif (CONFIG_HAS_HW_BPROT)
    set(LOCKDOWN_SRC lockdown/lockdown_bprot.c)
  elseif (CONFIG_HAS_HW_NRF_SPU)
    set(LOCKDOWN_SRC lockdown/lockdown_spu.c)
  else()
    message(FATAL_ERROR "No lockdown backend selected.")
  endif()

  target_sources(
    bootloader
    PRIVATE
    ${LOCKDOWN_SRC})
endif ()

# Nordic specific for startup code
target_sources_ifdef(CONFIG_SB_SYSTEM_INIT
	bootloader
	PRIVATE
	${HAL}/nordic/nrfx/mdk/system_nrf52840.c
	)

# SEGGER files for DEBUG
target_include_directories_ifdef(CONFIG_SB_SEGGER_RTT
	bootloader
	PRIVATE
	rtt
	${STD_INCLUDE}
	)

target_sources_ifdef(CONFIG_SB_SEGGER_RTT
	bootloader
	PRIVATE
	${SEGGER}/rtt/SEGGER_RTT.c
	${SEGGER}/rtt/SEGGER_RTT_printf.c
	rtt/SEGGER_RTT_Conf_sb.h
	rtt/SEGGER_RTT_sb.h
	)

# CONFIG_USE_C_LIB
target_sources_ifdef(CONFIG_SB_SEGGER_RTT
	bootloader
	PRIVATE
	${STD_SOURCE}/stdlib/atoi.c
	${STD_SOURCE}/stdlib/strtol.c
	${STD_SOURCE}/stdlib/strtoul.c
	${STD_SOURCE}/stdlib/malloc.c
	${STD_SOURCE}/string/strncasecmp.c
	${STD_SOURCE}/string/strstr.c
	${STD_SOURCE}/string/string.c
	${STD_SOURCE}/stdout/prf.c
	${STD_SOURCE}/stdout/stdout_console.c
	${STD_SOURCE}/stdout/sprintf.c
	${STD_SOURCE}/stdout/fprintf.c
	)
