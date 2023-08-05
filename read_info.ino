void read_info() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Reading Info...");
  u8g2.sendBuffer();

  uint64_t now = millis();
  bool found = false;
  String rfid_info = "";  // String to hold RFID info to display

  while (true) {
    if (millis() - now > READ_TAG_INFO_TIMEOUT) break;
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      continue;
    }

    // RFID Tag found
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfid_info += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : "");
      rfid_info += String(mfrc522.uid.uidByte[i], HEX);
    }
    rfid_info += "\n" + String(mfrc522.PICC_GetTypeName(piccType));

    mfrc522.PICC_HaltA();       // halt PICC
    mfrc522.PCD_StopCrypto1();  // stop encryption on PCD

    found = true;  // set found to true
    break;
  }

  // Display the RFID info on the OLED
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, rfid_info.c_str());
  u8g2.sendBuffer();

  // Print the RFID info to the Serial Monitor
  Serial.println(rfid_info);

  uint8_t selection = u8g2.userInterfaceMessage(
    found ? rfid_info.c_str() : "Timeout!",
    "",
    "",
    " OK ");
}