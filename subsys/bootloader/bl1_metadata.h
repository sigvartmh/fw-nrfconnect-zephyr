
/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/*
 * The package will consist of (firmware | (padding) | validation_info),
 * where the firmware contains the firmware_info at a predefined location. The
 * padding is present if the validation_info needs alignment. The
 * validation_info is not directly referenced from the firmware_info since the
 * validation_info doesn't actually have to be placed after the firmware.
 *
 * Putting the firmware info inside the firmware instead of in front of it
 * removes the need to consider the padding before the vector table of the
 * firmware. It will also likely make it easier to add all the info at compile
 * time.
 */

#include <stdint.h>
#include <stddef.h>

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#define INFO_VERSION     1 // This should go into KConfig.
#define HARDWARE_ID      // ? This should go into KConfig.
#define SIGNATURE_ALG    // ? This should go into KConfig.

#define MAGIC_COMMON          // ?
#define MAGIC_FIRMWARE_INFO   // ?
#define MAGIC_VALIDATION_INFO // ?
#define MAGIC_POINTER         // ?
#define MAGIC_COMPATIBILITY   (INFO_VERSION | (HARDWARE_ID << 8) | (SIG_ALG << 16)) // ?

#define MAGIC_LEN 12 // This should go into KConfig.

#define FIRMWARE_INFO_OFFSET 0x800 // The offset inside a firmware at which the firmware_info is found. This should go into KConfig

// Reference magics for comparing against real structs.
static const uint32_t firmware_info_magic[MAGIC_LEN / 4]   = {MAGIC_COMMON, MAGIC_FIRMWARE_INFO,   MAGIC_COMPATIBILITY};
static const uint32_t pointer_magic[MAGIC_LEN / 4]         = {MAGIC_COMMON, MAGIC_POINTER,         MAGIC_COMPATIBILITY};
static const uint32_t validation_info_magic[MAGIC_LEN / 4] = {MAGIC_COMMON, MAGIC_VALIDATION_INFO, MAGIC_COMPATIBILITY};

struct __packed bl1_firmware_info {
	uint8_t  magic[MAGIC_LEN];
	uint32_t firmware_size;      // Size without validation_info pointer and padding.
	uint32_t firmware_version;   // Monotonically increasing version counter.
	uint32_t firmware_address;   // The address of the start (vector table) of the firmware.
};

struct __packed bl1_validation_info {
	uint8_t  magic[MAGIC_LEN];
	uint32_t firmware_address;                     // The address of the start (vector table) of the firmware.
	uint8_t  firmware_hash[CONFIG_SB_HASH_LEN];    // The hash of the firmware.
	uint8_t  public_key[CONFIG_SB_PUBLIC_KEY_LEN]; // Public key to be used for signature verification. This must be checked against a trusted hash.
	uint8_t  signature[CONFIG_SB_SIGNATURE_LEN];   // Signature over the firmware as represented by the firmware_address and firmware_size in the firmware_info.
};

/*
 * Can be used to make the firmware discoverable in other locations, e.g. when
 * searching backwards. This struct would typically be constructed locally, so
 * it needs no version.
 */
struct bl1_validation_pointer {
	uint8_t                            magic[MAGIC_LEN];
	const struct bl1_validation_info * validation_info;
};


static inline bool memeq_32(const void * expected, const void * actual, uint32_t len)
{
	//ASSERT(!(len & 3)); // len is a multiple of 4
	uint32_t * expected_32 = (uint32_t *) expected;
	uint32_t * actual_32   = (uint32_t *) actual;
	for (uint32_t i = 0; i < (len / 4); i++) {
		if (expected_32[i] != actual_32[i]){
			return false;
		}
	}
	return true;
}

/*
 * Get a pointer to the firmware_info structure inside the firmware.
 */
static inline const struct bl1_firmware_info *
firmware_info_get(uint32_t firmware_address) {
	uint32_t finfo_addr = firmware_address + FIRMWARE_INFO_OFFSET;
	const struct bl1_firmware_info * finfo;

	finfo = (const struct bl1_firmware_info *)(finfo_addr);
	if (memeq_32(finfo->magic, firmware_info_magic, MAGIC_LEN)) {
		return finfo;
	}
	return NULL;
}

/*
 * Find the validation_info at the end of the firmware.
 */
static inline const struct bl1_validation_info *
validation_info_find(const struct bl1_firmware_info * finfo, uint32_t search_distance) {
	uint32_t vinfo_addr = finfo->firmware_address + finfo->firmware_size;
	const struct bl1_validation_info * vinfo;

	for (int i = 0; i <= search_distance; i++) {
		vinfo = (const struct bl1_validation_info *)(vinfo_addr + i);
		if (memeq_32(vinfo->magic, validation_info_magic, MAGIC_LEN)) {
			return vinfo;
		}
	}
	return NULL;
}
