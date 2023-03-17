#include <Adafruit_RA8875.h>


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, (millis() & 0x400) ? HIGH : LOW);
}
