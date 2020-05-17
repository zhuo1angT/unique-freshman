import socket
import socketserver
import logging
import struct
import select
import sys

from encrypt import *

addr = ''
port = 0
password = ''


class SocksProxyRemote(socketserver.StreamRequestHandler):

    SOCKS_VERSION = 5
    BUF_SIZE = 4 * 1024
    TIMEOUT = 10

    def exchange_loop(self, local, server, encrypt, decrypt):
        while True:
            # wait until client or remote is available for read
            r, w, x = select.select([local, server], [], [])

            if local in r:
                data = local.recv(SocksProxyRemote.BUF_SIZE)
                if server.send(decrypt(data)) <= 0:
                    break

            if server in r:
                data = server.recv(SocksProxyRemote.BUF_SIZE)
                if local.send(encrypt(data)) <= 0:
                    break

    def handle(self):
        logging.info('Accepting the connection from %s : %s' %
                     self.client_address)
        while True:
            npassword = struct.unpack("!B", self.request.recv(1))

            given_password = self.request.recv(npassword[0]).decode('utf-8')

            print(given_password)
            print(type(given_password))

            if given_password != password:
                self.server.close_request(self.request)
                return

            encryption = list(range(PASSWORD_LENGTH))
            for i in range(PASSWORD_LENGTH):
                encryption[i] = self.request.recv(1)

            def encrypt(data: bytes) -> bytes:
                ret = b''
                for i in range(len(data)):
                    ret += encryption[data[i]]
                return ret

            decrypt = gen_decryptor(encrypt)

            ver = struct.unpack("!B", decrypt(self.request.recv(1)))[0]
            cmd = struct.unpack("!B", decrypt(self.request.recv(1)))[0]
            padding = self.request.recv(1)
            atyp = struct.unpack("!B", decrypt(self.request.recv(1)))[0]

            assert ver == SocksProxyRemote.SOCKS_VERSION

            if atyp == 1:  # dotted decimal IPv4 address
                address = socket.inet_ntoa(decrypt(self.request.recv(4)))
            elif atyp == 3:  # Domain name
                domain_length = decrypt(self.request.recv(1))[0]  # int
                address = decrypt(self.request.recv(
                    domain_length)).decode('utf-8')  # str

            port = struct.unpack('!H', decrypt(self.request.recv(2)))[0]

            try:
                if cmd == 1:  # CONNECT
                    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    server.settimeout(SocksProxyRemote.TIMEOUT)
                    server.connect((address, port))
                    bind_address = server.getsockname()

                else:
                    self.server.close_request(self.request)
                # do *not* support the bind operation (the 2 == cmd case)
                # do *not* support UDP (the 3 == cmd case)

                addr = struct.unpack(
                    "!I", socket.inet_aton(bind_address[0]))[0]
                port = bind_address[1]

                reply = struct.pack("!BBBBIH", SocksProxyRemote.SOCKS_VERSION, 0, 0, 1,
                                    addr, port)

            except Exception as err:
                logging.error(err)
                # return connection refused error
                reply = struct.pack(
                    "!BBBBIH", SocksProxyRemote.SOCKS_VERSION, 5, 0, atyp, 0, 0)

            self.request.sendall(encrypt(reply))

            # establish data exchange
            if reply[1] == 0 and cmd == 1:
                self.exchange_loop(self.request, server, encrypt, decrypt)

            self.server.close_request(self.request)

        return


if __name__ == '__main__':
    addr = sys.argv[1]
    port = int(sys.argv[2])
    password = sys.argv[3]

    print(addr)
    print(port)
    print(password)
    with socketserver.ThreadingTCPServer((addr, port), SocksProxyRemote) as server:
        server.serve_forever()
