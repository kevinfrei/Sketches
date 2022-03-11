#include "Adafruit_TinyUSB.h"

#define queen Serial1

void setup()
{ 
  queen.begin(1<<20); // 1Mbps
}

unsigned int last = 0;
unsigned int number = 0;
void loop(){
  unsigned int now = millis();
  if (now - last < 325) {
    return;
  }
  last = now;
  queen.write((unsigned char)(number++ & 0xFF));
}
