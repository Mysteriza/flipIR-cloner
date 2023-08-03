
### FlipIR Cloner (Cloner Device)

This is a device for cloning IR Signals and RFID 13.56 MHz, just like [Flipper Zero](https://flipperzero.one/) but with very very very minimum functionality :)

Thanks to [Ahmad Fathan](https://github.com/ahmadfathan) for helping me making this project.

Features:
1. IR cloner (capable of duplicating up to 5 IR signals)
2. Emit All (Bruteforce IR Signals, automatically emitting all saved IR signals)
3. Read Info from RFID Card (only read UID and Type of RFID)
4. RFID UID reader and writer (1 UID)
5. Sub-GHz (Capable duplicating up to 5 433 Mhz signal and emitting the signal. but I haven't tested it with 433 Mhz module, maybe it's not working yet)
6. Sub-GHz Jammer (NEXT UPDATE! Jamming 433 MHz signal)
7. WiFi Evil Twin (NEXT UPDATE! Hacking 2.4 GHz WiFi with phising page and cloned WiFi)

***Any read results will be stored in EEPROM, so it will have limited features and capacity. Unless you expand it with the feature of being able to read/write to a memory card or uploading to server/MQTT/cloud, feel free to do so.***

#### PART LIST

1. ESP32 Lolin32 Lite
2. RFID Reader RC522 13.56 MHz Mifare Module (for read/write UID)
3. RFID Card 13.56 MHz
4. LCD OLED 0.96 I2C Module Display (or bigger screen, but you might need a bigger battery)
5. Toggle Button (button for control menu)
6. SPDT Switch Button (to turn on/off)
7. PCB Matrix Double Side 5x7
8. Ribbon Cable 10 pins 50 cm
9. Infrared & Photodiode LED Pair 5mm / IR TX & RX
10. HX1838 Remote Control IR Module Infrared Module Remote HX 1838
11. Resistor 220 Ohm (5 pcs, but you only use 1 in this project. Just prepare for any contingencies)
12. LiPo Battery 400 mAh (or bigger)
13. TP4056 LiPo Charger + Battery Protection Lithium Micro USB Module 1A
14. Transistor NPN s9013

Optional, if you want to 3D print the case:
1. 3D Printer
2. Filament

#### MENU ROUTE
|  |  |  |  |
|-------------|-------------|--------------|------------|
| > Main Menu | > IR Signal | > Add IR     | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             |             |              | > Signal 4 |
|             |             |              | > Signal 5 |
|             |             |              | > Back     |
|             |             | > Emit IR    | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             |             |              | > Signal 4 |
|             |             |              | > Signal 5 |
|             |             |              | > Back     |
|             |             | > Emit All   |            |
|             |             | > Back       |            |
|             | > RFID      | > Read Info  |            |
|             |             | > Read Tag   |            |
|             |             | > Write Tag  |            |
|             |             | > Back       |            |



#### Dependency

1. Display, https://github.com/olikraus/u8g2
2. IR send/receive, https://github.com/crankyoldgit/IRremoteESP8266
3. MFRC522, https://github.com/miguelbalboa/rfid

#### Showcase and Inside

<div style="text-align:center">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image1.jpeg" width="400">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image2.jpeg" width="400">
  <img src="https://github.com/Mysteriza/flipIR-cloner/blob/main/images/Inside.jpeg" width="400">
</div>
