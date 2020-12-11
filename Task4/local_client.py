import sys
import socket
import socketserver
import struct
import logging
import select

from encrypt import *

local_addr = ''
local_port = 0
remote_addr = ''
remote_port = 0
password = ''


class SocksProxyLocal(socketserver.StreamRequestHandler):

    SOCKS_VERSION = 5
    BUF_SIZE = 4 * 1024
    TIMEOUT = 10

    def get_methods(self, n):
        methods = []
        for i in range(n):
            methods.append(ord(self.request.recv(1)))
        return methods

    def exchange_loop(self, client: socket.socket, remote: socket.socket, encrypt, decrypt):
        while True:
            # wait until client or remote is available for read
            r, w, x = select.select([client, remote], [], [])

            if client in r:
                data = client.recv(SocksProxyLocal.BUF_SIZE)
                if remote.send(encrypt(data)) <= 0:
                    break

            if remote in r:
                data = remote.recv(SocksProxyLocal.BUF_SIZE)
                if client.send(decrypt(data)) <= 0:
                    break

    def handle(self):

        # logging.info('Accepting the connection from %s : %s' %
        # self.client_address)

        random_init()
        encrypt = gen_encryper()
        decrypt = gen_decryptor(encrypt)

        # self.request represents the socket object
        while True:
            # version identifier/method selection message
            greeting = self.request.recv(2)  # ver, nmethods
            ver, nmethods = struct.unpack("!BB", greeting)

            assert ver == SocksProxyLocal.SOCKS_VERSION and nmethods != 0

            methods = self.get_methods(nmethods)

            if 0 not in set(methods):
                self.server.close_request(self.request)
                return

            # send authentication message
            # no authentication required
            self.request.sendall(struct.pack(
                "!BB", SocksProxyLocal.SOCKS_VERSION, 0))

            # request
            ver, cmd, padding, atyp = struct.unpack(
                "!BBBB", self.request.recv(4))

            assert ver == SocksProxyLocal.SOCKS_VERSION

            if atyp == 1:  # dotted decimal IPv4 address
                address = socket.inet_ntoa(self.request.recv(4))
            elif atyp == 3:  # Domain name
                domain_length = self.request.recv(1)[0]
                address = self.request.recv(domain_length)

            port = struct.unpack('!H', self.request.recv(2))[0]

            # reply
            try:
                # do *not* support the bind operation (the 2 == cmd case)
                # do *not* support UDP (the 3 == cmd case)
                if cmd == 1:  # CONNECT
                    remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    remote.settimeout(SocksProxyLocal.TIMEOUT)
                    remote.connect((remote_addr, remote_port))
                    bind_address = remote.getsockname()

                else:
                    self.server.close_request(self.request)

                remote.send(struct.pack("!B", len(password)))
                remote.send(password.encode('utf-8'))

                for i in range(PASSWORD_LENGTH):
                    remote.send(encrypt(bytes([i])))

                remote.send(
                    encrypt(struct.pack("!BBBB", SocksProxyLocal.SOCKS_VERSION, cmd, 0, atyp)))

                if atyp == 1:  # dotted decimal IPv4 address
                    remote.send(encrypt(socket.inet_aton(address)))
                elif atyp == 3:  # Domain name
                    remote.send(encrypt(struct.pack("!B", domain_length)))
                    remote.send(encrypt(address))
                remote.send(encrypt(struct.pack("!H", port)))

                reply = decrypt(remote.recv(4))
                atyp = struct.unpack("!B", (bytes([reply[3]])))[0]

                if atyp == 1:
                    reply += decrypt(remote.recv(4))  # bytes
                elif atyp == 3:
                    domain_length = struct.unpack(
                        "!B", decrypt(remote.recv(1)))  # int
                    reply += struct.pack("!B", domain_length)
                    reply += decrypt(remote.recv(domain_length))  # bytes

            except Exception as err:
                # logging.error(err)
                # return connection refused error
                reply = struct.pack(
                    "!BBBBIH", SocksProxyLocal.SOCKS_VERSION, 5, 0, atyp, 0, 0)

            self.request.sendall(reply)

            # establish data exchange
            if reply[1] == 0 and cmd == 1:
                self.exchange_loop(self.request, remote, encrypt, decrypt)

            self.server.close_request(self.request)


if __name__ == '__main__':

    local_addr = sys.argv[1]
    local_port = int(sys.argv[2])
    remote_addr = sys.argv[3]
    remote_port = int(sys.argv[4])
    password = sys.argv[5]

    with socketserver.ThreadingTCPServer((local_addr, local_port), SocksProxyLocal) as server:
        server.serve_forever()
