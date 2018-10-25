#include <stdint.h>
#include <nrf.h>
#include <generated_dts_board.h>


#ifdef CONFIG_SB_SEGGER_RTT
#include <SEGGER_RTT_sb.h>
#define debug_print(fmt, ...) do{if(CONFIG_SB_SEGGER_RTT){SEGGER_RTT_printf(0, fmt, __VA_ARGS__);}}while(0)
#elif defined(CONFIG_SB_UART)
#include "uart.h"
#define debug_print(fmt, ...) do{if(CONFIG_SB_UART){uart_printf(fmt, __VA_ARGS__);}}while(0)
#else
#define debug_print(...) do{}while(0)
#endif /* CONFIG_SB_SEGGER_RTT */

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

void led_init(void)
{
	config_led(LED1_GPIO);
	config_led(LED2_GPIO);
	config_led(LED3_GPIO);
	config_led(LED4_GPIO);
}

void button_init(void)
{
	config_input(BUTTON1_GPIO);
	config_input(BUTTON2_GPIO);
	config_input(BUTTON3_GPIO);
	config_input(BUTTON4_GPIO);
}


static void inline _delay(uint32_t volatile tmr){
	while(tmr--);
}


int main(void)
{
<<<<<<< HEAD
	uint32_t volatile input;
#if CONFIG_SB_FLASH_LOCKDOWN
	lock_area(FLASH_AREA_SECURE_BOOT_OFFSET, FLASH_AREA_SECURE_BOOT_SIZE);
#endif //CONFIG_SB_FLASH_LOCKDOWN
=======
>>>>>>> Add uart support and hold button restart boot functionality
	button_init();
	//debug_print("%s\r\n", "Start BL");
#ifdef CONFIG_SB_SEGGER_RTT
	SEGGER_RTT_Init();
#endif /* CONFIG_SB_SEGGER_RTT */
<<<<<<< HEAD

	/* TODO: Clean up button and led  configurations before jump */
	debug_print("%s\n","Bootloader started");
	/* Add small delay to let RTT print out */
	_delay(10000000);
	input = ((NRF_GPIO->IN >> BUTTON1_GPIO) & 1UL);
	if(input){
		debug_print("%s\n","Boot from area s0");
		_delay(10000000);
		boot_from((uint32_t *)(0x00000000 + FLASH_AREA_S0_OFFSET));
	}
	input = ((NRF_GPIO->IN >> BUTTON2_GPIO) & 1UL);
	if(input){
		debug_print("%s\n","Boot from area s1");
		_delay(10000000);
		boot_from((uint32_t *)(0x00000000 + FLASH_AREA_S1_OFFSET));
	}
	input = ((NRF_GPIO->IN >> BUTTON3_GPIO) & 1UL);
	if(input){
		debug_print("%s\n","Boot from app");
		_delay(10000000);
=======
#ifdef CONFIG_SB_UART
	uart_init();
#endif /* CONFIG_SB_UART */
	
	uint32_t volatile input = NRF_GPIO->IN;
	if(input == BUTTON1){
		debug_print("%s\n\r","Boot from area s0");
		boot_from((uint32_t *)(0x00000000 + FLASH_AREA_S0_OFFSET));
	}
	else if(input == BUTTON2){
		debug_print("%s\n\r","Boot from area s1");
		boot_from((uint32_t *)(0x00000000 + FLASH_AREA_S1_OFFSET));
	}
	else if(input == BUTTON3){
		debug_print("%s\n\r","Boot from app");
>>>>>>> Add uart support and hold button restart boot functionality
		boot_from((uint32_t *)(0x00000000 + FLASH_AREA_APP_OFFSET));
	}

	return 0;
}
