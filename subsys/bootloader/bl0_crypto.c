
/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include "bootloader_crypto.h"

#define HASH_LEN 32

static bool verify_hash(uint8_t * data, uint32_t data_len, uint8_t * hash)
{
	// Oberon or MbedTLS or CryptoCell.
}


static bool verify_sig(uint8_t * data, uint32_t data_len,
		       uint8_t * sig, uint8_t * pk)
{
	// Oberon or MbedTLS or Cryptocell.
}


int verify_fw_cryptographic_root_of_trust(const uint8_t * pk,
                                          const uint8_t * pk_hash,
                                          const uint8_t * metadata,
                                          const uint32_t  metadata_len,
                                          const uint8_t * sig,
                                          const uint8_t * fw,
                                          const uint32_t  fw_len,
                                          const uint8_t * fw_hash)
{
	ASSERT(fw_hash >= metadata,
		"fw_hash must be part of signed metadata");
	ASSERT((fw_hash + HASH_LEN) <= (metadata + metadata_len),
		"fw_hash must be part of signed metadata");

	if (!verify_hash(pk, CONFIG_SB_SIG_LEN, pk_hash) {
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

