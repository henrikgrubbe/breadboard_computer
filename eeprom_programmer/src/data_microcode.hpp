#ifndef __DATA_UCODE__
#define __DATA_UCODE__

#include "idata.hpp"

#define HLT 0b1000000000000000  // Halt clock
#define MI  0b0100000000000000  // Memory address register in
#define RI  0b0010000000000000  // RAM data in
#define RO  0b0001000000000000  // RAM data out
#define IO  0b0000100000000000  // Instruction register out
#define II  0b0000010000000000  // Instruction register in
#define AI  0b0000001000000000  // A register in
#define AO  0b0000000100000000  // A register out
#define EO  0b0000000010000000  // ALU out
#define SU  0b0000000001000000  // ALU subtract
#define BI  0b0000000000100000  // B register in
#define OI  0b0000000000010000  // Output register in
#define CE  0b0000000000001000  // Program counter enable
#define CO  0b0000000000000100  // Program counter out
#define J   0b0000000000000010  // Jump (program counter in)
#define FI  0b0000000000000001  // Flags in

#define RS  0b0000001100000000  // Reset

#define FLAGS_Z0C0 0
#define FLAGS_Z0C1 1
#define FLAGS_Z1C0 2
#define FLAGS_Z1C1 3

#define JC  0b0111
#define JZ  0b1000

class Data_MicroCode: public IData {
  public:
    Data_MicroCode() {
      // ZF = 0, CF = 0
      memcpy(ucode[FLAGS_Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));

      // ZF = 0, CF = 1
      memcpy(ucode[FLAGS_Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
      ucode[FLAGS_Z0C1][JC][2] = IO|J;

      // ZF = 1, CF = 0
      memcpy(ucode[FLAGS_Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
      ucode[FLAGS_Z1C0][JZ][2] = IO|J;

      // ZF = 1, CF = 1
      memcpy(ucode[FLAGS_Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
      ucode[FLAGS_Z1C1][JC][2] = IO|J;
      ucode[FLAGS_Z1C1][JZ][2] = IO|J;
    }

    ~Data_MicroCode() {

    }

    uint8_t getData(int address) {
      int flags       = (address & 0b1100000000) >> 8;
      int byte_sel    = (address & 0b0010000000) >> 7;
      int instruction = (address & 0b0001111000) >> 3;
      int step        = (address & 0b0000000111);

      if (byte_sel) {
        return ucode[flags][instruction][step];
      } else {
        return ucode[flags][instruction][step] >> 8;
      }
    }
  
  private:
    uint16_t UCODE_TEMPLATE[16][8] = {
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 0000 - NOP
      { MI|CO,  RO|II|CE,  IO|MI, RO|AI,  RS,          0,  0, 0 },   // 0001 - LDA
      { MI|CO,  RO|II|CE,  IO|MI, RO|BI,  EO|AI|FI,    RS, 0, 0 },   // 0010 - ADD
      { MI|CO,  RO|II|CE,  IO|MI, RO|BI,  EO|AI|SU|FI, RS, 0, 0 },   // 0011 - SUB
      { MI|CO,  RO|II|CE,  IO|MI, AO|RI,  RS,          0,  0, 0 },   // 0100 - STA
      { MI|CO,  RO|II|CE,  IO|AI, RS,     0,           0,  0, 0 },   // 0101 - LDI
      { MI|CO,  RO|II|CE,  IO|J,  RS,     0,           0,  0, 0 },   // 0110 - JMP
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 0111 - JC
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1000 - JZ
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1001
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1010
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1011
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1100
      { MI|CO,  RO|II|CE,  RS,    0,      0,           0,  0, 0 },   // 1101
      { MI|CO,  RO|II|CE,  AO|OI, RS,     0,           0,  0, 0 },   // 1110 - OUT
      { MI|CO,  RO|II|CE,  HLT,   0,      0,           0,  0, 0 },   // 1111 - HLT
    };

    uint16_t ucode[4][16][8];
};





#endif