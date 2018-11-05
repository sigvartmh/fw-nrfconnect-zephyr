#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from ecdsa import SigningKey
from ecdsa.curves import NIST256p
import argparse


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate PEM file.",
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("-o", "--output", required=False, default="GENERATED_INSECURE.pem",
                        help="Output file name.")
    args = parser.parse_args()

    sk = SigningKey.generate(curve=NIST256p)
    with open(args.output, 'wb') as sk_file:
        sk_file.write(sk.to_pem())






