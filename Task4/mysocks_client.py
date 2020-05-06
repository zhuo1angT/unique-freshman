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

LOCAL_ADDR = ("127.0.0.1", 1080)
SERVER_ADDR = ("", 0)
MAX_THREAD = 255


def to_hex(value: int, length: int) -> str:
    hex_str = hex(value)
    if len(hex_str) - 2 < length:
        return "0" * (length - len(hex_str) + 2) + hex_str[2:]
    else:
        return hex_str[2:]


# Only work with standard dotted decimal ipv4 addresses
def ipv4_to_int(ipv4: str) -> int:
    index, cur, ret = 0, 0, 0
    while index < len(ipv4):
        if ipv4[index] != '.':
            cur *= 10
            cur += int(ipv4[index])
        else:
            ret <<= 8
            ret, cur = ret | cur, 0
        index += 1
    ret <<= 8
    ret, cur = ret | cur, 0
    return ret


# Get an socket bind to "address", and listening to at most "max_thread" connections
def socks_init(address: tuple, max_thread: int) -> socket.socket:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(address)
    s.listen(max_thread)
    return s


# |VER | NMETHODS | METHODS  |
# +----+----------+----------+
# | 1  |    1     | 1 to 255 |
# +----+----------+----------+
def socks_negotiation(socket: socket.socket, address: tuple, methods: tuple):

    packet_str = ""

    packet_str += "05"  # VER
    packet_str += to_hex(len(methods), 2)  # NMETHODS
    for method in methods:
        packet_str += to_hex(method, 2)  # METHODS

    s.send(base64.b16decode(packet_str, True))


# +----+-----+-------+------+----------+----------+
# |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
# +----+-----+-------+------+----------+----------+
# | 1  |  1  | X'00' |  1   | Variable |    2     |
# +----+-----+-------+------+----------+----------+

# Only implement the IPv4 part
def socks_require(socket: socket.socket, address: tuple, cmd: int, atyp: int):

    packet_str = ""

    packet_str += "05"  # VER
    packet_str += to_hex(cmd, 2)  # CMD
    packet_str += "00"  # RSV
    packet_str += to_hex(atyp, 2)  # ATYP, currently only IPv4
    packet_str += to_hex(ipv4_to_int(address[0]), 8)  # ipv4 address
    packet_str += to_hex(address[1], 4)  # target port

    print(packet_str)

    s.send(base64.b16decode(packet_str, True))


s = socks_init(LOCAL_ADDR, MAX_THREAD)
