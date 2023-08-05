// Fungsi untuk menambahkan sinyal RF
void addRFSignal(int signalNum) {
  mySwitch.enableReceive(33);

  unsigned long rfSignal;
  uint16_t address = RF_SIGNAL_EEPROM_START_ADDR + (signalNum - 1) * RF_SIGNAL_SIZE;

  // Clear the previous RF signal data at the specified address
  for (int i = 0; i < RF_SIGNAL_SIZE; i++) {
    EEPROM.write(address + i, 0);
  }

  // Capture RF signal within 7 seconds
  unsigned long startTime = millis();
  while (millis() - startTime < 7000) {
    if (mySwitch.available()) {
      rfSignal = mySwitch.getReceivedValue();
      // Save RF signal data to EEPROM
      EEPROM.put(address, rfSignal);
      mySwitch.resetAvailable();
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "RF Captured!");
      u8g2.sendBuffer();

      // Print the captured RF Signal to serial monitor
      Serial.print("Captured RF Signal (Decimal): ");
      Serial.println(rfSignal);
      Serial.print("Captured RF Signal (Hexadecimal): 0x");
      Serial.println(rfSignal, HEX);

      delay(700);
      return;
    }
  }

  // If no RF signal captured within 7 seconds, display a message and return to the main menu
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "No RF Captured!");
  u8g2.sendBuffer();
  delay(700);
  current_route = subghz_route[1];

  mySwitch.disableReceive();  // Nonaktifkan modul RF sebagai penerima setelah selesai mengirim sinyal
}