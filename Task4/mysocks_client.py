"""import socket

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.connect(('127.0.0.1', 9999))

for i in [b'lily', b'tom', b'jim']:
    s1.send(i)
    while True:
        r = s1.recv(1024)
        if r:
            print(r.decode('utf-8'))
        else:
            break
s1.send(b'exit')
s1.close()
"""


import socket
import base64

from utils import *


# Get an socket bind to "address", and listening to at most "max_thread" connections
def socks_init(address: tuple, max_thread: int) -> socket.socket:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(address)
    s.listen(max_thread)
    return s


def socks_send(socket: socket.socket, address: tuple, message: bytes):
    # s.send()
    return 0
