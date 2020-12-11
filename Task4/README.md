## PyProxy

Based on the SOCKS 5 protocol, implemented a light and elegant C/S architecture network proxy using Python, which supports data encryption, and authentication between client and server.



Use the following commands to use the proxy, all of these args are required.

```
python3 remote_server.py [srever_ip] [server_port] [password] # run in server
python3 local_host.py [local_ip] [local_port] [server_ip] [server_port] [password] # local machine
```

