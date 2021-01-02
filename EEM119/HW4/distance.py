import RPi.GPIO as GPIO
import time
import signal
import sys

# use Raspberry Pi board pin numbers
GPIO.setmode(GPIO.BCM)

# set GPIO Pins
pinTrigger = 18
pinEcho = 24

def close(signal, frame):
	print("\nTurning off ultrasonic distance detection...\n")
	GPIO.cleanup() 
	sys.exit(0)

signal.signal(signal.SIGINT, close)

# set GPIO input and output channels
GPIO.setup(pinTrigger, GPIO.OUT)
GPIO.setup(pinEcho, GPIO.IN)

while True:
	print("Trying to get distance...")
	# set Trigger to HIGH
	fail = 0
	GPIO.output(pinTrigger, True)
	# set Trigger after 0.01ms to LOW
	time.sleep(0.005)
	GPIO.output(pinTrigger, False)
	startTime = time.time()
	stopTime = startTime
	time.sleep(0.0005)
	# save time of arrival
	while 1 == GPIO.input(pinEcho):
		stopTime = time.time()
		if(stopTime - startTime) > 5:
			fail = 1
			break
	if fail == 1:
		print("Timed out... retrying...")
		continue
	# time difference between start and arrival
	TimeElapsed = stopTime - startTime
	# multiply with the sonic speed (34300 cm/s)
	# and divide by 2, because there and back
	distance = (TimeElapsed * 34300) / 2

	print ("Distance: %.1f cm" % distance)
	time.sleep(0.05)
