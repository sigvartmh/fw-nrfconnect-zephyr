#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from intelhex import IntelHex

import argparse
import struct
import os


def merge_hex_files(output, input_hex_files, verbose):
    ih = IntelHex()

    for hex_file_path in input_hex_files:
        if not os.path.isfile(hex_file_path):
            raise RuntimeError("%s is not a valid file path." % hex_file_path)
        to_merge = IntelHex(hex_file_path)
        if verbose:
            print("'%s' - Start address: %s" % (hex_file_path, to_merge.start_addr))

        ih.merge(to_merge, overlap='replace')
    ih.write_hex_file(output)


def generate_provision_hex_file(s0_address, s1_address, hashes, end_address, output):
    # Add addresses
    provision_data = struct.pack('II', s0_address, s1_address)

    num_bytes_for_hashes = 0

    # Convert from ascii-hex to int list
    for h in hashes:
        proper_hash = [int(h[x:x + 2], 16) for x in range(0, len(h), 2)]
        single_hash_format_string = "B" * len(proper_hash)
        num_bytes_for_hashes += len(proper_hash)
        provision_data += struct.pack(single_hash_format_string, *proper_hash)

    ih = IntelHex()
    ih.frombytes(provision_data, end_address-len(provision_data))
    ih.write_hex_file(output)


# Since cmake does not have access to DTS variables, fetch them manually.
def find_provision_memory_section(config_file):
    s0_address = 0
    s1_address = 0
    provision_address = 0

    with open(config_file, 'r') as lf:
        for line in lf.readlines():
            if "FLASH_AREA_S0_OFFSET" in line:
                s0_address = int(line.split('=')[1])
            elif "FLASH_AREA_S1_OFFSET" in line:
                s1_address = int(line.split('=')[1])
            elif "FLASH_AREA_PROVISION_OFFSET" in line:
                provision_address = int(line.split('=')[1])

    return s0_address, s1_address, provision_address


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate provision hex file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("--generated-conf-file", required=True, help="Generated conf file.")
    parser.add_argument("--public-key-hashes", required=True, nargs='+',
                        help="Space separated list of ascii hex formatted public key hashes.")
    parser.add_argument("-o", "--output", required=False, default="provision.hex",
                        help="Output file name.")
    return parser.parse_args()


def main():
    args = parse_args()

    s0_address, s1_address, end_address = find_provision_memory_section(args.generated_conf_file)
    generate_provision_hex_file(s0_address, s1_address, args.public_key_hashes, end_address, args.output)


if __name__ == "__main__":
    main()




