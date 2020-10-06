import socket
import threading
import sys
def main(argv):
    HOST = socket.gethostname()
    # specify host, port, data amount, etc.
    if len(argv)!=1:
        print("Invalid arguments. Run on default port 3000")
        PORT = 3000  
    else:
        PORT = int(argv[0])
    DATA_LEN = 2048
    CODE = "utf-8" 
    WELCOME_MSG = b"Welcome!" 

    # threading to handle a client function
    def tcplink(sock, address):
        print('Connection from %s:%s started.' % address)    
        while True:
            data = sock.recv(DATA_LEN)
            msg = data.decode(CODE)
            # receive data, make judgement on sepcial messages
            print("Received: %s" % msg)
            print("from %s:%s" %address)
            if not data or msg == 'QUIT':
                break
            if msg== "HELLO":
                sock.sendall(WELCOME_MSG)
            else:
                sock.sendall(data)

        sock.close()
        print('Connection from %s:%s closed.' % address)    


    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    # bind host and port
    sock.bind((HOST,PORT))
    # start to listen
    sock.listen(5)
    print("Server Start")
    while True:
        # accept from client and create a thread to handle it
        client, address = sock.accept()
        t = threading.Thread(target=tcplink, args=(client, address))
        t.start()
    client.close()

if __name__=="__main__":
    main(sys.argv[1:])





