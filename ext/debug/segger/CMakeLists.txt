
zephyr_include_directories_ifdef(CONFIG_USE_SEGGER_RTT rtt)
zephyr_sources_ifdef(CONFIG_USE_SEGGER_RTT
	rtt/SEGGER_RTT.c
	rtt/SEGGER_RTT_zephyr.c
	)
zephyr_compile_definitions(SEGGER_RTT_SECTION=".rtt")
zephyr_compile_definitions(SEGGER_RTT_BUFFER_SECTION=".rtt.buffer")
zephyr_include_directories_ifdef(CONFIG_SEGGER_SYSTEMVIEW systemview)
zephyr_sources_ifdef(CONFIG_SEGGER_SYSTEMVIEW systemview/SEGGER_SYSVIEW.c)
