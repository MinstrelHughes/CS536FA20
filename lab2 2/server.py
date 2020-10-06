import socket
import threading
import sys
import os
import time
# specify host, port, data amount, etc.

HOST = "127.0.0.1"
DATA_LEN = 2048
CODE = "utf-8" 
# dict and global var for HTTP headers
file_path_dict = {"html":"./files","jpg":"./files/files"}
header_dict_v11 = {"200":"HTTP/1.1 200 OK\r\n","400":"HTTP/1.1 400 Bad Request\r\n", "403":"HTTP/1.1 403 Forbidden\r\n","404":"HTTP/1.1 404 Not Found\r\n","505":"HTTP/1.1 505 Version Not Supported\r\n"}
header_dict_v10 = {"200":"HTTP/1.0 200 OK\r\n","400":"HTTP/1.1 400 Bad Request\r\n", "403":"HTTP/1.0 403 Forbidden\r\n","404":"HTTP/1.0 404 Not Found\r\n","505":"HTTP/1.0 505 Version Not Supported\r\n"}
content_type_dict = {"txt":"Content-Type: text\r\n","html":"Content-Type: html\r\n","jpg":"Content-Type: image/jpeg\r\n","gif":"Content-Type: image/gif\r\n"}
CONN_CLOSE = "Connection: close\r\n\r\n"

def tcplink(sock, address):
    print('Connection from %s:%s started.' % address) 

    data = sock.recv(DATA_LEN)
    res_header = ""
    res = b""
    file_path = ""
    file_type = ""
    header_code = None

    # parse request
    request = bytes.decode(data)
    request_split = request.split(' ')
    
    # for debug, print HTTP request
    print("HTTP Request")
    print(request_split)
    # get method, protocal, filename, file type from the request
    method = request_split[0]
    protocol = request_split[2]
    if protocol.find("1.0")!=-1:
        header_dict = header_dict_v10
    else:
        header_dict = header_dict_v11

    file_name = request_split[1]
    if file_name[-1]=="/":
        file_name += "index.html"

    if file_name.find(".")!=-1:
        file_type = file_name.split(".")[-1]

    file_path = "./"+file_name

    # if method is get, look for file
    if method=="GET":

        if os.path.isfile(file_path): # file existence
            if(os.access(file_path, os.R_OK)): # file permission
                with open(file_path, 'rb') as f:
                    res = f.read()
                header_code = "200"
            else:
                header_code = "403"
        else:
            header_code = "404"
    else:
        header_code = "400"
    # protocal unsupported 
    if protocol.find("1.1")==-1 and protocol.find("1.0")==-1:
        header_code = "505"
    
    res_header = header_dict[header_code]
    timer = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    res_header += "Date: {}\r\n".format(timer)
    res_header += "Server: webServer\r\n"
    # check file type supportance
    try:
        res_header += content_type_dict[file_type]
    except:
        res_header += "Content-Type: unsupported file type\r\n"
    res_header += CONN_CLOSE
    # print response header
    print("HTTP Response Header")
    print(res_header)
    # send data
    sock.send(res_header.encode())
    sock.send(res)
    sock.close()


def main(argv):
    if len(argv)!=1:
        print("Invalid arguments. Run on default port 3000")
        PORT = 3000  
    else:
        PORT = int(argv[0])

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





