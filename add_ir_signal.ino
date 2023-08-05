void add_ir_signal(uint8_t signal_num) {

  if (signal_num > MAX_IR_SIGNAL_COUNT) return;  // abort if signal num is exceeded

  uint16_t addr_signal_len = MAX_IR_SIGNAL_LEN * (signal_num - 1);
  uint16_t addr_signal = addr_signal_len + 2;

  irrecv.enableIRIn();  // enable IR receiver

  uint64_t now = millis();
  while (true) {
    if (irrecv.decode(&results)) {

      uint16_t rawdata[1024];
      uint16_t j = 0;
      for (uint16_t i = 1; i < results.rawlen; i++, j++) {
        uint32_t usecs = results.rawbuf[i] * kRawTick;
        rawdata[j] = usecs;
      }

      EEPROM.writeUShort(addr_signal_len, j);

      for (uint16_t i = 0; i < j; i++) {
        EEPROM.writeUShort(addr_signal + (i * 2), rawdata[i]);
      }

      EEPROM.commit();

      Serial.print("Received IR Signal: ");
      for (uint16_t i = 0; i < j; i++) {
        Serial.print(rawdata[i]);
        Serial.print(" ");
      }
      Serial.println();

      break;
    }
    if (millis() - now > ADD_IR_SIGNAL_TIMEOUT) break;
  }

  irrecv.disableIRIn();  // disable IR receiver
}