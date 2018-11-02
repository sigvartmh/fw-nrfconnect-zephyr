set(ARM ${ZEPHYR_BASE}/arch/arm/core)
set(CORTEX_M ${ARM}/cortex_m)
set(EXTS ${ZEPHYR_BASE}/ext)
set(HAL ${EXTS}/hal)

add_library(c_runtime_setup STATIC ${ZEPHYR_BASE}/misc/empty_file.c)
target_link_libraries(c_runtime_setup PRIVATE zephyr_interface)
target_link_libraries(c_runtime_setup PRIVATE bootloader)

target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_CUSTOM
  c_runtime_setup
  PRIVATE
  startup.c
  )

target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
	c_runtime_setup
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

target_include_directories_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
	c_runtime_setup
	PRIVATE
	${CORTEX_M}
	)

target_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
	c_runtime_setup
	PRIVATE
	gcc_startup_nrf52840.S
	)

# Nordic specific for startup code
target_sources_ifdef(CONFIG_SB_VENDOR_SYSTEM_INIT
  c_runtime_setup
  PRIVATE
  ${HAL}/nordic/nrfx/mdk/system_nrf52840.c
  )



