#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0


from intelhex import IntelHex

import argparse
import os


def merge_hex_files(output, input_hex_files):
    ih = IntelHex()

    for hex_file_path in input_hex_files:
        to_merge = IntelHex(hex_file_path)

        # Since 'arm-none-eabi-objcopy' incorrectly inserts record type '03 - Start Segment Address', we need to remove
        # the start_addr to avoid conflicts when merging.
        to_merge.start_addr = None

        ih.merge(to_merge)
    ih.write_hex_file(output)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Merge hex files.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--input-files", required=True, nargs='+',
                        type=argparse.FileType('r', encoding='UTF-8'),
                        help="Space separated list of hex files to merge.")
    parser.add_argument("-o", "--output", required=False, default="merged.hex",
                        type=argparse.FileType('w', encoding='UTF-8'),
                        help="Output file name.")
    return parser.parse_args()


def main():
    args = parse_args()

    merge_hex_files(args.output, args.input_files)


if __name__ == "__main__":
    main()
