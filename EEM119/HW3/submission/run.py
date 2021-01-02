import pexpect
import time
from datetime import datetime
import sched
import asyncio
import threading
import re

DEVICES = [] #00:13:50:04:00:4A and 00:20:40:0C:00:1F
INSTANCES = {}
OFFSETS = {} #TO BE DONE IN "CONNECT_AND_INIT"
THREADS = {}

def get_MAC():
    with open("mac.txt",'r') as MAC_file:
        for address in MAC_file:
            DEVICES.append(re.sub('[\r\n]', '', address))
            
#drift is almost unavoidable here as we cannot be too sure how long an update takes (and also consider disconnects)
def update_forever(device):
    while True:
        try:
            time.sleep(2)
            print("updating {}".format(device))
            dt = datetime.now()
            hour = dt.hour
            minute = dt.minute
            sec = dt.second
            ms = int(dt.microsecond/100000.0)
            print("{} {} {} {}".format(hour, minute, sec, ms))
            ms_offset = (int(OFFSETS[device]) % 1) * 100
            sec_offset = int(OFFSETS[device]) % 60
            ms = ms + ms_offset
            sec = sec + sec_offset
            if ms >= 1000:
                ms = ms % 1000
                sec = sec + 1
            if sec >= 60:
                sec = sec % 60
                minute = minute + 1
            if minute >= 60:
                minute = minute % 60
            if hour >= 24:
                hour = hour % 24
            INSTANCES[device].sendline("char-write-req 61 0304{:02x}{:02x}{:02x}{:02x}0000000000000000000000000000".format((hour&0xff), (minute&0xff), (sec&0xff), (ms&0xff)))
            INSTANCES[device].expect("Characteristic value was written successfully", timeout=10)
        except:
            print("reconnecting {}".format(device))
            connect_and_init(device)
 
def connect_and_init(device):
    INSTANCES[device].sendline("disconnect {0}".format(device))
    INSTANCES[device].sendline("connect {0}".format(device))
    while True:
        try:
            print("connecting to {}".format(device))
            INSTANCES[device].expect("Connection successful", timeout=5)
            if OFFSETS[device] == 0:
                time1 = time.time()
                x = 0
                while x < 5:
                    dt = datetime.now()
                    INSTANCES[device].sendline("char-write-req 61 0304{:02x}{:02x}{:02x}{:02x}0000000000000000000000000000".format((dt.hour&0xff), (dt.minute&0xff), (dt.second&0xff), (0xff&(int((dt.microsecond/100000.0))))))
                    INSTANCES[device].expect("Characteristic value was written successfully", timeout=10)
                    x += 1
                time2 = time.time()
                OFFSETS[device] = ((time2 - time1) / 10)
            break
        except:
            INSTANCES[device].sendline("disconnect {0}".format(device))
            INSTANCES[device].sendline("connect {0}".format(device))
 
def instantiate(device):
    print("instantiating {}".format(device))
    INSTANCES[device] = pexpect.spawn("gatttool -I")
    OFFSETS[device] = 0
    connect_and_init(device)
 
def main():
    get_MAC()
    for dev in DEVICES:
        print(dev)
        instantiate(dev)
        THREADS[dev] = threading.Thread(target = update_forever, args = [dev])
        THREADS[dev].start()
    while True:
        time.sleep((0.5 - time.time()) % 0.5)
        dt = datetime.now()
        print("{:02d}:{:02d}:{:02d}:{:02d}".format(dt.hour, dt.minute, dt.second, int(dt.microsecond / 1000)))
 
if __name__ == '__main__':
        main()



#reference used:
#https://dzone.com/articles/using-python-gatttool-and-bluetooth-low-energy-wit
