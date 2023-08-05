void write_tag_uid() {
  uint16_t addr_rfid_tag_uid_len = ADDR_RFID_TAG_UID;
  uint8_t uid_len = EEPROM.readByte(addr_rfid_tag_uid_len);

  uint16_t addr_rfid_tag_uid = addr_rfid_tag_uid_len + 1;

  String uid_str = "";

  uint8_t uid[uid_len];

  for (uint8_t i = 0; i < uid_len; i++, addr_rfid_tag_uid++) {
    uid[i] = EEPROM.readByte(addr_rfid_tag_uid);

    uid_str += String(uid[i], 16) + " ";
  }

  uid_str.toUpperCase();

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Writing TAG...");
  u8g2.drawStr(0, 50, &uid_str[0]);
  u8g2.sendBuffer();

  Serial.print("UID to be written: ");
  Serial.println(uid_str);

  uint64_t now = millis();
  bool success = false;
  const char* err_title = "Timeout!";
  const char* err_msg = "";
  while (true) {
    if (millis() - now > WRITE_TAG_UID_TIMEOUT) break;
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      continue;
    }

    // Write tag UID
    if (!mfrc522.MIFARE_SetUid(uid, uid_len, true)) {
      err_title = "Failed!";
      err_msg = "Card Declined.";
      success = false;
      break;
    }

    success = true;
    break;
  }

  uint8_t selection = u8g2.userInterfaceMessage(success ? "Success!" : err_title, "", success ? "" : err_msg, success ? " OK " : " OK \n Retry ");

  if (selection == 2) {  // retry
    write_tag_uid();
  }
}