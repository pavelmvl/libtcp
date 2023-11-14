#!/usr/bin/env python3

import socket
import sys
import time

writeFlag = False
writeCount = 0

if (len(sys.argv) > 2):
    writeFlag = True
    filePath = sys.argv[1]
    file = open(filePath, "wb")
    count = int(sys.argv[2])
    if count > 10:
        count = 10

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('127.0.0.1', 9999)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

try:

    # Look for the response
    amount_received = 0

    while True:
        start = time.time()
        data = sock.recv(1536000*4, socket.MSG_WAITALL)
        finish = time.time()
        amount_received = len(data)
        if amount_received <= 0:
            break
        if writeFlag:
            file.write(data)
            if writeCount < count:
                writeCount += 1
            else:
                break
        print('received {}, speed: {} Mbs'.format(amount_received, amount_received * 8 / (finish - start) / 1024 / 1024))
        #print('received {!r}'.format(data))

finally:
    print('closing socket')
    sock.close()
    if writeFlag:
        file.close()
