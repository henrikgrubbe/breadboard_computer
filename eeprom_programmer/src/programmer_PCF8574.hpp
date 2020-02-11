#ifndef __PROG_PCF8574__
#define __PROG_PCF8574__

#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"
#include "iprogrammer.hpp"

class Programmer_PCF8574: public IProgrammer
{
  public:
    Programmer_PCF8574() {
      addrExp = new PCF8574(0x20);
      dataExp = new PCF8574(0x21);

      pinMode(addr8_PIN, OUTPUT);
      pinMode(addr9_PIN, OUTPUT);
      pinMode(addr10_PIN, OUTPUT);

      pinMode(out_PIN, OUTPUT);
      pinMode(write_PIN, OUTPUT);

      digitalWrite(out_PIN, HIGH);
      digitalWrite(write_PIN, HIGH);

      dataExp->begin();
      addrExp->begin();
    }
    
    ~Programmer_PCF8574();
    
    uint8_t readEEPROM(uint16_t addr) {
      digitalWrite(out_PIN, HIGH);
      digitalWrite(write_PIN, HIGH);

      uint8_t addrLOW = addr & 0x00ff;
      uint16_t addr8 = addr & 0x0100;
      uint16_t addr9 = addr & 0x0200;
      uint16_t addr10 = addr & 0x0400;

      addrExp->write8(addrLOW);
      digitalWrite(addr8_PIN, addr8 ? HIGH : LOW);
      digitalWrite(addr9_PIN, addr9 ? HIGH : LOW);
      digitalWrite(addr10_PIN, addr10 ? HIGH : LOW);

      digitalWrite(out_PIN, LOW);
      delay(1);

      uint8_t data = dataExp->read8();
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

      addrExp->write8(addrLOW);
      digitalWrite(addr8_PIN, addr8 ? HIGH : LOW);
      digitalWrite(addr9_PIN, addr9 ? HIGH : LOW);
      digitalWrite(addr10_PIN, addr10 ? HIGH : LOW);

      dataExp->write8(data);
      delay(1);

      digitalWrite(write_PIN, LOW);
      delayMicroseconds(1);
      digitalWrite(write_PIN, HIGH);

      delay(10);
    }


  private:
    uint8_t addr8_PIN = 8;
    uint8_t addr9_PIN = 9;
    uint8_t addr10_PIN = 10;

    uint8_t out_PIN = 2;
    uint8_t write_PIN = 3;

    PCF8574* addrExp;
    PCF8574* dataExp;
};

#endif