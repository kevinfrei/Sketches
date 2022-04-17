// #include "Adafruit_TinyUSB.h"

const byte ROWS = 6;
const byte COLS = 6;

/* 
 * Left pins for Adafruit Feather nRF52840 Express
 *
 * byte colPins[COLS] = {12, 6, 5, A4, SCK, MOSI}; //connect to the column pinouts of the keypad
 * byte rowPins[ROWS] = {A1, A0, A2, 10, 11, 13}; //connect to the row pinouts of the keypad
 */
// Left (slightly rearranged) pins for Adafruit Feather Huzzah ESP32 
byte colPins[COLS] = {12, 32, 14, 21/*36*/, 5, 18}; 
byte rowPins[ROWS] = {25, 26, 22/*34*/, 33, 27, 13};
/* 
 * Right pins for Adafruit Feather nRF52840 Express
 * byte colPins[COLS] = {13, 5, 6, MOSI, SCK, A0}; //connect to the column pinouts of the keypad
 * byte rowPins[ROWS] = {A2, 12, A1, A5, A4, A3}; //connect to the row pinouts of the keypad
 */
// Right pins for Adafruit Feather Huzzah ESP32 
// byte colPins[COLS] = {13, 14, 32, 18, 5, 26}; 
// byte rowPins[ROWS] = {34, 12, 25, 4, 36, 39};

uint32_t last_change[COLS * ROWS] = {0};
bool pressed[COLS * ROWS] = {0};
const uint32_t debounce_time = 15;
const uint16_t write_delay_usec = 1350;
const uint16_t read_delay_usec = 250;

uint8_t write_transform(uint8_t val) {
  return val * 3 + val % 3 + 1;
}

void setup(){
  // Serial.begin(9600);
  Serial1.begin(1 << 20);
  // pinMode(LED_RED, OUTPUT);
  // pinMode(LED_BLUE, OUTPUT);
  // digitalWrite(LED_RED, HIGH);
  // analogWrite(LED_BLUE, 0);
  for (byte r : rowPins) {
    pinMode(r, INPUT_PULLUP);
  }
  for (byte c : colPins) {
    pinMode(c, OUTPUT);
    digitalWrite(c, HIGH);
  }
  // digitalWrite(LED_RED, LOW);
}
  
void loop(){
  uint32_t now = millis();
  // lk;janalogWrite(LED_BLUE, (now & 0x400) ? 0 : 0x18);
  for (uint8_t c = 0 ; c < COLS; c++) {
    digitalWrite(colPins[c], LOW);
    delayMicroseconds(write_delay_usec);
    for (uint8_t r = 0; r < ROWS; r++) {
      delayMicroseconds(read_delay_usec);
      bool p = digitalRead(rowPins[r]) == LOW;
      if (p != pressed[r * COLS + c] && last_change[r * COLS + c] < now + debounce_time) {
        uint8_t val = r * COLS + c + (p ? 0 : ROWS * COLS);
        Serial1.write(write_transform(val));
        // digitalWrite(LED_RED, p ? HIGH : LOW);
        pressed[r * COLS + c] = p;
        last_change[r * COLS + c] = now;
      }
    }
    digitalWrite(colPins[c], HIGH);
    delayMicroseconds(write_delay_usec);
  }
}
