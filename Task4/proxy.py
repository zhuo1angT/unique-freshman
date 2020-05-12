import socket
import base64
import threading

from socketserver import BaseRequestHandler, ThreadingTCPServer

from utils import *

LOCAL_ADDR = ("127.0.0.1", 1081)
SERVER_ADDR = ("", 0)

BUF_SIZE = 4 * 1024


class proxy_client:
    'a proxy connection'

    MAX_THREAD = 255
    METHODS = [0, 2]  # NO AUTHENTICATION REQUIRED & USERNAME / PASSWORD

    INITIALIZED = 0
    CONNECTED = 1
    NEGOTIATED = 2
    VERIFIED = 3

    def init(self, remote_addr: tuple, key: str, local_addr: tuple, encrypt: int,
             name="default", timeout=10) -> None:

        self.name = name
        self.remote_addr = remote_addr
        self.key = key
        self.local_addr = local_addr
        self.encrypt = encrypt
        self.timeout = timeout

        self.status = proxy_client.INITIALIZED

    def connect(self) -> None:
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(self.local_addr)
        self.socket.listen(proxy_client.MAX_THREAD)

        self.socket.connect(self.remote_addr)
        self.status = proxy_client.CONNECTED

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

        self.status = proxy_client.NEGOTIATED

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

        self.status = proxy_client.VERIFIED

    # +----+-----+-------+------+----------+----------+
    # |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
    # +----+-----+-------+------+----------+----------+
    # | 1  |  1  | X'00' |  1   | Variable |    2     |
    # +----+-----+-------+------+----------+----------+

    # Only implement the IPv4 part

    def socks_requst(self, address: tuple, cmd: int) -> None:

        atyp = 1  # ipv4

        packet_str = ""

        packet_str += "05"  # VER
        packet_str += to_hex(cmd, 2)  # CMD
        packet_str += "00"  # RSV
        packet_str += to_hex(atyp, 2)  # ATYP, currently only IPv4
        packet_str += to_hex(ipv4_to_int(address[0]), 8)  # ipv4 address
        packet_str += to_hex(address[1], 4)  # target port

        self.socket.send(base64.b16decode(packet_str, True))

    def socks_send(self, address: tuple, data: bytes) -> None:

        self.socket.send(data)
        self.socket.recv(BUF_SIZE)  # 4 KB


class Handler(BaseRequestHandler):
    def handle(self):
        address, pid = self.client_address
        print('%s connected!' % address)
        while True:
            data = self.request.recv(BUF_SIZE)
            if len(data) > 0:
                print('receive=', data.decode('utf-8'))
                # cur_thread = threading.current_thread()
                # response = '{}:{}'.format(cur_thread.ident,data)
                self.request.sendall('response'.encode('utf-8'))
                print('send:', 'response')
            else:
                print('close')
                break
