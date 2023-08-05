void read_tag_uid() {

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Reading TAG...");
  u8g2.sendBuffer();

  String uid_str = "";
  uint64_t now = millis();
  bool found = false;
  while (true) {
    if (millis() - now > READ_TAG_UID_TIMEOUT) break;
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      continue;
    }

    // RFID Tag found
    uint16_t addr_rfid_tag_uid_len = ADDR_RFID_TAG_UID;
    EEPROM.writeByte(addr_rfid_tag_uid_len, mfrc522.uid.size);

    uint16_t addr_rfid_tag_uid = addr_rfid_tag_uid_len + 1;

    // Dump UID
    for (uint8_t i = 0; i < mfrc522.uid.size; i++, addr_rfid_tag_uid++) {

      EEPROM.writeByte(addr_rfid_tag_uid, mfrc522.uid.uidByte[i]);

      uid_str += String(mfrc522.uid.uidByte[i], 16) + "";
    }

    EEPROM.commit();

    Serial.print("Received UID: ");
    Serial.println(uid_str);

    found = true;
    break;
  }

  uint8_t selection = u8g2.userInterfaceMessage(
    found ? "Saved!" : "Timeout!",
    found ? &uid_str[0] : "",
    "",
    found ? " OK " : " OK \n Retry ");

  if (selection == 2) {  // retry
    read_tag_uid();
  }
}