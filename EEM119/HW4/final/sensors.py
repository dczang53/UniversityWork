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
pinTrigger1 = 18
pinEcho1 = 24
pinTrigger2 = 5
pinEcho2 = 13
GPIO.setup(pinTrigger1, GPIO.OUT)
GPIO.setup(pinEcho1, GPIO.IN)
GPIO.setup(pinTrigger2, GPIO.OUT)
GPIO.setup(pinEcho2, GPIO.IN)

UDP_IP = sys.argv[1]
print(UDP_IP)
UDP_PORT = 8080
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
signal.signal(signal.SIGINT, close)

#log_file = open("read_values.csv",'w')
#log_file.write("time,sensor1,sensor2\n")

while True:
	#print("Trying to get distance...")
	fail = 0
	GPIO.output(pinTrigger1, True)
	GPIO.output(pinTrigger2, True)
	time.sleep(0.005)
	GPIO.output(pinTrigger1, False)
	GPIO.output(pinTrigger2, False)
	startTime = time.time()
	stopTime1 = startTime
	stopTime2 = startTime
	time.sleep(0.0005)
	while 1 == GPIO.input(pinEcho1) and 1 == GPIO.input(pinEcho2):
		stopTime1 = time.time()
		stopTime2 = stopTime1
		if(stopTime1 - startTime) > 3:
			fail = 1
			break
	if 1 == GPIO.input(pinEcho1):
		while 1 == GPIO.input(pinEcho1):
			stopTime1 = time.time()
			if(stopTime1 - startTime) > 3:
				fail = 1
				break
	else:
		while 1 == GPIO.input(pinEcho2):
			stopTime2 = time.time()
			if(stopTime2 - startTime) > 3:
				fail = 1
				break
	if fail == 1:
		print("Timed out... retrying...")
		continue
	TimeElapsed1 = stopTime1 - startTime
	TimeElapsed2 = stopTime2 - startTime
	distance1 = (TimeElapsed1 * 34300) / 2
	distance2 = (TimeElapsed2 * 34300) / 2
	#print(distance1)
	#print(distance2)
	#log_file.write("{},{},{}\n".format(time.time(), distance1, distance2))
	sock.sendto(bytearray(struct.pack("f" * 2, distance1, distance2)), (UDP_IP, UDP_PORT))
	time.sleep(0.025)

#NOTES:
#to run, input IP address of computer running Pong as first and only argument
#again, to get IP address, use "hostname -I"

#SOURCES:
#https://github.com/leon-anavi/rpi-examples/blob/master/HC-SR04/python/distance.py
#https://www.geeksforgeeks.org/datagram-in-python/
