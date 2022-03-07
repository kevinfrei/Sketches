// #include <SoftwareSerial.h>
#include <HardwareSerial.h>

#define LED 13

//SoftwareSerial right(7, 8);

#define right Serial2

void setup()
{
  pinMode(LED, OUTPUT);
  right.begin(230400);
}

unsigned int last = 0;

void loop()
{
  if (right.available()) {
    byte val = right.read();
    digitalWrite(LED, (1 == (val & 7)) ? HIGH : LOW);
  }
}
