#ifndef CONFIG_H
#define CONFIG_H

/** IR Signal **/
const uint16_t MAX_IR_SIGNAL_LEN = 500;  // maximum of IR signal length
const uint8_t MAX_IR_SIGNAL_COUNT = 5;   // total of IR signal saved
const uint16_t ADD_IR_SIGNAL_TIMEOUT = 7000;

// Define pin numbers for RF receiver and transmitter
#define RF_RECEIVER_PIN 33
#define RF_TRANSMITTER_PIN 25
#define MAX_RF_SIGNALS 5
#define RF_SIGNAL_EEPROM_START_ADDR 3500
#define RF_SIGNAL_SIZE 8 // Size of each RF signal data in bytes

#define IR_LED_PIN 4                              // GPIO 4, where IR LED attached
#define IR_RECEIVER_PIN 15                        // GPIO 15,  where IR Recevier attached
const uint16_t IR_RECV_BUFFER_SIZE = 1024;        // multiply by 2-byte (size of uint16_t)
const uint16_t IR_RECV_TIMEOUT = 20;              // in ms
const uint8_t IR_RECV_TOLERANCE_PERCENTAGE = 25;  // tolerance percentage in %

#define CARRIER_SIGNAL_FREQ 38  // in kHz

/** Menu Button **/
#define MENU_SELECT_PIN 18  // GPIO 18
#define MENU_NEXT_PIN 19    // GPIO 19
#define MENU_PREV_PIN 5     // GPIO 5

/** Display **/
#define DISPLAY_FONT u8g2_font_8x13_tf  // font of LED display, refer to https://github.com/olikraus/u8g2/wiki/fntlistall


/** EEPROM **/
#define EEPROM_SIZE 4096  // in byte
#define ADDR_RFID_TAG_UID 3000


/** MFRC522 Module **/
#define RST_PIN 26
#define SS_PIN 27
#define MOSI_PIN 13
#define MISO_PIN 12
#define SCK_PIN 14

const uint16_t READ_TAG_UID_TIMEOUT = 5000;
const uint16_t READ_TAG_INFO_TIMEOUT = 5000;
const uint16_t WRITE_TAG_UID_TIMEOUT = 5000;


#endif
