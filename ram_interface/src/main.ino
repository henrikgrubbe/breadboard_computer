#include <Wire.h>

#define DELAY 5

#define BUS0_PIN 2
#define BUS1_PIN 3
#define BUS2_PIN 4
#define BUS3_PIN 5
#define BUS4_PIN 6
#define BUS5_PIN 7
#define BUS6_PIN 8
#define BUS7_PIN 9
#define MI_PIN 10
#define RI_PIN 11
#define CLK_PIN 12
#define OE_PIN 13

void setup() {
  disablePins();

  Serial.begin(115200);
  Serial.setTimeout(30000);
}

void loop() {
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');

    if (line.length() >= 13) {
      uint8_t addr = parseBinary(line.substring(0, 4));
      uint8_t data = parseBinary(line.substring(5, 13));
      writeRAM(addr, data);
    }
  }
}

uint8_t parseBinary(String str) {
  uint8_t result = 0;
  uint8_t len = str.length();

  for (int i = 0; i < len; i++) {
    result *= 2;
    result += (str.charAt(i) == '1' ? 1 : 0);
  }

  return result;
}

void enablePins() {
  for (uint8_t pin = BUS0_PIN; pin <= RI_PIN; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  pinMode(CLK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(OE_PIN, HIGH);
}

void disablePins() {
  for (uint8_t pin = BUS0_PIN; pin <= RI_PIN; pin++) {
    pinMode(pin, INPUT);
    digitalWrite(pin, LOW);
  }

  pinMode(CLK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  digitalWrite(OE_PIN, LOW); 
}

void pulseClk() {
  digitalWrite(CLK_PIN, HIGH);
  delay(DELAY);
  digitalWrite(CLK_PIN, LOW);
}

void pulseMI() {
  digitalWrite(MI_PIN, HIGH);
  delay(DELAY);
  pulseClk();
  digitalWrite(MI_PIN, LOW);
}

void pulseRI() {
  digitalWrite(RI_PIN, HIGH);
  delay(DELAY);
  pulseClk();
  digitalWrite(RI_PIN, LOW);
}

void writeRAM(uint8_t addr, uint8_t data) {
  enablePins(); 

  // Write address to bus
  for (uint8_t busPin = BUS0_PIN; busPin <= BUS3_PIN; busPin++) {
    uint8_t mask = 0b00000001 << (busPin - BUS0_PIN);
    digitalWrite(busPin, addr & mask ? HIGH : LOW);
  }

  // Write bus to address register
  pulseMI();

  // Write data to bus
  for (uint8_t busPin = BUS0_PIN; busPin <= BUS7_PIN; busPin++) {
    uint8_t mask = 0b00000001 << (busPin - BUS0_PIN);
    digitalWrite(busPin, data & mask ? HIGH : LOW);
  }

  // Write bus to RAM
  pulseRI();

  disablePins();
  delay(DELAY);
}
