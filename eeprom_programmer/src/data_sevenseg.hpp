#ifndef __DATA_7SEG__
#define __DATA_7SEG__

#include "idata.hpp"

class Data_SevenSeg: public IData {
public:
  Data_SevenSeg();
  ~Data_SevenSeg();

  uint8_t getData(int address) {
    // value that should be displayed
    int value = address & 0xFF;

    // Ones place
    if(address <= 0x0FF) { return digits[(value / 1) % 10]; }
    // tens place
    if(address <= 0x1FF) { return digits[(value / 10) % 10]; }
    // houndreds place
    if(address <= 0x2FF) { return digits[(value / 100) % 10]; }
    // sign (unsigned)
    if(address <= 0x3FF) { return 0; }
   
    // signed value
    value -= 128;

    // Ones place
    if(address <= 0x4FF) { return digits[abs(value / 1) % 10]; }
    // tens place
    if(address <= 0x5FF) { return digits[abs(value / 10) % 10]; }
    // houndreds place
    if(address <= 0x6FF) { return digits[abs(value / 100) % 10]; }
    // sign (signed)
    if(address <= 0x7FF) { return value < 0 ? 0x01 : 0; }

    return 0;
  }

  private: 
    uint8_t digits[10] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };
};


#endif