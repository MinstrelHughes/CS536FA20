import socket
import sys
# specify host, port, data amount and code style
def main(argv):
	if len(argv)==2:
		HOST = argv[0]
		PORT = int(argv[1])
	else:
		print("Invalid arguments. Run on default.")
		HOST = '127.0.0.1'  
		PORT = 3000  

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

if __name__=="__main__":
	main(sys.argv[1:])
