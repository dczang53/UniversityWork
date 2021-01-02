import paho.mqtt.client as mqtt
import time

def on_message(client, userdata, message):
    print(str(message.payload.decode("utf-8")), "@", message.topic[-17:])

def main():
    client = mqtt.Client(client_id="client", protocol=mqtt.MQTTv311, transport="tcp")
    client.connect("broker.hivemq.com", port=1883)
    client.on_message=on_message
    client.loop_start()
    client.subscribe("ecem119/2019s/hexiwear/#")
    while True:
        time.sleep(6000)

if __name__ == '__main__':
	main()



#references used:
#http://www.steves-internet-guide.com/into-mqtt-python-client/
