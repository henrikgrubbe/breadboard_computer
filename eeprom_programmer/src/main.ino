#include <Wire.h>
#include "PCF8574.h"

#define MICROCODE 0
#define SEVEN_SEG 1

uint8_t addr8_PIN = 8;
uint8_t addr9_PIN = 9;
uint8_t addr10_PIN = 10;

uint8_t out_PIN = 2;
uint8_t write_PIN = 3;

PCF8574 addrExp(0x20);
PCF8574 dataExp(0x21);


uint8_t program = MICROCODE;


void setup() {
  pinMode(addr8_PIN, OUTPUT);
  pinMode(addr9_PIN, OUTPUT);
  pinMode(addr10_PIN, OUTPUT);

  pinMode(out_PIN, OUTPUT);
  pinMode(write_PIN, OUTPUT);

  digitalWrite(out_PIN, HIGH);
  digitalWrite(write_PIN, HIGH);

  dataExp.begin();
  addrExp.begin();

  Serial.begin(115200);

  if (program == MICROCODE) {
    Serial.println("Started writing microcode...");
    setupMicrocode();
    writeMicrocode();
    Serial.println("Finished writing microcode...");
  }

  if (program == SEVEN_SEG) {
    Serial.println("Started writing 7 segment...");
    writeSevenSeg();
    Serial.println("Finished writing 7 segment...");
  }
}

 
void loop() {}


uint8_t readEEPROM(uint16_t addr) {
  digitalWrite(out_PIN, HIGH);
  digitalWrite(write_PIN, HIGH);

  uint8_t addrLOW = addr & 0x00ff;
  uint16_t addr8 = addr & 0x0100;
  uint16_t addr9 = addr & 0x0200;
  uint16_t addr10 = addr & 0x0400;

  addrExp.write8(addrLOW);
  digitalWrite(addr8_PIN, addr8 ? HIGH : LOW);
  digitalWrite(addr9_PIN, addr9 ? HIGH : LOW);
  digitalWrite(addr10_PIN, addr10 ? HIGH : LOW);

  digitalWrite(out_PIN, LOW);
  delay(1);

  uint8_t data = dataExp.read8();
  delay(1);

  digitalWrite(out_PIN, HIGH);
  delay(1);

  return data;
}


void writeEEPROM(uint16_t addr, uint8_t data) {
  digitalWrite(out_PIN, HIGH);
  digitalWrite(write_PIN, HIGH);

  uint8_t addrLOW = addr & 0x00ff;
  uint16_t addr8 = addr & 0x0100;
  uint16_t addr9 = addr & 0x0200; 
  uint16_t addr10 = addr & 0x0400;

  addrExp.write8(addrLOW);
  digitalWrite(addr8_PIN, addr8 ? HIGH : LOW);
  digitalWrite(addr9_PIN, addr9 ? HIGH : LOW);
  digitalWrite(addr10_PIN, addr10 ? HIGH : LOW);

  dataExp.write8(data);
  delay(1);

  digitalWrite(write_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(write_PIN, HIGH);

  delay(10);

  return;
}


void printContents(uint16_t start, uint16_t length) {
  uint8_t stepSize = 16;
  for (uint16_t base = start; base < length; base += stepSize) {
    byte data[stepSize];
    for (uint8_t offset = 0; offset < stepSize; offset++) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}
