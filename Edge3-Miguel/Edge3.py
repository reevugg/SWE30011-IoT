import paho.mqtt.client as mqtt
import json
import serial
import time

# IoT node
device = '/dev/ttyACM0'
arduino = serial.Serial(device, 9600)

# ThingsBoard MQTT broker details
THINGSBOARD_HOST = "thingsboard.cloud"  # Replace with your ThingsBoard host
ACCESS_TOKEN = "QdE4HrgTVqs0qW09FS2x"  # Replace with your device access token

# MQTT topic format for telemetry data
telemetry_topic = "v1/devices/me/telemetry"
attribute_topic = "v1/devices/me/attributes"
rpc_topic = "v1/devices/me/rpc/request/+"

# Create MQTT client and set access token
client = mqtt.Client()
client.username_pw_set(ACCESS_TOKEN)

# Connect callback
def on_connect(client, userdata, flags, rc):
    print("Connected to ThingsBoard with result code: " + str(rc))
    # Subscribe to attribute and RPC topics upon connection
    client.subscribe(attribute_topic)
    client.subscribe(rpc_topic)

# Publish callback
def on_publish(client, userdata, mid):
    print("Telemetry data published.")

# Message callback
def on_message(client, userdata, msg):
    print("Received message: " + str(msg.payload))
    # Process incoming message here
    topic = msg.topic
    payload = json.loads(msg.payload.decode())

    if topic == rpc_topic:
        # RPC request received
        requestId = topic.split("/")[-1]
        method = payload["method"]
        params = payload["params"]
        print(params)
        data = str(params)
        arduino.write(data.encode("utf-8"))

# Set callbacks
client.on_connect = on_connect
client.on_publish = on_publish
client.on_message = on_message

# Connect to ThingsBoard MQTT broker
client.connect(THINGSBOARD_HOST, 1883, 60)

# Function to send telemetry data to ThingsBoard
def send_telemetry_data(payload):
    # Convert telemetry data to JSON string
    payload_str = json.dumps(payload)
    print(payload_str)

    # Publish telemetry data
    client.publish(telemetry_topic, payload_str)

# Loop to send telemetry data
while True:
    # Process any incoming MQTT messages
    client.loop()

    # Delay before sending the next telemetry data
    time.sleep(1)  # Adjust the delay as needed
