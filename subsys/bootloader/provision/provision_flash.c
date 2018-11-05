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


/* This symbol is stored at the page memory region named 'provision' in dts. 
 * It works as a placeholder here, and is filled by a python script 'provision.py'. */
__attribute__ ((section(".PROVISION_DATA"))) volatile const provision_flash_t provision_data;

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

