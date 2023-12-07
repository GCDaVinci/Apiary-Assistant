This code utilizes the Mrf24j40-arduino-library by Karl Palsson. More info here https://github.com/karlp/Mrf24j40-arduino-library .
This code meant for operating Microchip's Mrf24j40 trancievers with ESP32 microntrollers.
  - mrf24j40 library contains the necessary incude and header files to use the mrf24j40s with the Arduino framework
  - Hive_Node is set to continuosly transmit a message to the device address 0x6001 on the personal area netowrk 0xcafe
  - Reciver_node is set to recieve messages on the PAN 0xcafe and transmit said message via I2C as a slave device
  - RPi3B_I2C_Read is a raspberry pi python script for reading the transmitted message from the reciver node via I2C
