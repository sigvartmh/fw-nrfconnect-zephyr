#include <nrf.h>
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define PIN_RXD (8)
#define PIN_TXD (6)
#include <generated_dts_board.h>


void uart_init(void){
  // Configure the UARTE with no flow control, one parity bit and 115200 baud rate
  NRF_UARTE0->CONFIG = (UART_CONFIG_HWFC_Disabled   << UART_CONFIG_HWFC_Pos) |
                       (UART_CONFIG_PARITY_Included << UART_CONFIG_PARITY_Pos);

  NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200 << UARTE_BAUDRATE_BAUDRATE_Pos;

  // Select TX and RX pins
  NRF_UARTE0->PSEL.TXD = CONFIG_UART_0_TX_PIN;
  NRF_UARTE0->PSEL.RXD = CONFIG_UART_0_RX_PIN;

}


void uart_printf(const char *__fmt, ...){
  va_list argptr;
  uint32_t len = 1;
  char _str[120];
  memset(&_str, 0x00, sizeof(_str));
  va_start(argptr, __fmt);
  len = vsprintf(_str, __fmt, argptr);
  va_end(argptr);

  // Enable the UART (starts using the TX/RX pins)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
  // Configure transmit buffer and start the transfer
  NRF_UARTE0->TXD.MAXCNT =  len;
  NRF_UARTE0->TXD.PTR = (uint32_t)&_str[0];
  NRF_UARTE0->TASKS_STARTTX = 1;

  // Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0)
  {
  }

  // Stop the UART TX
  NRF_UARTE0->TASKS_STOPTX = 1;
  // Wait until we receive the stopped event
  while (NRF_UARTE0->EVENTS_TXSTOPPED == 0);

  // Disable the UARTE (pins are now available for other use)
  NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
}
