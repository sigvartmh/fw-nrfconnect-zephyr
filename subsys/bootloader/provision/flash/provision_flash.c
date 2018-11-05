#include "provision.h"
#include "generated_dts_board.h"

#define PK_SIZE 128 /* TODO take these two out to kconf */
#define NUM_PK 5

/* To avoid giving a pointer to the provisioned public keys, copy
 * the key material to this buffer, and return the pointer to it. */
static uint8_t key_buf[PK_SIZE];

typedef struct {
	uint32_t s0_address;
	uint32_t s1_address;
	uint8_t  pkd[NUM_PK * PK_SIZE];
} provision_flash_t;


/* This symbol is stored at the page memory region named 'provision' in dts. */
__attribute__ ((section(".PROVISION_DATA"))) volatile const provision_flash_t provision_data = {
	.s0_address = CONFIG_FLASH_BASE_ADDRESS + FLASH_AREA_S0_OFFSET,
	.s1_address = CONFIG_FLASH_BASE_ADDRESS + FLASH_AREA_S1_OFFSET,
	.pkd = {0}
};

uint32_t s0_address_read(void)
{
	return provision_data.s0_address;
}

uint32_t s1_address_read(void)
{
	return provision_data.s1_address;
}

const uint8_t * public_key_data_read(uint32_t key_index)
{
	uint32_t i = 0;
	for(i = 0; i < PK_SIZE; ++i) {
		key_buf[i] = provision_data.pkd[key_index*PK_SIZE + i];
	}
	return key_buf;
}

