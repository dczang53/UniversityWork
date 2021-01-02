import RPi.GPIO as GPIO
import time
import signal
import sys
import socket
import struct

def close(signal, frame):
	print("\nTurning off ultrasonic distance detection...\n")
	GPIO.cleanup() 
	sys.exit(0)

if (len(sys.argv) != 2):
	print("Invalid # of arguments")
	sys.exit(1)

GPIO.setmode(GPIO.BCM)
pinTrigger = 18
pinEcho = 24
GPIO.setup(pinTrigger, GPIO.OUT)
GPIO.setup(pinEcho, GPIO.IN)

UDP_IP = sys.argv[1]
print(UDP_IP)
UDP_PORT = 8080
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
signal.signal(signal.SIGINT, close)

while True:
	print("Trying to get distance...")
	fail = 0
	GPIO.output(pinTrigger, True)
	time.sleep(0.005)
	GPIO.output(pinTrigger, False)
	startTime = time.time()
	stopTime = startTime
	time.sleep(0.0005)
	while 1 == GPIO.input(pinEcho):
		stopTime = time.time()
		if(stopTime - startTime) > 5:
			fail = 1
			break
	if fail == 1:
		print("Timed out... retrying...")
		continue
	TimeElapsed = stopTime - startTime
	distance = (TimeElapsed * 34300) / 2
	print(distance)
	sock.sendto(bytearray(struct.pack("f", distance)), (UDP_IP, UDP_PORT))
	time.sleep(0.05)

#SOURCES:
#https://www.geeksforgeeks.org/datagram-in-python/
