#!/usr/bin/env python3
#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic


from intelhex import IntelHex

import argparse
import struct
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import utils
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import hashes


VALIDATION_METADATA_FORMAT = 'IIII'  # Dummy value, 4 x 32bit unsigned int.


def get_signature_bytes(private_key, hash_to_sign):
    return private_key.sign(
        hash_to_sign,
        padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()),
            salt_length=padding.PSS.MAX_LENGTH),
        utils.Prehashed(hash_to_sign))


def get_private_key(pem_path):
    return serialization.load_pem_private_key(open(pem_path, 'rb').read(), password=None, backend=default_backend())


def get_hash_bytes(input_hex):
    # To be compliant with the RSA library we must use this hashing library.
    chosen_hash = hashes.SHA256()
    hasher = hashes.Hash(chosen_hash, default_backend())
    hasher.update(input_hex.tobinstr())
    return hasher.finalize()


def get_public_key_bytes(private_key):
    return private_key.public_key().public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo)


def get_validation_data(pem_path, input_hex, magic_ascii):
    hash_bytes = get_hash_bytes(input_hex)
    private_key = get_private_key(pem_path)

    validation_bytes = bytearray
    validation_bytes += struct.pack('B'*len(magic_ascii), magic_ascii)
    validation_bytes += struct.pack('I', len(input_hex))
    validation_bytes += hash_bytes
    validation_bytes += get_public_key_bytes(private_key)
    validation_bytes += get_signature_bytes(private_key, hash_bytes)

    return struct.pack(VALIDATION_METADATA_FORMAT, validation_bytes)


def sign_and_append_validation_data(pem_path, input_file, offset, output_file, magic_value):
    ih = IntelHex(input_file)

    validation_data = get_validation_data(pem_path, ih, magic_value)
    validation_data_hex = IntelHex()
    validation_data_hex.puts(offset, validation_data)

    ih.merge(validation_data_hex)
    ih.write_hex_file(output_file)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Sign hex file and append metadata at specified offset.",
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("-i", "--input", required=True, nargs='+', help="Hex file to sign.")
    parser.add_argument("-o", "--offset", required=True,
                        help="Offset to store validation metadata at.")
    parser.add_argument("-p", "--pem", required=True,
                        help="Public key used to sign.")
    parser.add_argument("-m", "--magic-value", required=True,
                        help="ASCII representation of magic value.")
    parser.add_argument("--output", required=False, default=None,
                        help="Output file name Default is to overwrite --input-file.")
    args = parser.parse_args()
    if args.output is None:
        args.output = args.input

    return args


def main():
    args = parse_args()

    sign_and_append_validation_data(args.pem, args.input_files, args.output, args.offset, args.magic_value)


if __name__ == "__main__":
    main()




