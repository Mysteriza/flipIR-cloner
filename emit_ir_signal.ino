void emit_ir_signal(uint8_t signal_num) {

  if (signal_num > MAX_IR_SIGNAL_COUNT) return;

  uint16_t addr_signal_len = MAX_IR_SIGNAL_LEN * (signal_num - 1);
  uint16_t signal_len = EEPROM.readUShort(addr_signal_len);

  if (signal_len == 0) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Signal Not Set");
    u8g2.sendBuffer();
    return;
  }

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Emiting...");
  u8g2.sendBuffer();

  uint16_t raw_data[signal_len];

  uint16_t addr_signal = addr_signal_len + 2;

  for (uint16_t i = 0; i < signal_len; i++) {
    raw_data[i] = EEPROM.readUShort(addr_signal + (i * 2));
  }

  // Tambahkan kode berikut untuk menampilkan sinyal infrared pada Serial Monitor
  Serial.print("Emitted Infrared Signal: ");
  for (uint16_t i = 0; i < signal_len; i++) {
    Serial.print(raw_data[i]);
    Serial.print(" ");
  }
  Serial.println();

  irsend.sendRaw(raw_data, signal_len, CARRIER_SIGNAL_FREQ);
}