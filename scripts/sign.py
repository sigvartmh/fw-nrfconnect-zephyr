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
from cryptography.hazmat.primitives.asymmetric import ec


# TODO Have this here, or as argument?
VALIDATION_METADATA_FORMAT = 'IIII'  # Dummy value, 4 x 32bit unsigned int.


def get_signature_bytes(private_key, hash_to_sign, chosen_hash, sign_type):
    if sign_type == "rsa":
        return private_key.sign(
            hash_to_sign,
            padding.PSS(
                mgf=padding.MGF1(hashes.SHA256()),
                salt_length=padding.PSS.MAX_LENGTH),
            utils.Prehashed(chosen_hash))
    elif sign_type == "ecdsa":
        return private_key.sign(
            hash_to_sign,
            ec.ECDSA(utils.Prehashed(chosen_hash))
        )
    else:
        raise RuntimeError("Invalid param: %s" % sign_type)


def get_private_key(pem_path):
    return serialization.load_pem_private_key(open(pem_path, 'rb').read(), password=None, backend=default_backend())


def get_hash(input_hex):
    # To be compliant with the RSA library we must use this hashing library.
    chosen_hash = hashes.SHA256()
    hasher = hashes.Hash(chosen_hash, default_backend())
    hasher.update(input_hex.tobinstr())
    return hasher.finalize(), chosen_hash


def get_public_key_bytes(private_key):
    return private_key.public_key().public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo)


def get_validation_data(pem_path, input_hex, magic_ascii, sign_type):
    hash_bytes, chosen_hash = get_hash(input_hex)
    private_key = get_private_key(pem_path)

    validation_bytes = bytes
    validation_bytes += struct.pack('B'*len(magic_ascii), magic_ascii)
    validation_bytes += struct.pack('I', len(input_hex))
    validation_bytes += hash_bytes
    validation_bytes += get_public_key_bytes(private_key)
    validation_bytes += get_signature_bytes(private_key, hash_bytes, chosen_hash, sign_type)

    return struct.pack(VALIDATION_METADATA_FORMAT, validation_bytes)


def sign_and_append_validation_data(pem_path, input_file, offset, output_file, magic_value, sign_type):
    ih = IntelHex(input_file)

    validation_data = get_validation_data(pem_path, ih, magic_value, sign_type)
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
                        help="Private key pem file.")
    parser.add_argument("-m", "--magic-value", required=True,
                        help="ASCII representation of magic value.")
    parser.add_argument("-s", "--sign-type", required=True, choices=['rsa', 'ecdsa'],
                        help="Signature type.")
    parser.add_argument("--output", required=False, default=None,
                        help="Output file name Default is to overwrite --input-file.")
    args = parser.parse_args()
    if args.output is None:
        args.output = args.input

    return args


def main():

    bytes = get_public_key_bytes(get_private_key('/home/haam/lol.txt'))

    args = parse_args()
    sign_and_append_validation_data(args.pem, args.input_files, args.output, args.offset, args.magic_value,
                                    args.sign_type)



if __name__ == "__main__":
    main()




