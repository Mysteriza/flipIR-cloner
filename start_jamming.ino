void start_jamming() {
  pinMode(MENU_SELECT_PIN, INPUT_PULLUP);  // Konfigurasikan pin sebagai input dengan pull-up resistor
  mySwitch.enableTransmit(25);
  unsigned long lastSendTime = 0;
  const unsigned long sendInterval = 10;

  // Loop untuk melakukan jamming terus menerus
  while (true) {
    // Tampilkan teks "Jamming..." pada layar
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Jamming...");
    u8g2.sendBuffer();

    // Cek apakah pin MENU_SELECT_PIN ditekan (rendah karena pull-up)
    if (digitalRead(MENU_SELECT_PIN) == LOW) {
      delay(50);
      if (digitalRead(MENU_SELECT_PIN) == LOW) {
        mySwitch.disableTransmit();
        current_route = main_menu_route[3];  // Set current route to main menu
        break;                             // Keluar dari loop while
      }
    }

    // Cek apakah sudah waktunya mengirim kode acak
    if (millis() - lastSendTime >= sendInterval) {
      long randomCode = random(1, 1000000);
      mySwitch.send(randomCode, 24);

      // Cetak kode acak ke Serial Monitor
      Serial.print("Kode Acak Terkirim: ");
      Serial.println(randomCode);

      lastSendTime = millis();
    }
  }
}