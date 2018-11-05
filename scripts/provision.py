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


def generate_provision_hex_file(s0_address, s1_address, hashes, address, output):
    # Add addresses
    provision_data = struct.pack("II", s0_address, s1_address)

    proper_hashes = list()

    # Convert from ascii-hex to int list
    for h in hashes:
        proper_hash = [int(h[x:x + 2], 16) for x in range(0, len(h), 2)]
        single_hash_format_string = "B" * len(proper_hashes[0])
        provision_data += struct.pack(single_hash_format_string, *proper_hash)

    ih = IntelHex()
    ih.frombytes(provision_data, address)
    ih.write_hex_file(output)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate provision hex file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("--address", required=True, help="Where should provision data be stored.")
    parser.add_argument("--s0-address", required=True)
    parser.add_argument("--s1-address", required=True)
    parser.add_argument("--public-key-hashes", required=True, nargs='+',
                        help="Space separated list of ascii hex formatted public key hashes.")
    parser.add_argument("-o", "--output", required=False, default="provision.hex",
                        help="Output file name.")
    return parser.parse_args()


def main():
    args = parse_args()

    generate_provision_hex_file(args.s0_address, args.s1_address, args.public_key_hashes, args.address, args.output)


if __name__ == "__main__":
    main()




