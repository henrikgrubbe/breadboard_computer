#ifndef __IPROGRAMMER__
#define __IPROGRAMMER__

class IProgrammer {
  public:
    virtual uint8_t readEEPROM(uint16_t addr) = 0;
    virtual void writeEEPROM(uint16_t addr, uint8_t data) = 0;
};

#endif