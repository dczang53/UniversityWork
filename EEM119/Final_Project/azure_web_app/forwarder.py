from datetime import datetime
import sched
import asyncio
import threading
import re
import csv
import statistics


DEVICE = "00:13:50:04:00:4A" #and 00:20:40:0C:00:1F
THRESHOLDS = {
	"heart" : (),
	"pedometer" : (),
	"acceleration_xy" : (),
	"acceleration_z" : (),
	"temperature" : ()
}
heart = []
pedometer = []
acceleration = []
temperature = []
#SESSION = None
LAST_UPDATE = None
#OFFSET = 0 #any use for actual clock offset?

def twos_complement(hexstr,bits):
	value = int(hexstr,16)
	if value & (1 << (bits-1)):
		value -= 1 << bits
	return value

def connect_and_init():
	SESSION.sendline("disconnect {0}".format(DEVICE))
	SESSION.sendline("connect {0}".format(DEVICE))
	global OFFSET
	OFFSET = 0
	print("hello")
	while True:
		try:
			#print("connecting to {}".format(device))
			SESSION.expect("Connection successful", timeout=5)
			if OFFSET == 0:
				time1 = time.time()
				x = 0
				while x < 5:
					print("hi")
					dt = datetime.now()
					SESSION.sendline("char-write-req 61 0304{:02x}{:02x}{:02x}{:02x}0000000000000000000000000000".format((dt.hour&0xff),$
					SESSION.expect("Characteristic value was written successfully", timeout=10)
					x += 1
				time2 = time.time()
				OFFSET = ((time2 - time1) / 10)
			break
		except:
			SESSION.sendline("disconnect {0}".format(DEVICE))
			SESSION.sendline("connect {0}".format(DEVICE))

def main():
	#assign MAC address of DEVICE in line 9
	'''
	#(scrap this) initialize thresholds viw command line
	if(len(sys.argv) != 11):
		print("Invalid # of arguments")
		sys.exit(1)
	THRESHOLDS["heart"] = (sys.argv[1], sys.argv[2])
	THRESHOLDS["pedometer"] = (sys.argv[3], sys.argv[4])
	THRESHOLDS["acceleration"] = (sys.argv[5], sys.argv[6])
	THRESHOLDS["angular"] = (sys.argv[7], sys.argv[8])
	THRESHOLDS["temperature"] = (sys.argv[9], sys.argv[10])
	'''
	global SESSION
	SESSION = pexpect.spawn("gatttool -I")
	connect_and_init()
	last_transmission = time.time()
	with open('hh.csv', mode='w') as hh_file:
		hh_writer = csv.writer(hh_file)
		hh_writer.writerow(['time', 'heart', 'pedometer', 'acceleration', 'temperature'])
	while True:
		try:
			#initialize/update thresholds once per hour
			if (LAST_UPDATE == None ) or ((LAST_UPDATE - time.time()) > 3600):
				#request thresholds from azure files API
				THRESHOLDS["heart"] = 1
				THRESHOLDS["pedometer"] = 1
				THRESHOLDS["acceleration_xy"] = 1
				THRESHOLDS["acceleration_z"] = 1
				THRESHOLDS["temperature"] = 1
			#reading sensors data here (please modify
			#SESSION.before[...] to correct position)
			SESSION.sendline("char-read-hnd 0x3f")
			SESSION.expect("Characteristic value/descriptor: ", timeout=10)
			SESSION.expect("\r\n", timeout=10)
			data = SESSION.before[0:5]
			data1 = int(data,16)
			print(data)
			print(data1)
			if data1 > THRESHOLDS["heart"] or data1 < THRESHOLDS["heart"]:
				dt = datetime.now()
				timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
				#how to write to server here
			#global heart
			#heart  = []
			heart.append(data1)
			SESSION.sendline("char-read-hnd 0x28") #0x28 for Step
			SESSION.expect("Characteristic value/descriptor: ", timeout=5)
			SESSION.expect("\r\n", timeout=5)
			data = SESSION.before[0:5]
			data1 = int(data,16)
			print(SESSION.before[0:5])
			print(SESSION.before[0:5])
			if data1 > THRESHOLDS["pedometer"] or data1 < THRESHOLDS["pedometer"]:
				dt = datetime.now()
				timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
				#how to write to server here
			#global pedometer
			#pedometer = []
			pedometer.append(data1)
			SESSION.sendline("char-read-hnd 0x30") #0x30 for accel
			SESSION.expect("Characteristic value/descriptor: ", timeout=5)
			SESSION.expect("\r\n", timeout=5)
			data1 = SESSION.before[0:5]
			data2 = SESSION.before[6:11]
			data3 = SESSION.before[12:17]
			print(SESSION.before[0:5])
			print(SESSION.before[6:11])
			print(SESSION.before[12:17])
			data4 = twos_complement(data1,16)/256
			data5 = twos_complement(data2,16)/256
			data6 = twos_complement(data3,16)/256
			print(data4)
			print(data5)
			print(data6)
			data7 = (data4**2+data5**2)**0.5
			if data7 > THRESHOLDS["acceleration_xy"] or data7 < THRESHOLDS["acceleration_xy"]:
				dt = datetime.now()
				timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
				#how to write to server here
				if data6 > THRESHOLDS["acceleration_z"] or data6 < THRESHOLDS["acceleration_z"]:
					dt = datetime.now()
					timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
			#global acceleration_xy
			#acceleration_xy = []
			acceleration_xy.append(data7)
			#global acceleration_z
			#accleration_z = []
			accleration_z.append(data6)
			SESSION.sendline("char-read-hnd 0x43") #0x43 for temperature
			SESSION.expect("Characteristic value/descriptor: ", timeout=5)
			SESSION.expect("\r\n", timeout=5)
			data = SESSION.before[0:5]
			data1 = int(data,16)
			print(SESSION.before[0:5])
			print(data1)
			if data1 > THRESHOLDS["temperature"] or data1 < THRESHOLDS["temperature"]:
				dt = datetime.now()
				timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
				#how to write to server here
			temperature.append(data1)
			#sending data
			if (time.time() - last_transmission > 10):
				dt = datetime.now()
				timestamp = "{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second)
				#write to csv file
				with open('hh.csv', mode='a') as hh_file:
					hh_writer = csv.writer(hh_file)
					hh_writer.writerow([
						timestamp,
						statistics.mean(heart),
						statistics.mean(pedometer),
						statistics.mean(acceleration),
						statistics.mean(temperature)])
				#how to write to server here
				heart = []
				pedometer = []
				acceleration = []
				temperature = []
				last_transmission = time.time()
			time.sleep(abs(15 - time.time()) % 15)
		except:
			connect_and_init(device)

if __name__ == '__main__':
	main()
#still need to make sure that gattool session NEVER ends somehow? PLAN:
