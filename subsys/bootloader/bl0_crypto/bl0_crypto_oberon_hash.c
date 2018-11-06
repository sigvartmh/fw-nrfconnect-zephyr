
/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stdint.h>
#include <stdbool.h>
#include "occ_sha256.h"
#include "occ_constant_time.h"


bool verify_hash(const uint8_t * data, uint32_t data_len, const uint8_t * expected)
{
	uint8_t hash[CONFIG_SB_HASH_LEN];
	occ_sha256(hash, data, data_len);
	return occ_constant_time_equal(expected, hash, CONFIG_SB_HASH_LEN);
}
