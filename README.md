This code utilizes the Mrf24j40-arduino-library by Karl Palsson. It's meant for operating Microchip's Mrf24j40 trancievers with ESP32 microntrollers.
  - The ESP32 has a buad rate of 115200
  - Hive_Node is set to continuosly transmit a message to the device address 0x6001 on the personal area netowrk 0xcafe
  - Reciver_node is set to recieve messages on the PAN 0xcafe and transmit said message via I2C as a slave device
