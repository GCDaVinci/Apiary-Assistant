#include <SPI.h>
#include <mrf24j.h>
#include <Arduino.h>


//const int pin_reset = 6;
//const int pin_reset = 21; //clock on esp32
#define pin_reset GPIO_NUM_15
//const int pin_cs = 10; // default CS pin on ATmega8/168/328
//const int pin_cs = 25; //xtal on esp32
#define pin_cs GPIO_NUM_4
//const int pin_interrupt = 2; // default interrupt pin on ATmega8/168/328
//const int pin_interrupt = 19;
#define pin_interrupt GPIO_NUM_13

Mrf24j mrf(pin_reset, pin_cs, pin_interrupt);

long last_time;
long tx_interval = 5000;

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


void setup() {
  Serial.begin(115200);
  pinMode(pin_reset, OUTPUT);
  delay(2000);
  SPI.begin(); 
  mrf.reset();
  mrf.init();
  
  mrf.set_pan(0xcafe);
  // This is _our_ address
  mrf.address16_write(0x4202); 

  // uncomment if you want to receive any packet on this channel
  //mrf.set_promiscuous(true);
  
  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);
  
  // uncomment if you want to buffer all PHY Payload
  //mrf.set_bufferPHY(true);

  attachInterrupt(pin_interrupt, interrupt_routine, CHANGE); // interrupt 0 equivalent to pin 2(INT0) on ATmega8/168/328
  last_time = millis();
  interrupts();
}

void loop() {
        mrf.check_flags(&handle_rx, &handle_tx);
        unsigned long current_time = millis();
        int test_var = 1234;
        char b = test_var;
        if (current_time - last_time > tx_interval) {
            last_time = current_time;
            Serial.println(" ");
            Serial.println("txxxing...");
            mrf.send16(0x6001, "Transmission from Hive Node");
        }
}
