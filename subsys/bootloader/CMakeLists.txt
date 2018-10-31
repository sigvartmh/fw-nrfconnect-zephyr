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
set(ARM ${ZEPHYR_BASE}/arch/arm/core)
set(CORTEX_M ${ARM}/cortex_m)

add_dependencies(bootloader offsets_h)

target_link_libraries(bootloader zephyr_interface)

target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_CUSTOM
  bootloader
  PRIVATE
  startup.c
  )

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

target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
	bootloader
	PRIVATE
	${CORTEX_M}/vector_table.h
	${CORTEX_M}/vector_table.S
	${CORTEX_M}/reset.S
	${CORTEX_M}/prep_c.c
	${ZEPHYR_BASE}/kernel/init.c
	${ARM}/fault_s.S
	${ARM}/swap_helper.S
	${CORTEX_M}/nmi.c
	${CORTEX_M}/nmi_on_reset.S
	)


target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
	bootloader
	PRIVATE
	startup/gcc_startup_nrf52840.S
	)


target_include_directories_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
	bootloader
	PRIVATE
	${CORTEX_M}
	)


# Nordic specific for startup code
target_sources_ifdef(CONFIG_SB_VENDOR_SYSTEM_INIT
  bootloader
  PRIVATE
  ${HAL}/nordic/nrfx/mdk/system_nrf52840.c
  )

# SEGGER files for DEBUG
target_include_directories_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT
  bootloader
  PRIVATE
  rtt
  ${STD_INCLUDE}
  )

target_sources_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT
  bootloader
  PRIVATE
  ${SEGGER}/rtt/SEGGER_RTT.c
  ${SEGGER}/rtt/SEGGER_RTT_printf.c
  rtt/SEGGER_RTT_Conf_sb.h
  rtt/SEGGER_RTT_sb.h
  )

target_sources_ifdef(CONFIG_SB_DEBUG_PORT_UART
	bootloader
	PRIVATE
	uart/uart.h	
	uart/uart.c
	)

target_include_directories_ifdef(CONFIG_SB_DEBUG_PORT_UART
	bootloader
	PRIVATE
	uart
	)

target_sources_ifdef(CONFIG_SECURE_BOOT_DEBUG
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
