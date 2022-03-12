#include "Adafruit_TinyUSB.h"

const byte ROWS = 6;
const byte COLS = 6;

// Left:
byte colPins[COLS] = {12, 6, 5, A4, SCK, MOSI}; //connect to the column pinouts of the keypad
byte rowPins[ROWS] = {A1, A0, A2, 10, 11, 13}; //connect to the row pinouts of the keypad
// Right:
// byte colPins[COLS] = {13, 5, 6, MOSI, SCK, A0}; //connect to the column pinouts of the keypad
// byte rowPins[ROWS] = {A2, 12, A1, A5, A4, A3}; //connect to the row pinouts of the keypad
uint32_t last_change[COLS * ROWS] = {0};
bool pressed[COLS * ROWS] = {0};
const uint32_t debounce_time = 15;

void setup(){
  Serial.begin(9600);
  Serial1.begin(1 << 20);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  analogWrite(LED_BLUE, 0);
  for (byte r : rowPins) {
    pinMode(r, INPUT_PULLUP);
  }
  for (byte c : colPins) {
    pinMode(c, OUTPUT);
    digitalWrite(c, HIGH);
  }
  digitalWrite(LED_RED, LOW);
}
  
void loop(){
  uint32_t now = millis();
  analogWrite(LED_BLUE, (now & 0x400) ? 0 : 0x18);
  for (uint8_t c = 0 ; c < COLS; c++) {
    digitalWrite(colPins[c], LOW);
    delay(1);
    for (uint8_t r = 0; r < ROWS; r++) {
      delay(1);
      bool p = digitalRead(rowPins[r]) == LOW;
      if (p != pressed[r * 6 + c] && last_change[r * 6 + c] < now + debounce_time) {
        uint8_t val = r * 6 + c + (p ? 0 : 36);
        Serial1.write((unsigned char)(val * 3 + val % 3 + 1));
        digitalWrite(LED_RED, p ? HIGH : LOW);
        pressed[r * 6 + c] = p;
        last_change[r * 6 + c] = now;
      }
    }
    digitalWrite(colPins[c], HIGH);
  }
}
