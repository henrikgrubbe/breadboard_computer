#include <Wire.h>
#include <EEPROM.h>

#define DELAY 25

#define BUS0_PIN 2  // D2
#define BUS1_PIN 3  // D3
#define BUS2_PIN 4  // D4
#define BUS3_PIN 5  // D5
#define BUS4_PIN 6  // D6
#define BUS5_PIN 7  // D7
#define BUS6_PIN 8  // D8
#define BUS7_PIN 9  // D9

#define MI_PIN 10   // D10 - Memory in
#define RI_PIN 11   // D11 - RAM in
#define CLK_PIN 12  // D12 - Processor clock
#define OE_PIN 13   // D13 - Active low -output enable- of control logic
#define HLT_PIN 15  // A1  - Halt signal

#define BTN_PIN 14  // A0  - Button input

// SDA = A4/18
// SCL = A5/19

#define SLAVE_ADDR 9

long lastButtonPress = 0;
uint8_t shouldProgram = 0;

void setup() {
  disablePins();
  pinMode(BTN_PIN, INPUT);
  digitalWrite(BTN_PIN, HIGH);

  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);


  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);

  Serial.begin(115200);
  Serial.setTimeout(30000);
}

void loop() {
  if(Serial.available()) {
    while (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      if (line.length() >= 13) {
        uint8_t addr = parseBinary(line.substring(0, 4));
        uint8_t data = parseBinary(line.substring(5, 13));
        EEPROM.write(addr, data);
        delay(DELAY);
        Serial.print(addr); Serial.print(':'); Serial.print(data); Serial.print('\n');
      }
    }
    programFromEEPROM();
  }

  if (!digitalRead(BTN_PIN)) {
    long now = millis();
    if (now - lastButtonPress > 1000) {
      lastButtonPress = now;
      programFromEEPROM();
    }
  }

  if(shouldProgram) {
    shouldProgram = 0;
    programFromEEPROM();
  }
}

void programFromEEPROM() {
  Serial.println("Programming Breadboard from EEPROM");
  for (int i = 0; i < 16; ++i) {
    writeRAM(i, EEPROM.read(i));
    delay(DELAY);
  }
}

uint8_t lastAddr = 0xFF;
uint8_t lastData = 0xFF;
void receiveEvent() {
  uint8_t addr  = Wire.available() ? Wire.read() : 0xFF;
  uint8_t data  = Wire.available() ? Wire.read() : 0xFF;
  while(Wire.available()) Wire.read();
  lastAddr = addr;
  lastData = data;


  switch ((addr & 0xF0) >> 4) {
    case 0x0: // normal instruction
      EEPROM.write(addr, data);
      Serial.print("Receive event: WRITE TO ADDRESS - "); Serial.print(addr); Serial.print(" : "); Serial.println(data, HEX);

      break;
    case 0x1:
      shouldProgram = 1;
      Serial.println("Receive event: PROGRAM FROM EEPROM - ");

      break;
    default:
      Serial.print("Receive event "); Serial.print(addr); Serial.print(" : "); Serial.println(data, HEX);
      break;
  }

}
 
void requestEvent() {
  uint8_t response[2];
  response[0] = lastAddr;
  response[1] = lastData;
  
  Wire.write(response,sizeof(response));
  
  Serial.println("Request event");
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
