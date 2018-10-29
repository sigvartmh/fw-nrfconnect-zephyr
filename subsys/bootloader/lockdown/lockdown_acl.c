#include <nrf_acl.h>

void lock_area(uint32_t start, size_t length)
{
	nrf_acl_region_set(NRF_ACL,
			0,
			start,
			length,
			NRF_ACL_PERM_READ_NO_WRITE);
}
