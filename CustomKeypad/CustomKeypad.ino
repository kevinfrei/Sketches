#include "Adafruit_TinyUSB.h"

const byte ROWS = 6;
const byte COLS = 6;

byte colPins[COLS] = {12, 6, 5, A4, SCK, MOSI}; //connect to the column pinouts of the keypad
byte rowPins[ROWS] = {A1, A0, A2, 10, 11, 13}; //connect to the row pinouts of the keypad

void setup(){
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  for (byte r : rowPins) {
    pinMode(r, INPUT_PULLUP);
  }
  for (byte c : colPins) {
    pinMode(c, OUTPUT);
    digitalWrite(c, HIGH);
  }
  digitalWrite(LED_RED, HIGH);
  Serial.println("Setup!");
}
  
void loop(){
  digitalWrite(LED_BLUE, (millis() & 0x400) ? LOW : HIGH);
  for (uint8_t c = 0 ; c < COLS; c++) {
    digitalWrite(colPins[c], LOW);
    delay(1);
    for (uint8_t r = 0; r < ROWS; r++) {
      if (digitalRead(rowPins[r]) == LOW) {
        Serial.print("Pressed row ");
        Serial.print(r);
        Serial.print(" col ");
        Serial.println(c);
      }
    }
    digitalWrite(colPins[c], HIGH);
    delay(1);
  }
}
