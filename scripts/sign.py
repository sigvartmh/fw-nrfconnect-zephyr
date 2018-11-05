#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from intelhex import IntelHex

import hashlib
import argparse
import struct
from ecdsa import SigningKey
from ecdsa.keys import sigencode_string


def get_signature_bytes(private_key, hash_to_sign):
    return private_key.sign(hash_to_sign, hashfunc=hashlib.sha256, sigencode=sigencode_string)


def get_private_key(pem_file):
    return SigningKey.from_pem(pem_file.read())


def get_hash(input_hex):
    m = hashlib.sha256()
    m.update(input_hex.tobinstr())
    return m.digest()


def get_public_key_bytes(private_key):
    return private_key.get_verifying_key().to_string()


def get_validation_data(pem_file, input_hex, magic_value):
    hash_bytes = get_hash(input_hex)
    private_key = get_private_key(pem_file)

    validation_bytes = struct.pack('B' * (len(magic_value) // 2),
                                   *[int(magic_value[x:x + 2], 16) for x in range(0, len(magic_value), 2)])
    validation_bytes += struct.pack('I', input_hex.addresses()[0])
    validation_bytes += hash_bytes
    validation_bytes += struct.pack('I', len(input_hex))
    validation_bytes += get_public_key_bytes(private_key)
    validation_bytes += get_signature_bytes(private_key, hash_bytes)

    return validation_bytes


def sign_and_append_validation_data(pem_file, input_file, offset, output_file, magic_value):
    ih = IntelHex(input_file)
    ih.start_addr = None  # OBJCOPY incorrectly inserts x86 specific records, remove the start_addr as it is wrong.

    minimum_offset = ((ih.maxaddr() // 4) + 1) * 4
    if offset != 0 and offset < minimum_offset:
        raise RuntimeError("Incorrect offset, must be bigger than %x" % minimum_offset)

    validation_data = get_validation_data(pem_file=pem_file,
                                          input_hex=ih,
                                          magic_value=magic_value)
    validation_data_hex = IntelHex()

    # If no offset is given, append metadata right after input hex file (word aligned).
    if offset == 0:
        offset = minimum_offset

    validation_data_hex.frombytes(validation_data, offset)

    ih.merge(validation_data_hex)
    ih.write_hex_file(output_file)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Sign hex file and append metadata at specified offset.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--input", required=True, type=argparse.FileType('r', encoding='UTF-8'),
                        help="Hex file to sign.")
    parser.add_argument("-o", "--offset", required=False, type=int,
                        help="Offset to store validation metadata at.", default=0)
    parser.add_argument("-p", "--pem", required=True, type=argparse.FileType('r', encoding='UTF-8'),
                        help="Private key pem file.")
    parser.add_argument("-m", "--magic-value", required=True,
                        help="ASCII representation of magic value.")
    parser.add_argument("--output", required=False, default=None, type=argparse.FileType('w'),
                        help="Output file name Default is to overwrite --input.")

    args = parser.parse_args()
    if args.output is None:
        args.output = args.input

    return args


def main():

    args = parse_args()
    sign_and_append_validation_data(pem_file=args.pem,
                                    input_file=args.input,
                                    offset=args.offset,
                                    output_file=args.output,
                                    magic_value=args.magic_value)


if __name__ == "__main__":
    main()




