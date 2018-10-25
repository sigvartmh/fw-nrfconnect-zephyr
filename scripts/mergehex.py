#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from intelhex import IntelHex

import argparse
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


def parse_args():
    parser = argparse.ArgumentParser(
        description="Merge hex files. Start address will be dictated by latest input file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--input-files", required=True, nargs='+',
                        help="Comma separated list of hex files to merge.")
    parser.add_argument("-o", "--output", required=False, default="merged.hex",
                        help="Output file name.")
    parser.add_argument("-v", "--verbose", required=False, default=False,
                        help="Enable verbose mode.")
    return parser.parse_args()


def main():
    args = parse_args()

    merge_hex_files(args.output, args.input_files, args.verbose)


if __name__ == "__main__":
    main()




