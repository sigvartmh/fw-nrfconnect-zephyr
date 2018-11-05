set(EXTS ${ZEPHYR_BASE}/ext)
set(NORDIC ${EXTS}/hal/nordic)
set(NRFX ${NORDIC}/nrfx)
set(HAL  ${NRFX}/hal)

add_library(nrfx_uart
	STATIC
	${NRFX}/drivers/src/nrfx_uart.c
	${NORDIC}/nrfx_glue.c
	uart_printf.c
	uart_printf.h
	)

add_library(uart
	STATIC
	uart.c
	uart.h
	)

add_compile_definitions(NRFX_UART_ENABLED)
add_compile_definitions(NRFX_UART0_ENABLED)
add_compile_definitions(NRFX_UART0_NO_INTERRUPTS)

target_link_libraries(uart PRIVATE zephyr_interface)
target_link_libraries(uart PRIVATE libc)
target_link_libraries(nrfx_uart PRIVATE zephyr_interface)

target_include_directories(uart
  PRIVATE
  .
  ${NRFX}/drivers/include
  )
