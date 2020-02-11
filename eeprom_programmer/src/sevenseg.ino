void writeSevenSeg() {
  uint8_t digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };

  Serial.println("Programming ones place");
  for (int value = 0; value < 256; value++) {
    writeEEPROM(value, digits[value % 10]);
    if (value % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");

  Serial.println("Programming tens place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
    if (value % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  
  Serial.println("Programming hundreds place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 512, digits[(value / 100) % 10]);
    if (value % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  
  Serial.println("Programming sign");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 768, 0);
    if (value % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  


  Serial.println("Programming ones place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1024, digits[abs(value) % 10]);
    if ((value + 128) % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  
  Serial.println("Programming tens place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1280, digits[abs(value / 10) % 10]);
    if ((value + 128) % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  
  Serial.println("Programming hundreds place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1536, digits[abs(value / 100) % 10]);
    if ((value + 128) % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
  
  Serial.println("Programming sign (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    if (value < 0) {
      writeEEPROM((byte)value + 1792, 0x01);
    } else {
      writeEEPROM((byte)value + 1792, 0);
    }
    if ((value + 128) % 16 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done!");
}
