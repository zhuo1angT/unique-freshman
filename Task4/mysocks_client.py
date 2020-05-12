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
import getopt
import sys

from utils import *
from proxy import *


def main(argv):

    server = ThreadingTCPServer(LOCAL_ADDR, Handler)
    print('Debug:: listening')
    server.serve_forever()  # 监听，建立好TCP连接后，为该连接创建新的socket和线程，并由处理类中的handle方法处理
    print(server)

    try:
        opts, args = getopt.getopt(
            argv, "s:p:b:l:k:", ["server_addr=", "server_port=", "local_addr=", "local_port=", "password="])

    except getopt.GetoptError:
        print("Error: mysocks_client.py, unrecognized argument")
        sys.exit(2)

    #  -s SERVER_ADDR  server address
    #  -p SERVER_PORT  server port, default: 8388
    #  -b LOCAL_ADDR   local binding address, default: 127.0.0.1
    #  -l LOCAL_PORT   local port, default: 1080
    #  -k PASSWORD     password
    # 处理 返回值options是以元组为元素的列表。

    server_addr = ""
    server_port = -1
    local_addr = ""
    local_port = -1
    password = ""

    for opt, arg in opts:
        if opt in ("-s", "--server_addr"):
            NotImplemented
        elif opt in ("-p", "--server_port"):
            NotImplemented
        elif opt in ("-b", "--local_addr"):
            NotImplemented
        elif opt in ("-l", "--local_port"):
            NotImplemented
        elif opt in ("-k", "--password"):
            NotImplemented

    # 打印 返回值args列表，即其中的元素是那些不含'-'或'--'的参数。
    for i in range(0, len(args)):
        print('参数 %s 为：%s' % (i + 1, args[i]))


if __name__ == '__main__':

    main(sys.argv[1:])
