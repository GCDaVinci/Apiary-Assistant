/**
 * Example code for using a microchip mrf24j40 module to send and receive
 * packets using plain 802.15.4
 * Requirements: 3 pins for spi, 3 pins for reset, chip select and interrupt
 * notifications
 * This example file is considered to be in the public domain
 * Originally written by Karl Palsson, karlp@tweak.net.au, March 2011
 */
#include <SPI.h>
#include <mrf24j.h>
#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT20.h>

#define TMP1 GPIO_NUM_36
#define TMP2 GPIO_NUM_39
#define DHT_PIN GPIO_NUM_21
#define MIC GPIO_NUM_32

//const int pin_reset = 6;
//const int pin_reset = 21; //clock on esp32
#define pin_reset GPIO_NUM_15
//const int pin_cs = 10; // default CS pin on ATmega8/168/328
//const int pin_cs = 25; //xtal on esp32
#define pin_cs GPIO_NUM_4
//const int pin_interrupt = 2; // default interrupt pin on ATmega8/168/328
//const int pin_interrupt = 19;
#define pin_interrupt GPIO_NUM_13

int VREF = 3300;
Mrf24j mrf(pin_reset, pin_cs, pin_interrupt);

long last_time;
long tx_interval = 1000;

DHT20 DHT;
int count = 0;

void interrupt_routine() {
    mrf.interrupt_handler(); // mrf24 object interrupt routine
}

void handle_rx() {
    Serial.print("received a packet ");Serial.print(mrf.get_rxinfo()->frame_length, DEC);Serial.println(" bytes long");
    
    if(mrf.get_bufferPHY()){
      Serial.println("Packet data (PHY Payload):");
      for (int i = 0; i < mrf.get_rxinfo()->frame_length; i++) {
          Serial.print(mrf.get_rxbuf()[i]);
      }
    }
    
    Serial.println("\r\nASCII data (relevant data):");
    for (int i = 0; i < mrf.rx_datalength(); i++) {
        Serial.write(mrf.get_rxinfo()->rx_data[i]);
    }
    
    Serial.print("\r\nLQI/RSSI=");
    Serial.print(mrf.get_rxinfo()->lqi, DEC);
    Serial.print("/");
    Serial.println(mrf.get_rxinfo()->rssi, DEC);
}

void handle_tx() {
    if (mrf.get_txinfo()->tx_ok) {
        Serial.println("TX went ok, got ack");
    } else {
        Serial.print("TX failed after ");Serial.print(mrf.get_txinfo()->retries);Serial.println(" retries\n");
    }
}

void temperatureAnalog() {
    int adc1 = analogRead(TMP1);
    int adc2 = analogRead(TMP2);

    float mv1 = adc1 * (VREF / 4096);
    float mv2 = adc2 * (VREF / 4096);

    float tempc1 = mv1 / 10;
    float tempc2 = mv2 / 10;

    float tempf1 = tempc1 * 9 / 5 + 32;
    float tempf2 = tempc2 * 9 / 5 + 32;

    Serial.println("\nTemperatures: ");
    Serial.println(tempf1);
    Serial.println(tempf2);
}

void temperaturei2c() {
    int status = DHT.read();
    Serial.println("Type\tHumidity (%)\tTemp (°C)\tTime (µs)\tStatus");

    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT.getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.print("\t\t");
    switch (status)
    {
      case DHT20_OK:
        Serial.print("OK");
        break;
      case DHT20_ERROR_CHECKSUM:
        Serial.print("Checksum error");
        break;
      case DHT20_ERROR_CONNECT:
        Serial.print("Connect error");
        break;
      case DHT20_MISSING_BYTES:
        Serial.print("Missing bytes");
        break;
      case DHT20_ERROR_BYTES_ALL_ZERO:
        Serial.print("All bytes read zero");
        break;
      case DHT20_ERROR_READ_TIMEOUT:
        Serial.print("Read time out");
        break;
      case DHT20_ERROR_LASTREAD:
        Serial.print("Error read too fast");
        break;
      default:
        Serial.print("Unknown error");
        break;
    }
    Serial.print("\n");
}

void setup() {
  Serial.begin(115200);
  pinMode(pin_reset, OUTPUT);
  pinMode(TMP1, INPUT);
  pinMode(TMP2, INPUT);
  pinMode(MIC, INPUT);
  Wire.begin();
  DHT.begin();    //  ESP32 default pins 21 22
  delay(2000);
  SPI.begin(); 
  mrf.reset();
  mrf.init();
  
  mrf.set_pan(0xcafe);
  // This is _our_ address
  mrf.address16_write(0x4206); 

  // uncomment if you want to receive any packet on this channel
  mrf.set_promiscuous(true);
  
  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);
  
  // uncomment if you want to buffer all PHY Payload
  //mrf.set_bufferPHY(true);

  attachInterrupt(pin_interrupt, interrupt_routine, CHANGE); // interrupt 0 equivalent to pin 2(INT0) on ATmega8/168/328
  last_time = millis();
  interrupts();
}

void loop() {
    count++;
    temperatureAnalog();
    temperaturei2c();
    if (count == 5) {
        mrf.check_flags(&handle_rx, &handle_tx);
        unsigned long current_time = millis();
        if (current_time - last_time > tx_interval) {
            last_time = current_time;
            Serial.println("txxxing...");
            mrf.send16(0x4204, "You are 0x4204");
        }
        count = 0;
    }
    delay(1000);
}
