import paho.mqtt.client as mqtt
import pexpect
import time
from datetime import datetime
import threading
import re
DEVICES = [] #00:13:50:04:00:4A and 00:20:40:0C:00:1F
INSTANCES = {}
CONNECTIONS = {}
THREADS = {}
directions = {
    "1" : "right",
    "2" : "left",
    "3" : "up",
    "4" : "down"
}

def get_MAC():
    with open("mac.txt",'r') as MAC_file:
        for address in MAC_file:
            DEVICES.append(re.sub('[\r\n]', '', address))

def send_mqtt(device, direct):
    mesg = "event {}".format(directions[direct])
    print("publishing", mesg)
    CONNECTIONS[device].publish("ecem119/2019s/hexiwear/{}".format(device), payload=mesg)
    time.sleep(0.1)

def stay_connected(device):
    print("Start to poll from {}".format(device))
    while True:
        try:
            INSTANCES[device].sendline("char-read-hnd 65")
            INSTANCES[device].expect("Characteristic value/descriptor: ", timeout=10)
            INSTANCES[device].expect("\r\n", timeout=10)
            direct = chr(INSTANCES[device].before[4])
            if direct in directions:
                send_mqtt(device, direct)
        except:
            print("reconnecting {}".format(device))
            connect_loop(device)

def connect_loop(device):
    INSTANCES[device].sendline("disconnect {0}".format(device))
    INSTANCES[device].sendline("connect {0}".format(device))
    while True:
        try:
            print("connecting to {}".format(device))
            INSTANCES[device].expect("Connection successful", timeout=5)
            break
        except:
            INSTANCES[device].sendline("disconnect {0}".format(device))
            INSTANCES[device].sendline("connect {0}".format(device))
 
def instantiate(device):
    print("instantiating {}".format(device))
    INSTANCES[device] = pexpect.spawn("gatttool -I")
    CONNECTIONS[device] = mqtt.Client(client_id=device, protocol=mqtt.MQTTv311, transport="tcp")
    CONNECTIONS[device].connect("broker.hivemq.com", port=1883)
    connect_loop(device)

def main():
    get_MAC()
    for dev in DEVICES:
        instantiate(dev)
        THREADS[dev] = threading.Thread(target=stay_connected, args=[dev])
        THREADS[dev].start()
    while True:
        time.sleep(6000)
    
if __name__ == '__main__':
        main()



#references used:
#https://dzone.com/articles/using-python-gatttool-and-bluetooth-low-energy-wit
#http://www.steves-internet-guide.com/into-mqtt-python-client/
