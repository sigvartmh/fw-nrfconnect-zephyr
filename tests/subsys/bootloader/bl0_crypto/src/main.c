/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#include "bl0_crypto.h"
#include "test_vector.c"


void test_verify_fw_cryptographic_root_of_trust(void)
{
	// Success.
	int retval = crypto_root_of_trust(pk, pk_hash, metadata, sizeof(metadata),
							 sig, firmware, sizeof(firmware), &metadata[1]);

	zassert_equal(0, retval, "retval was %d", retval);

	// pk_hash partly outside metadata.
	retval = crypto_root_of_trust(pk, pk_hash, metadata, CONFIG_SB_HASH_LEN,
							 sig, firmware, sizeof(firmware), &metadata[1]);

	zassert_equal(-EINVAL, retval, "retval was %d", retval);

	// pk_hash partly outside metadata.
	retval = crypto_root_of_trust(pk, pk_hash, &metadata[2], CONFIG_SB_HASH_LEN,
							 sig, firmware, sizeof(firmware), &metadata[1]);

	zassert_equal(-EINVAL, retval, "retval was %d", retval);

	// pk_hash totally outside metadata.
	retval = crypto_root_of_trust(pk, pk_hash, metadata, sizeof(metadata),
							 sig, firmware, sizeof(firmware), firmware_hash);

	zassert_equal(-EINVAL, retval, "retval was %d", retval);

	// pk doesn't match pk_hash.
	pk[1]++;
	retval = crypto_root_of_trust(pk, pk_hash, metadata, sizeof(metadata),
							 sig, firmware, sizeof(firmware), &metadata[1]);
	pk[1]--;

	zassert_equal(-EPKHASHINV, retval, "retval was %d", retval);

	// metadata doesn't match signature
	metadata[0]++;
	retval = crypto_root_of_trust(pk, pk_hash, metadata, sizeof(metadata) - 1,
							 sig, firmware, sizeof(firmware), &metadata[1]);
	metadata[0]--;

	zassert_equal(-ESIGINV, retval, "retval was %d", retval);

	// firmware doesn't match hash.
	firmware[1]++;
	retval = crypto_root_of_trust(pk, pk_hash, metadata, sizeof(metadata),
							 sig, firmware, sizeof(firmware), &metadata[1]);
	firmware[1]--;

	zassert_equal(-EHASHINV, retval, "retval was %d", retval);
}

void test_main(void)
{
	ztest_test_suite(test_bl0_crypto,
			 ztest_unit_test(test_verify_fw_cryptographic_root_of_trust));
	ztest_run_test_suite(test_bl0_crypto);
}
