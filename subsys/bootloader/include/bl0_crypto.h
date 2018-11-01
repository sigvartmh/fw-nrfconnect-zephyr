#ifndef BOOTLOADER_CRYPTO_H__
#define BOOTLOADER_CRYPTO_H__

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stdint.h>
#include <stdbool.h>

// Placeholder defines. Values should be updated, if no existing errors can be
// used instead.
#define EPKHASHINV 1
#define ESIGINV    2
#define EHASHINV   3


/**
 * @brief Verify a signature using configured signature and SHA-256
 *
 * Verifies the public key against the public key hash, then verifies the hash
 * of the signed data against the signature using the public key.
 *
 * @param[in]  pk            Public key.
 * @param[in]  pk_hash       Expected hash of the public key. This is the root
 *			     of trust.
 * @param[in]  metadata      Signed metadata for the firmware.
 * @param[in]  metadata_len  Length of the metadata.
 * @param[in]  sig           Signature
 * @param[in]  fw            Firmware
 * @param[in]  fw_len        Length of firmware.
 * @param[in]  fw_hash       Expected hash of the firmware. This must be inside
 *			     the signed metadata to maintain root of trust.
 *
 * @retval 0            On success.
 * @retval -EINVAL      If fw_hash is not inside metadata.
 * @retval -EPKHASHINV  If pk_hash didn't match pk.
 * @retval -ESIGINV     If signature validation failed.
 * @retval -EHASHINV    If firmware hash didn't match firmware.
 *
 * @remark No parameter can be NULL.
 */
int crypto_root_of_trust(const uint8_t * pk,
                         const uint8_t * pk_hash,
                         const uint8_t * metadata,
                         const uint32_t  metadata_len,
                         const uint8_t * sig,
                         const uint8_t * fw,
                         const uint32_t  fw_len,
                         const uint8_t * fw_hash);

#endif

