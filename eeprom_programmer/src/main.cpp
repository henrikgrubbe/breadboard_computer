#include <Arduino.h>

#include "iprogrammer.hpp"
#include "programmer_PCF8574.hpp"

#include "idata.hpp"
#include "data_microcode.hpp"
#include "data_sevenseg.hpp"

IData* data;
IProgrammer* programmer;

void printContents(uint16_t start, uint16_t length);

void setup() {
  Serial.begin(115200);

  // select programmer
  programmer = new Programmer_PCF8574();

  // select data
  data = new Data_MicroCode();
  // data = new Data_SevenSeg();


  Serial.println("Started writing to EEPROM...");
  for (int address = 0; address < 2048; address++) {
    programmer->writeEEPROM(address, data->getData(address));
    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println("Finished writing EEPROM...");
}

void loop() {}

void printContents(uint16_t start, uint16_t length) {
  uint8_t stepSize = 16;
  for (uint16_t base = start; base < length; base += stepSize) {
    byte data[stepSize];
    for (uint8_t offset = 0; offset < stepSize; offset++) {
      data[offset] = programmer->readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}