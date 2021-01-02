# importing socket module 
import socket 
  
UDP_IP = "169.254.9.16"
UDP_PORT = 8080
MESSAGE = "GeeksforGeeks"
  
print ("message:", MESSAGE) 

try:  
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
	sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))
except:
	print("fail to send")
