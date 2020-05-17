import time
import random
import struct


PASSWORD_LENGTH = 256


def random_init() -> None:
    random.seed(int(time.time()))


def gen_encryper():
    password = list(range(PASSWORD_LENGTH))
    random.shuffle(password)

    for index, val in enumerate(password):
        if index == val:
            return gen_encryper()

    def encrypt(data: bytes) -> bytes:
        ret = b''
        for i in range(len(data)):
            ret += struct.pack('B', password[data[i]])
        return ret

    return encrypt


def gen_decryptor(encrypt):
    decryptor = list(range(PASSWORD_LENGTH))
    for i in range(PASSWORD_LENGTH):
        val = ord(encrypt(int(i).to_bytes(length=1, byteorder='big')))
        decryptor[val] = i

    def decrypt(data) -> bytes:
        ret = b''
        print(data)
        for i in range(len(data)):
            ret += struct.pack('B', decryptor[data[i]])
        return ret

    return decrypt
