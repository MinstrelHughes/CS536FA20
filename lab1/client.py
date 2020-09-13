import socket
HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)
DATA_LEN = 2048
CODE = "utf-8" 

sock = socket.socket()
server_addr = (HOST,PORT)

try:
	sock.connect(server_addr)
	sent_data = raw_input("Input> ")
	while sent_data!="QUIT" and sent_data:
		sock.sendall(sent_data.encode(CODE))
		received_data = sock.recv(DATA_LEN)
		print "Received: ", received_data.decode(CODE)
		sent_data = raw_input("Input> ")
except socket.gaierror as e:
	print str(e)
except Exception as e:
	print e
finally:
	sock.close()
