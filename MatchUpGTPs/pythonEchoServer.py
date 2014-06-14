
from socket import *
import sys

def handleClient(sock):
  data = sock.recv(32)
  while data:
    sock.sendall(data)
    data = sock.recv(32)
  sock.close()

if len(sys.argv) != 2:
  print __doc__
else:
  sock = socket(AF_INET, SOCK_STREAM)
  sock.bind(('',int(sys.argv[1])))
  sock.listen(5)
  while 1:
    newsock, client_addr = sock.accept()
    print "Client connected:", client_addr
    handleClient(newsock)
