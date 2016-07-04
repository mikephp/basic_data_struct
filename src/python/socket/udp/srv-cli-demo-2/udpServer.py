from socket import *
from time import ctime

HOST = ''
# convert to network port
PORT = htons(514)
#PORT = 20123
BUFSIZ = 128
ADDR = (HOST, PORT)

udpServer = socket(AF_INET, SOCK_DGRAM)
# port reuse
udpServer.setsockopt(SOL_SOCKET,SO_REUSEADDR,1)
udpServer.bind(ADDR)

while True:
    print 'waiting for message...'

    data, addr = udpServer.recvfrom(BUFSIZ)

    udpServer.sendto('[%s] %s' % (ctime(), data), addr)
    print '...received from and returned to:', addr,data

udpServer.close()
