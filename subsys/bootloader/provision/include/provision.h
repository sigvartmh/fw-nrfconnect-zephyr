#ifndef PROVISION_H_
#define PROVISION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief Function for reading address of slot 0.
	 *
	 * @retval Address of slot 0.
	 */
	uint32_t s0_address_read(void);

	/**
	 * @brief Function for reading address of slot 1.
	 *
	 * @retval Address of slot 1.
	 */
	uint32_t s1_address_read(void);

	/**
	 * @brief Function for reading public key data.
	 *
	 * @param[in] key_index  Index of key.
	 *
	 * @retval Pointer to key data.
	 */
	 uint8_t * public_key_data_read(uint32_t key_index);

#ifdef __cplusplus
}
#endif

#endif /* PROVISION_H_ */
