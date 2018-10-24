/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nrf_acl.h"
#include <ztest.h>

static void test_nrf_acl_region_id_set(uint8_t region_id)
{
	nrf_acl_region_set(NRF_ACL, region_id, 0x1000, 0x1000, NRF_ACL_PERM_READ_NO_WRITE);
	zassert_equal(0x1000, nrf_acl_region_size_get(NRF_ACL, region_id), "Incorrect size read out");
	zassert_equal(0x1000, nrf_acl_region_address_get(NRF_ACL, region_id), "Incorrect address read out");
	zassert_equal(NRF_ACL_PERM_READ_NO_WRITE,
			nrf_acl_region_perm_get(NRF_ACL, region_id), "Incorrect permissions read out");
}

/**
 * @brief Test nrf_acl.h
 *
 * This test verifies the functionality of nrf_acl.h
 */
static void test_nrf_acl_region_set(void)
{
	uint32_t i = 0;
	for (i = 0; i < ACL_REGIONS_COUNT; ++i) {
		test_nrf_acl_region_id_set(i);
	}
}

void test_main(void)
{
	ztest_test_suite(nrf_acl_tests,
			ztest_unit_test(test_nrf_acl_region_set),
			);
	ztest_run_test_suite(nrf_acl_tests);
}
