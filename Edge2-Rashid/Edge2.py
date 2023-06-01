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
        data =params
        arduino.write(data.encode("utf-8"))
        # Convert the received string to an integer
        try:
            value = int(params)
        except ValueError:
            print("Invalid value received from RPC:", params)
            return

        # Send the converted integer to Arduino over serial
        #arduino.write(str(value).encode())

          

# Set callbacks
client.on_connect = on_connect
client.on_publish = on_publish
client.on_message = on_message

# Connect to ThingsBoard MQTT broker
client.connect(THINGSBOARD_HOST, 1883, 60)

# Function to read the data from Arduino with retries
def read_arduino_data(retries=3, delay=0.1):
    for _ in range(retries):
        if arduino.in_waiting > 0:
            try:
                return arduino.readline().strip().decode("utf-8")
            except UnicodeDecodeError:
                print("UnicodeDecodeError: Failed to decode data from Arduino")
        time.sleep(delay)
    return None

# Function to send telemetry data to ThingsBoard
def send_telemetry_data(payload):
    # Convert telemetry data to JSON string
    payload_str = json.dumps(payload)
    print(payload_str)

    # Publish telemetry data
    client.publish(telemetry_topic, payload_str)

# Loop to read and send telemetry data
while True:
    sensor_reading_list = None
    expected_length = 7  # Adjust this value if needed

    while sensor_reading_list is None or len(sensor_reading_list) != expected_length:
        sensor_reading = read_arduino_data()
        if sensor_reading is None:
            print("Failed to read data from Arduino")
            continue
        sensor_reading_list = sensor_reading.split(" ")

    # Assign the list values into variables
    try:
        antitheft_status = sensor_reading_list[0]
        authorized_status = sensor_reading_list[1]
        gate_status = sensor_reading_list[2]
        ac_status = sensor_reading_list[3]
        light_status = sensor_reading_list[4]
        temp_value = sensor_reading_list[5]
        humid_value = sensor_reading_list[6]
    except IndexError:
        print("IndexError: Failed to parse sensor reading list")
        continue

    # Construct the payload as a dictionary
    payload = {
        "antitheft": antitheft_status,
        "authorized": authorized_status,
        "gate": gate_status,
        "ac": ac_status,
        "light": light_status,
        "temperature": temp_value,
        "humidity": humid_value
    }

    # Send telemetry data to ThingsBoard
    send_telemetry_data(payload)

    # Delay before sending the next telemetry data
    time.sleep(1)  # Adjust the delay as needed

    # Process any incoming MQTT messages
    client.loop()

