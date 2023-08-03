// Native Library
#include <Arduino.h>
#include "EEPROM.h"
#include <Wire.h>

#include <WiFi.h>

// https://github.com/olikraus/u8g2
#include <U8g2lib.h>

// https://github.com/crankyoldgit/IRremoteESP8266
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

#include <SPI.h>
#include <MFRC522.h>

#include <RCSwitch.h>

#include "config.h"

IRrecv irrecv(IR_RECEIVER_PIN, IR_RECV_BUFFER_SIZE, IR_RECV_TIMEOUT, true);
IRsend irsend(IR_LED_PIN);  // Set the GPIO to be used to sending the message.

decode_results results;  // Somewhere to store the results

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // All Boards without Reset of the Display

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Inisialisasi RCSwitch
RCSwitch mySwitch = RCSwitch();

#define EMIT_ALL_DELAY 10  // Delay between each signal in "Emit All" in milliseconds

struct Route {
  uint8_t pos;
  const char* title;
  const char* list;
  Route* children;
  uint8_t num_of_children;
};

Route signal_route[] = {
  Route{ 1, "IR Signal 1" },
  Route{ 2, "IR Signal 2" },
  Route{ 3, "IR Signal 3" },
  Route{ 4, "IR Signal 4" },
  Route{ 5, "IR Signal 5" },
  Route{ 6, "Back" },
};

Route ir_signal_route[] = {
  Route{ 1, "Add IR Signal", "IR Signal 1\n"
                             "IR Signal 2\n"
                             "IR Signal 3\n"
                             "IR Signal 4\n"
                             "IR Signal 5\n"
                             "Back",
         &signal_route[0], 6 },
  Route{ 2, "Emit IR Signal", "IR Signal 1\n"
                              "IR Signal 2\n"
                              "IR Signal 3\n"
                              "IR Signal 4\n"
                              "IR Signal 5\n"
                              "Back",
         &signal_route[0], 6 },
  Route{ 3, "Emit All" },
  Route{ 4, "Back" }
};

Route rfid_route[] = {
  Route{ 1, "Read Info" },
  Route{ 2, "Read Tag" },
  Route{ 3, "Write Tag" },
  Route{ 4, "Back" },
};

Route add_rf_route[] = {
  Route{ 1, "RF Signal 1" },
  Route{ 2, "RF Signal 2" },
  Route{ 3, "RF Signal 3" },
  Route{ 4, "RF Signal 4" },
  Route{ 5, "RF Signal 5" },
  Route{ 6, "Back" },
};

Route emit_rf_route[] = {
  Route{ 1, "Emit RF 1" },
  Route{ 2, "Emit RF 2" },
  Route{ 3, "Emit RF 3" },
  Route{ 4, "Emit RF 4" },
  Route{ 5, "Emit RF 5" },
  Route{ 6, "Back" },
};

Route subghz_route[] = {
  Route{ 1, "Add RF Signal", "RF Signal 1\n"
                             "RF Signal 2\n"
                             "RF Signal 3\n"
                             "RF Signal 4\n"
                             "RF Signal 5\n"
                             "Back",
         &add_rf_route[0], 6 },
  Route{ 2, "Emit RF Signal", "Emit RF 1\n"
                              "Emit RF 2\n"
                              "Emit RF 3\n"
                              "Emit RF 4\n"
                              "Emit RF 5\n"
                              "Back",
         &emit_rf_route[0], 6 },
  Route{ 3, "Back" },
};

Route main_menu_route[] = {
  Route{ 1, "IR Signal", "Add IR Signal\n"
                         "Emit IR Signal\n"
                         "Emit All\n"
                         "Back",
         &ir_signal_route[0], 4 },
  Route{ 2, "RFID", "Read Info\n"
                    "Read Tag\n"
                    "Write Tag\n"
                    "Back",
         &rfid_route[0], 4 },
  Route{ 3, "Sub-GHz", "Add RF Signal\n"
                       "Emit RF Signal\n"
                       "Back",
         &subghz_route[0], 3 },
  Route{ 4, "Back" },
};

Route root = Route{ 1, "Main Menu", "IR Signal\n"
                                    "RFID\n"
                                    "Sub-GHz",
                    &main_menu_route[0], 3 };

Route current_route = root;
Route back_stack[10];
uint8_t route_pos = 0;
uint8_t last_selection = 0;

void setup(void) {

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();  // Init MFRC522 card

  irrecv.setTolerance(IR_RECV_TOLERANCE_PERCENTAGE);  // Override the default tolerance.
  irsend.begin();

  EEPROM.begin(EEPROM_SIZE);

  u8g2.setBusClock(5000000);

  u8g2.begin(MENU_SELECT_PIN, MENU_NEXT_PIN, MENU_PREV_PIN, /* menu_up_pin= */ U8X8_PIN_NONE, /* menu_down_pin= */ U8X8_PIN_NONE, /* menu_home_pin= */ U8X8_PIN_NONE);

  u8g2.setFont(DISPLAY_FONT);

  back_stack[route_pos] = current_route;
}

void loop(void) {
  uint8_t current_selection = u8g2.userInterfaceSelectionList(
    current_route.title,
    (last_selection == 0) ? current_route.children[0].pos : last_selection,
    current_route.list);

  Route* next_route = find_next_route_by_pos(current_selection);

  if (next_route == NULL) return;

  last_selection = current_selection;

  if (next_route->title == "Back") {
    route_pos--;
    current_route = back_stack[route_pos];
    last_selection = 0;
    return;
  } else if (next_route->title == "Read Info") {
    read_info();
    return;
  } else if (next_route->title == "Read Tag") {
    read_tag_uid();
    return;
  } else if (next_route->title == "Write Tag") {
    write_tag_uid();
    return;
  } else if (next_route->title == "IR Signal 1") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(1);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording...");
      u8g2.sendBuffer();

      add_ir_signal(1);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "IR Signal 2") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(2);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording...");
      u8g2.sendBuffer();

      add_ir_signal(2);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "IR Signal 3") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(3);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording...");
      u8g2.sendBuffer();

      add_ir_signal(3);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "IR Signal 4") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(4);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording...");
      u8g2.sendBuffer();

      add_ir_signal(4);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "IR Signal 5") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(5);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording...");
      u8g2.sendBuffer();

      add_ir_signal(5);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "RF Signal 1") {
    if (current_route.title == "Emit RF Signal") {
      emitRFSignal(1);
    } else if (current_route.title == "Add RF Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Capturing...");
      u8g2.sendBuffer();

      addRFSignal(1);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "RF Signal 2") {
    if (current_route.title == "Emit RF Signal") {
      emitRFSignal(2);
    } else if (current_route.title == "Add RF Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Capturing...");
      u8g2.sendBuffer();

      addRFSignal(2);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "RF Signal 3") {
    if (current_route.title == "Emit RF Signal") {
      emitRFSignal(3);
    } else if (current_route.title == "Add RF Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Capturing...");
      u8g2.sendBuffer();

      addRFSignal(3);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "RF Signal 4") {
    if (current_route.title == "Emit RF Signal") {
      emitRFSignal(4);
    } else if (current_route.title == "Add RF Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Capturing...");
      u8g2.sendBuffer();

      addRFSignal(4);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "RF Signal 5") {
    if (current_route.title == "Emit RF Signal") {
      emitRFSignal(5);
    } else if (current_route.title == "Add RF Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Capturing...");
      u8g2.sendBuffer();

      addRFSignal(5);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done!");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "Emit All") {
    for (uint8_t i = 1; i <= MAX_IR_SIGNAL_COUNT; i++) {
      // Display the message on the OLED display
      u8g2.clearBuffer();                   // clear the internal memory
      u8g2.drawStr(0, 10, "Emitting IR ");  // write something to the internal memory
      u8g2.setCursor(100, 10);
      u8g2.print(i);
      u8g2.sendBuffer();  // transfer internal memory to the display
      emit_ir_signal(i);
      delay(EMIT_ALL_DELAY);  // add delay between signals
    }
    // Return to the IR Signal menu after all signals have been emitted
    current_route = main_menu_route[0];
    return;
  }

  route_pos++;
  current_route = *next_route;
  back_stack[route_pos] = current_route;
  last_selection = 0;
}


Route* find_next_route_by_pos(uint8_t pos) {
  for (uint8_t i = 0; i < current_route.num_of_children; i++) {
    if (pos == current_route.children[i].pos) {
      return &current_route.children[i];
    }
  }
  return NULL;
}

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

  uint8_t selection = u8g2.userInterfaceMessage(
    success ? "Success!" : err_title,
    "",
    success ? "" : err_msg,
    success ? " OK " : " OK \n Retry ");

  if (selection == 2) {  // retry
    write_tag_uid();
  }
}

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
