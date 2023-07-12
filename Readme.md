
### FLIPPER LITE (Cloner Device)

This project tries to duplicate [Flipper Zero](https://flipperzero.one/) with very very very minimum functionality :)

Thanks to [Ahmad Fathan](https://github.com/ahmadfathan) for making me this project.

It has:
1. IR cloner (capable of duplicating up to 3 IR signals)
2. RFID UID reader/writer (1 UID)

***Any read results will be stored in EEPROM, so it will have limited features and capacity. Unless you expand it with the feature of being able to read/write to a memory card, feel free to do so.***

#### PART LIST

1. ESP32 Lolin32 Lite
2. RFID Reader RC522 13.56 MHz Mifare Module (for read/write UID)
3. RFID Card 13.56 MHz
4. LCD OLED 0.96 I2C Module Display (or bigger screen, but you might need a bigger battery)
5. Toggle Button (to switch on/off the device)
6. Push Button (3 pcs)
7. PCB Matrix Double Side 5x7
8. Ribbon Cable 10 pins 50 cm
9. Infrared & Photodiode LED Pair 5mm / IR TX & RX
10. HX1838 Remote Control IR Module Infrared Module Remote HX 1838
11. Resistor 220 Ohm (5 pcs, but you only use 1 in this project. Just prepare for any contingencies)
12. LiPo Battery 400 mAh (or bigger)
13. TP4056 LiPo Charger + Battery Protection Lithium Micro USB Module 1A

Optional, if you want to 3D print the case:
1. 3D Printer
2. Filament

#### MENU ROUTE
|  |  |  |  |
|-------------|-------------|--------------|------------|
| > Main Menu | > IR Signal | > Add Signal | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             |             | > IR Signal  | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             | > RFID      | > Read Tag   |            |
|             |             | > Write Tag  |            |

#### EEPROM Address Mapping

##### 1. IR Signal
|Address From|Address To|Remark|
|-|-|-|
|0|999|IR Signal 1|
|1000|1999|IR Signal 2|
|2000|2999|IR Signal 3|

##### 2. RFID

|Address From|Address To|Remark|
|-|-|-|
|3000|30009|UID 1 (currently only 1 RFID Tag)|

#### Dependency

1. Display, https://github.com/olikraus/u8g2
2. IR send/receive, https://github.com/crankyoldgit/IRremoteESP8266
3. MFRC522, https://github.com/miguelbalboa/rfid

#### Showcase

<div style="text-align:center">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image1.jpeg" width="400">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image2.jpeg" width="400">
</div>
