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
  Route{ 3, "RF Jammer" },
  Route{ 4, "Back" },
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
                       "RF Jammer\n"
                       "Back",
         &subghz_route[0], 4 },
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

  else if (next_route->title == "RF Jammer") {
    start_jamming();
    // current_route = main_menu_route[3];  // Atau indeks yang sesuai dengan menu Sub-GHz
    // return;  // Menghentikan eksekusi lebih lanjut dan kembali ke menu yang diinginkan
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
