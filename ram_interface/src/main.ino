#include <Wire.h>
#include <EEPROM.h>

#define DELAY 25

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
#define HLT_PIN 15

#define BTN_PIN 14

long lastButtonPress = 0;

void setup() {
  disablePins();
  pinMode(BTN_PIN, INPUT);
  digitalWrite(BTN_PIN, HIGH);

  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

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
      EEPROM.write(addr, data);
      delay(DELAY);
      Serial.print(addr); Serial.print(':'); Serial.print(data); Serial.print('\n');
    }
  }

  if (!digitalRead(BTN_PIN)) {
    long now = millis();
    if (now - lastButtonPress > 1000) {
      lastButtonPress = now;
      for (int i = 0; i < 16; ++i) {
        writeRAM(i, EEPROM.read(i));
      }
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
  pinMode(OE_PIN, OUTPUT);  
  pinMode(HLT_PIN, OUTPUT);
  digitalWrite(OE_PIN, HIGH);
  digitalWrite(HLT_PIN, HIGH);
  delay(DELAY);

  for (uint8_t pin = BUS0_PIN; pin <= RI_PIN; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  } 
}

void disablePins() {
  for (uint8_t pin = BUS0_PIN; pin <= RI_PIN; pin++) {
    pinMode(pin, INPUT);
    digitalWrite(pin, LOW);
  }

  pinMode(HLT_PIN, INPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(HLT_PIN, LOW);
  digitalWrite(OE_PIN, LOW);
  delay(DELAY);
}

void pulse(uint8_t pin) {
  digitalWrite(pin, HIGH);
  delay(DELAY);
  if (pin != CLK_PIN) {
    pulse(CLK_PIN);
  }
  digitalWrite(pin, LOW);
}

void writeRAM(uint8_t addr, uint8_t data) {
  enablePins();
  delay(DELAY);

  // Write address to bus
  for (uint8_t busPin = BUS0_PIN; busPin <= BUS3_PIN; busPin++) {
    uint8_t mask = 0b00000001 << (busPin - BUS0_PIN);
    digitalWrite(busPin, addr & mask ? HIGH : LOW);
  }

  // Write bus to address register
  pulse(MI_PIN);

  // Write data to bus
  for (uint8_t busPin = BUS0_PIN; busPin <= BUS7_PIN; busPin++) {
    uint8_t mask = 0b00000001 << (busPin - BUS0_PIN);
    digitalWrite(busPin, data & mask ? HIGH : LOW);
  }

  // Write bus to RAM
  pulse(RI_PIN);

  disablePins();
  delay(DELAY);
}
