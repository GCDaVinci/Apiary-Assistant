#!/usr/bin/env python
import smbus
import time
# Define the I2C address to listen for
address = 0x69

# Create an I2C bus object
bus = smbus.SMBus(1)  # Use 1 for Raspberry Pi Model B

def receive_integer():
    try:
        # Read a single byte from the I2C bus
        data = bus.read_byte(address)
        data = bus.read_i2c_block_data(address,0,32)
        received_message = "".join([chr(byte) for byte in data if byte !=0])
        print("Received Message:", received_message)

        # Print the received integer
        # print("Received integer:", data)

    except Exception as e:
        print("Error:", str(e))

# Main loop
while True:
    receive_integer()
    time.sleep(1)
