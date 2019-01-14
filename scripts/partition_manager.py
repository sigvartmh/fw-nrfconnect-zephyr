#!/usr/bin/env python3
#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic

import argparse
import json
from os import path


def generate_override(input_files, output_file_name):
    for f in input_files:
        cfg = json.load(f)
        out_path = path.join(path.dirname(f.name), output_file_name)
        open(out_path, 'w').write('''\
#undef CONFIG_FLASH_BASE_ADDRESS
#define CONFIG_FLASH_BASE_ADDRESS %s'''
                                  % cfg['address'])


def parse_args():
    parser = argparse.ArgumentParser(
        description="Parse given input configurations and generate override header files.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--input", required=True, type=argparse.FileType('r', encoding='UTF-8'), nargs="+",
                        help="List of JSON formatted config files.")
    parser.add_argument("-o", "--output", required=True, help="Output file name. Will be stored in same dir as input.")

    args = parser.parse_args()
    if args.output is None:
        args.output = args.input

    return args


def main():

    args = parse_args()

    generate_override(args.input, args.output)


if __name__ == "__main__":
    main()
