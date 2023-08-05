// Fungsi untuk mengirim sinyal RF
void emitRFSignal(int signalNum) {
  mySwitch.enableTransmit(25);

  uint16_t address = RF_SIGNAL_EEPROM_START_ADDR + (signalNum - 1) * RF_SIGNAL_SIZE;
  unsigned long rfSignal;

  // Read the RF signal data from EEPROM
  EEPROM.get(address, rfSignal);

  // Check if the RF signal data is valid
  if (rfSignal != 0) {
    // Send the RF signal
    mySwitch.send(rfSignal, 24);  // Send 24-bit RF signal (adjust the number of bits if needed)
    delay(10);
    //mySwitch.send(rfSignal, 24);  // Send it twice for better reliability
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "RF Signal Sent!");
    u8g2.sendBuffer();
    delay(500);
  } else {
    // If RF signal data is not available, display a message
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "No RF Saved!");
    u8g2.sendBuffer();
    delay(1000);

    // Return to "Emit RF Signal" menu
    current_route = subghz_route[2];  // Assuming the "Emit RF Signal" option is the second option in the "IR Signal" menu
  }
  mySwitch.disableTransmit();
}