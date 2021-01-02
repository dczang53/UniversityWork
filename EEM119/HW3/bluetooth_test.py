# Using Hexiwear with Python
import pexpect
import time
import sched
import asyncio
import threading

DEVICES = ["00:20:40:0C:00:1F"] #00:20:40:0C:00:1F
INSTANCES = {}

async def update(device):
    try:
        #write over bluetooth
    except:
        connect_and_init(device)

async def connect_and_init(device):
    INSTANCE[device].expect("Connection successful", timeout=5)
    while True:
        try:
            INSTANCE[device].sendline("connect {0}".format(device))
            #initialize shit here
            break
        except:
            INSTANCE[device].expect("Connection successful", timeout=5)

async def instantiate(device):
    INSTANCE[device] = pexpect.spawn("gatttool -I")
    connect_and_init(device)

async def main():
    #get_MAC()
    for dev in DEVICES:
        instantiate(dev)
        threading.Timer(2.0, update, dev).start()

if __name__ == '__main__':
	asyncio.run(main())
    

#execute at exact time
#disconnection in middle of function

#char-write-req 0x000c 56FFFFFF00f0aa





























# function to transform hex string like "0a cd" into signed integer
def hexStrToInt(hexstr):
    val = int(hexstr[0:2],16) + (int(hexstr[3:5],16)<<8)
    if ((val&0x8000)==0x8000): # treat signed 16bits
        val = -((val^0xffff)+1)
    return val
 
#while True:
# Accelerometer
child.sendline("char-read-hnd 0x30")
child.expect("Characteristic value/descriptor: ", timeout=10)
child.expect("\r\n", timeout=10)
print("Accel: "),
print(child.before),
print(float(hexStrToInt(child.before[0:5]))/100),
print(float(hexStrToInt(child.before[6:11]))/100),
print(float(hexStrToInt(child.before[12:17]))/100)
 
# Accelerometer
child.sendline("char-read-hnd 0x34")
child.expect("Characteristic value/descriptor: ", timeout=10)
child.expect("\r\n", timeout=10)
print("Gyro: "),
print(child.before),
print(float(hexStrToInt(child.before[0:5]))/100),
print(float(hexStrToInt(child.before[6:11]))/100),
print(float(hexStrToInt(child.before[12:17]))/100)
 
# Magnetometer
child.sendline("char-read-hnd 0x38")
child.expect("Characteristic value/descriptor: ", timeout=10)
child.expect("\r\n", timeout=10)
print("Magneto:"),
print(child.before),
print(hexStrToInt(child.before[0:5])),
print(hexStrToInt(child.before[6:11])),
print(hexStrToInt(child.before[12:17]))

#if __name__ == '__main__':
#	main()
