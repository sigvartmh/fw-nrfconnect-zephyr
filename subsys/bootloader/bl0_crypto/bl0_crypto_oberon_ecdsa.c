
/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stdint.h>
#include <stdbool.h>
#include "occ_ecdsa_p256.h"

bool verify_sig(const uint8_t * data, uint32_t data_len,
		const uint8_t * sig, const uint8_t * pk)
{
	int retval = occ_ecdsa_p256_verify(sig, data, data_len, pk);
	return retval == 0;
}
