
/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <errno.h>
#include <misc/__assert.h>
#include "bl0_crypto.h"

bool verify_hash(const uint8_t * data, uint32_t data_len, const uint8_t * expected);

bool verify_sig(const uint8_t * data, uint32_t data_len,
		const uint8_t * sig, const uint8_t * pk);


int crypto_root_of_trust(const uint8_t * pk,
                         const uint8_t * pk_hash,
                         const uint8_t * metadata,
                         const uint32_t  metadata_len,
                         const uint8_t * sig,
                         const uint8_t * fw,
                         const uint32_t  fw_len,
                         const uint8_t * fw_hash)
{
	if ((fw_hash < metadata) || ((fw_hash + CONFIG_SB_HASH_LEN) > (metadata + metadata_len))) {
		// LOG("fw_hash is not part of metadata");
		return -EINVAL;
	}

	if (!verify_hash(pk, CONFIG_SB_PUBLIC_KEY_LEN, pk_hash)) {
		return -EPKHASHINV;
	}

	if (!verify_sig(metadata, metadata_len, sig, pk)) {
		return -ESIGINV;
	}

	if (!verify_hash(fw, fw_len, fw_hash)) {
		return -EHASHINV;
	}

	return 0;
}
