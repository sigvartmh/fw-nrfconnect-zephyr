#include <stdint.h>
#include <nrf.h>
#include <generated_dts_board.h>


#ifdef CONFIG_SB_DEBUG_PORT_SEGGER_RTT
#include <SEGGER_RTT_sb.h>
#define debug_print(fmt, ...) do{if(CONFIG_SB_DEBUG_PORT_SEGGER_RTT){SEGGER_RTT_printf(0, fmt, __VA_ARGS__);}}while(0)
#elif defined(CONFIG_SB_DEBUG_PORT_UART)
#include "uart.h"
#define debug_print(fmt, ...) do{if(CONFIG_SB_DEBUG_PORT_UART){uart_printf(fmt, __VA_ARGS__);}}while(0)
#else
#define debug_print(...) do{}while(0)
#endif /* CONFIG_SB_DEBUG_PORT_SEGGER_RTT */

#ifdef CONFIG_SB_FLASH_LOCKDOWN
#include <lockdown.h>
#endif

#define LED1_GPIO (GPIO_LEDS_LED_0_GPIO_PIN)
#define LED2_GPIO (GPIO_LEDS_LED_1_GPIO_PIN)
#define LED3_GPIO (GPIO_LEDS_LED_2_GPIO_PIN)
#define LED4_GPIO (GPIO_LEDS_LED_3_GPIO_PIN)

#define BUTTON1_GPIO (GPIO_KEYS_BUTTON_0_GPIO_PIN)
#define BUTTON2_GPIO (GPIO_KEYS_BUTTON_1_GPIO_PIN)
#define BUTTON3_GPIO (GPIO_KEYS_BUTTON_2_GPIO_PIN)
#define BUTTON4_GPIO (GPIO_KEYS_BUTTON_3_GPIO_PIN)

#define BUTTON3 0x2041800
#define BUTTON1 0x3041000
#define BUTTON2 0x3040800
#define BUTTON4 0x1041800


#define EnablePrivilegedMode() __asm("SVC #0")

void config_led(uint32_t pin_num)
{
	NRF_GPIO->PIN_CNF[pin_num] =
		(GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos)
		| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
		| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
		| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
		| (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

void config_input(uint32_t pin_num)
{
	NRF_GPIO->PIN_CNF[pin_num] =
		(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos)
		| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
		| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
		| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
		| (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}


static void boot_from(uint32_t *address)
{
	if (CONTROL_nPRIV_Msk & __get_CONTROL()) {
		EnablePrivilegedMode();
	}

	__disable_irq();

	for (uint8_t i = 0; i < 8; i++){
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
	for (uint8_t i = 0; i < 8; i++){
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

	SysTick->CTRL = 0;

	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
			| SCB_SHCSR_MEMFAULTENA_Msk);

	if (CONTROL_SPSEL_Msk & __get_CONTROL()) {
		__set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
	}

	__DSB(); /* Force Memory Write before continuing */
	__ISB(); /* Flush and refill pipeline with updated premissions */

	SCB->VTOR = (uint32_t)address;

	__enable_irq();
	__set_MSP(address[0]);
	((void (*)(void))address[1])();
}

int main(void)
{
#if CONFIG_SB_FLASH_LOCKDOWN
	lock_area(FLASH_AREA_SECURE_BOOT_OFFSET, FLASH_AREA_SECURE_BOOT_SIZE);
#endif //CONFIG_SB_FLASH_LOCKDOWN
#if defined(CONFIG_SB_DEBUG_PORT_SEGGER_RTT)
	SEGGER_RTT_Init();
#elif defined(CONFIG_SB_DEBUG_PORT_UART)
	uart_init();
#endif /* CONFIG_SB_RTT */
	boot_from((uint32_t *)(0x00000000 + FLASH_AREA_APP_OFFSET));
	/* Unreachable */
	boot_from((uint32_t *)(0x00000000 + FLASH_AREA_S1_OFFSET));
	boot_from((uint32_t *)(0x00000000 + FLASH_AREA_APP_OFFSET));
	return 0;
}
