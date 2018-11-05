/*
 * Nordic Semiconductor ASA Copyright 2018
 * Sigvart M. Hovland
 */

/* General TODO: put stack after .bss and .data
 * +--------------+
 * | .bss + .data |
 * |--------------|
 * |	.stack	  |
 * |      |       |
 * |	  V		  |
 * |--------------|
 * | 	 End      |
 * +--------------+
 * And upwards growing heap if heap is present ontop of .bss and .data
 * This gives you MMU less stack protection for both .heap and .stack
 * +--------------+
 * |--------------|
 * |	  ^		  |
 * |	  |       |
 * |	.heap	  |
 * |--------------|
 * | .bss + .data |
 * |--------------|
 * |	.stack	  |
 * |      |       |
 * |	  V		  |
 * |--------------|
 * | 	 End      |
 * +--------------+
 * MMU Less stack and heap protection, but limited heap size
 * You can't resize the stack to account for more heap etc. so this
 * should be configurable. As you may  want the heap to grow towards
 * the stack for for small devices with low memory.
 */

#include <stdint.h>

/* Linker-defined symbols for addresses of flash and ram */

/* Block started by symbol, data container for statically allocated objects
 * Such as uninitialized objects both variables and constatns declared in
 * file scope and uninitialized static local variables
 * Short name BETTER SAVE SPACE(BSS)
 */
extern uint32_t __bss_start;
extern uint32_t __bss_end;
extern uint32_t __data_rom_start;
extern uint32_t __data_ram_start;
extern uint32_t __data_ram_end;
extern uint32_t _image_text_end;

/* C main function to be called from the reset_handler */
extern int main(void);
/* Device specific intialization functions for erratas and system clock setup */
extern void SystemInit(void);
/* Forward decleration for dummy handler */
void dummy_handler(void);

/* weak assign all interrupt handlers to dummy_handler */
#define ALIAS(name) __attribute__((weak, alias(name)))
void reset_handler(void);
void nmi_handler(void) ALIAS("dummy_handler");
void hard_fault_handler(void) ALIAS("dummy_handler");
#if defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
void mpu_fault_handler(void) ALIAS("dummy_handler");
void bus_fault_handler(void) ALIAS("dummy_handler");
void usage_fault_handler(void) ALIAS("dummy_handler");
void debug_monitor_handler(void) ALIAS("dummy_handler");
#if defined(CONFIG_ARM_SECURE_FIRMWARE)
void secure_fault_handler(void) ALIAS("dummy_handler");
#endif /* CONFIG_ARM_SECURE_FIRMWARE */
#endif /* CONFIG_ARMV7_M_ARMV8_M_MAINLINE */
void svc_handler(void) ALIAS("dummy_handler");
void pend_sv_handler(void) ALIAS("dummy_handler");
void sys_tick_handler(void) ALIAS("dummy_handler");


extern uint32_t __kernel_ram_start; // TODO: Find _end_of_stack symbol

/* TODO: Add vendor specific vectors to vector table */
void *core_vector_table[16] __attribute__((section(".exc_vector_table"))) = {
	&__kernel_ram_start + CONFIG_MAIN_STACK_SIZE,
	reset_handler,      //__reset
	nmi_handler,	// __nmi
	hard_fault_handler, //__hard_fault
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
	0,
	/* reserved */ //__reserved
	0,	     /* reserved */
	0,	     /* reserved */
	0,	     /* reserved */
	0,	     /* reserved */
	0,	     /* reserved */
	0,	     /* reserved */
	svc_handler,   //__svc
	0,	     /* reserved */
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
	mpu_fault_handler,     //__mpu_fault
	bus_fault_handler,     //__bus_fault
	usage_fault_handler,   //__usage_fault
#if defined(CONFIG_ARM_SECURE_FIRMWARE)
	secure_fault_handler,  //__secure_fault
#else
	0, /* reserved */
#endif /* CONFIG_ARM_SECURE_FIRMWARE */
	0,		       /* reserved */
	0,		       /* reserved */
	0,		       /* reserved */
	svc_handler,	   //__svc
	debug_monitor_handler, // debug_monitor
#else  /* ARM_ARCHITECTURE */
#error Unknown ARM architecture
#endif			 /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */
	0,		 /* reserved */
	pend_sv_handler, //__pendsv
#if defined(CONFIG_CORTEX_M_SYSTICK)
	sys_tick_handler, //_timer_int_handler
#else
	0,		       /* reserved */
#endif /* CONFIG_CORTEX_M_SYSTICK */
};

void POWER_CLOCK_IRQHandler(void) ALIAS("dummy_handler");
void RADIO_IRQHandler(void) ALIAS("dummy_handler");
void UARTE0_UART0_IRQHandler(void) ALIAS("dummy_handler");
void SPI0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) ALIAS("dummy_handler");
void SPI1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) ALIAS("dummy_handler");
void NFCT_IRQHandler(void) ALIAS("dummy_handler");
void GPIOTE_IRQHandler(void) ALIAS("dummy_handler");
void SAADC_IRQHandler(void) ALIAS("dummy_handler");
void TIMER0_IRQHandler(void) ALIAS("dummy_handler");
void TIMER1_IRQHandler(void) ALIAS("dummy_handler");
void TIMER2_IRQHandler(void) ALIAS("dummy_handler");
void RTC0_IRQHandler(void) ALIAS("dummy_handler");
void TEMP_IRQHandler(void) ALIAS("dummy_handler");
void RNG_IRQHandler(void) ALIAS("dummy_handler");
void ECB_IRQHandler(void) ALIAS("dummy_handler");
void CCM_AAR_IRQHandler(void) ALIAS("dummy_handler");
void WDT_IRQHandler(void) ALIAS("dummy_handler");
void RTC1_IRQHandler(void) ALIAS("dummy_handler");
void QDEC_IRQHandler(void) ALIAS("dummy_handler");
void COMP_LPCOMP_IRQHandler(void) ALIAS("dummy_handler");
void SWI0_EGU0_IRQHandler(void) ALIAS("dummy_handler");
void SWI1_EGU1_IRQHandler(void) ALIAS("dummy_handler");
void SWI2_EGU2_IRQHandler(void) ALIAS("dummy_handler");
void SWI3_EGU3_IRQHandler(void) ALIAS("dummy_handler");
void SWI4_EGU4_IRQHandler(void) ALIAS("dummy_handler");
void SWI5_EGU5_IRQHandler(void) ALIAS("dummy_handler");
void TIMER3_IRQHandler(void) ALIAS("dummy_handler");
void TIMER4_IRQHandler(void) ALIAS("dummy_handler");
void PWM0_IRQHandler(void) ALIAS("dummy_handler");
void PDM_IRQHandler(void) ALIAS("dummy_handler");
void MWU_IRQHandler(void) ALIAS("dummy_handler");
void PWM1_IRQHandler(void) ALIAS("dummy_handler");
void PWM2_IRQHandler(void) ALIAS("dummy_handler");
void SPIM2_SPIS2_SPI2_IRQHandler(void) ALIAS("dummy_handler");
void RTC2_IRQHandler(void) ALIAS("dummy_handler");
void I2S_IRQHandler(void) ALIAS("dummy_handler");
void FPU_IRQHandler(void) ALIAS("dummy_handler");
void USBD_IRQHandler(void) ALIAS("dummy_handler");
void UARTE1_IRQHandler(void) ALIAS("dummy_handler");
void QSPI_IRQHandler(void) ALIAS("dummy_handler");
void CRYPTOCELL_IRQHandler(void) ALIAS("dummy_handler");
void PWM3_IRQHandler(void) ALIAS("dummy_handler");
void SPIM3_IRQHandler(void) ALIAS("dummy_handler");

void *vendor_vector_table[] __attribute__ ((section(".gnu.linkonce.irq_vector_table"))) = {
POWER_CLOCK_IRQHandler,
RADIO_IRQHandler,
UARTE0_UART0_IRQHandler,
SPI0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler,
SPI1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler,
NFCT_IRQHandler,
GPIOTE_IRQHandler,
SAADC_IRQHandler,
TIMER0_IRQHandler,
TIMER1_IRQHandler,
TIMER2_IRQHandler,
RTC0_IRQHandler,
TEMP_IRQHandler,
RNG_IRQHandler,
ECB_IRQHandler,
CCM_AAR_IRQHandler,
WDT_IRQHandler,
RTC1_IRQHandler,
QDEC_IRQHandler,
COMP_LPCOMP_IRQHandler,
SWI0_EGU0_IRQHandler,
SWI1_EGU1_IRQHandler,
SWI2_EGU2_IRQHandler,
SWI3_EGU3_IRQHandler,
SWI4_EGU4_IRQHandler,
SWI5_EGU5_IRQHandler,
TIMER3_IRQHandler,
TIMER4_IRQHandler,
PWM0_IRQHandler,
PDM_IRQHandler,
0,
0,
MWU_IRQHandler,
PWM1_IRQHandler,
PWM2_IRQHandler,
SPIM2_SPIS2_SPI2_IRQHandler,
RTC2_IRQHandler,
I2S_IRQHandler,
FPU_IRQHandler,
USBD_IRQHandler,
UARTE1_IRQHandler,
QSPI_IRQHandler,
CRYPTOCELL_IRQHandler,
0,
0,
PWM3_IRQHandler,
0,
SPIM3_IRQHandler,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
};

void _bss_zero(uint32_t *dest, uint32_t *end)
{
	while (dest < (uint32_t *)&end) {
		*dest++ = 0;
	}
}

void _data_copy(uint32_t *src, uint32_t *dest, uint32_t *end)
{
	while (dest < (uint32_t *)&end) {
		*dest++ = *src++;
	}
}

void reset_handler(void)
{
	_bss_zero(&__bss_start, &__bss_end);
	_data_copy(&_image_text_end, &__data_ram_start, &__data_ram_end);
#if defined(CONFIG_SB_VENDOR_SYSTEM_INIT)
	SystemInit(); /* Create define for system INIT */
#endif /* CONFIG_SECURE_BOOT TODO: Find a way to use select defines? */
	main();
	while (1);
}

/* TODO: Find a way to redefine the entry point from __start to reset handler?
*/
void __start(void)
{
	reset_handler();
}

void dummy_handler(void)
{
	/* Hang on unexpected interrupts as it's considered a bug in the program
	*/
	while (1);
}
