#include "provision.h"
#include "generated_dts_board.h"

#define PK_LEN (CONFIG_SB_PUBLIC_KEY_HASH_SIZE / 8)

/* To avoid giving a pointer to the provisioned public keys, copy
 * the key material to this buffer, and return the pointer to it. */
static uint8_t key_buf[PK_LEN];

typedef struct {
	uint32_t s0_address;
	uint32_t s1_address;
	uint8_t  pkd[CONFIG_SB_NUM_PUBLIC_KEY_HASHES * PK_LEN];
} provision_flash_t;

static const provision_flash_t * p_provision_data = (provision_flash_t *)FLASH_AREA_PROVISION_OFFSET;

uint32_t s0_address_read(void)
{
	return p_provision_data->s0_address;
}

uint32_t s1_address_read(void)
{
	return p_provision_data->s1_address;
}

const uint8_t * public_key_data_read(uint32_t key_index)
{
	uint32_t i = 0;
	for(i = 0; i < PK_LEN; ++i) {
		key_buf[i] = p_provision_data->pkd[key_index*PK_LEN + i];
	}
	return key_buf;
}

