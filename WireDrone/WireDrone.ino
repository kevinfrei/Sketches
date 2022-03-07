#include <SoftwareSerial.h>

#define RED_LED 17
#define BLUE_LED 19

// This is for the right side:
SoftwareSerial queen(6, 8);
// Dunno if the left needs to be different

void setup()
{
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);  
  queen.begin(230400);
}

unsigned int last = 0;
void loop(){
  unsigned int now = millis();
  if (now - last > 127) {
    byte val1 = (now >> 7) & 0xFF;
    queen.write(val1);
    last = now;
  }
}
