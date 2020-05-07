import socket
import base64


from utils import *


LOCAL_ADDR = ("127.0.0.1", 1080)
SERVER_ADDR = ("", 0)


class proxy_client:
    'a proxy connection'

    MAX_THREAD = 255
    METHODS = [0, 2]  # NO AUTHENTICATION REQUIRED & USERNAME / PASSWORD

    def init(self, remote_addr: tuple, key: str, local_addr: tuple, encrypt: int,
             name="default", timeout=10) -> None:
        self.name = name
        self.remote_addr = remote_addr
        self.key = key
        self.local_addr = local_addr
        self.encrypt = encrypt
        self.timeout = timeout

    def connect(self) -> None:
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(self.local_addr)
        self.socket.listen(proxy_client.MAX_THREAD)

        self.socket.connect(self.remote_addr)

    # |VER | NMETHODS | METHODS  |
    # +----+----------+----------+
    # | 1  |    1     | 1 to 255 |
    # +----+----------+----------+

    def socks_negotiate(self) -> None:
        packet_str = ""

        packet_str += "05"  # VER
        packet_str += to_hex(len(proxy_client.METHODS), 2)  # NMETHODS
        for method in proxy_client.METHODS:
            packet_str += to_hex(method, 2)  # METHODS

        self.socket.send(base64.b16decode(packet_str, True))

    # |VER | NUSERNAME | USERNAME | NPASSWORD | PASSWORD |
    # +----+-----------+----------+-----------|----------|
    # | 1  |    1      | 1 to 255 |    1      | 1 to 255 |
    # +----+-----------+----------+-----------|----------|

    def socks_verify(self, username: str, password: str):
        packet_str = ""

        packet_str += "05"  # VER
        packet_str += to_hex(len(username), 2)  # NUSERNAME
        packet_str += username  # USERNAME
        packet_str += to_hex(len(password), 2)  # NPASSWORD
        packet_str += password  # PASSWORD

        self.socket.send(base64.b16decode(packet_str, True))

    # +----+-----+-------+------+----------+----------+
    # |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
    # +----+-----+-------+------+----------+----------+
    # | 1  |  1  | X'00' |  1   | Variable |    2     |
    # +----+-----+-------+------+----------+----------+

    # Only implement the IPv4 part

    def socks_requst(self, address: tuple, cmd: int) -> None:

        atyp = 1

        packet_str = ""

        packet_str += "05"  # VER
        packet_str += to_hex(cmd, 2)  # CMD
        packet_str += "00"  # RSV
        packet_str += to_hex(atyp, 2)  # ATYP, currently only IPv4
        packet_str += to_hex(ipv4_to_int(address[0]), 8)  # ipv4 address
        packet_str += to_hex(address[1], 4)  # target port

        self.socket.send(base64.b16decode(packet_str, True))
