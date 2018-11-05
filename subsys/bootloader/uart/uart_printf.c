#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <nrfx_config.h>
#include <nrfx_uart.h>
#include <generated_dts_board.h>

//#define PIN_RXD (8) CONFIG_UART_0_RX_PIN
//#define PIN_TXD (6) CONFIG_UART_0_TX_PIN
//#define NRFX_UART_DEFAULT_CONFIG_HWFC 0

nrfx_uart_t p_instance = NRFX_UART_INSTANCE(0);

void uart_init(void){
	nrfx_uart_config_t p_config = NRFX_UART_DEFAULT_CONFIG;
	p_config.pseltxd = CONFIG_UART_0_TX_PIN;
	p_config.pselrxd = CONFIG_UART_0_RX_PIN;
	p_config.baudrate = NRF_UART_BAUDRATE_115200;
	p_config.hwfc = 0;
	p_config.parity = 1;
	nrfx_uart_init(&p_instance, &p_config, NULL);
}

void uart_uninit(void){
	nrfx_uart_uninit(&p_instance);
}

void uart_printf(const char * __fmt, uint32_t len){
//void uart_printf(const char * __fmt, ...){
	/*
	va_list argptr;
	uint32_t len = 1;
	uint8_t _str[120];
	memset(&_str, 0x00, sizeof(_str));
	va_start(argptr, __fmt);
	len = vsprintf(_str, __fmt, argptr);
	va_end(argptr);
	*/
	nrfx_uart_tx(&p_instance, __fmt, len);
}

