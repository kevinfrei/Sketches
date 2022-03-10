#include "SoftwareSerial.h" 

// This is for the right side:
SoftwareSerial queen(PIN_SERIAL1_TX, PIN_SERIAL1_RX);
// Dunno if the left needs to be different

// #define queen Serial1


void setup()
{ 
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT); 
  queen.begin(115200);
  /*
   pinMode(PIN_SERIAL1_RX, INPUT_PULLUP);
   pinMode(PIN_SERIAL1_TX, OUTPUT);
   */
}

unsigned int last = 0;
unsigned int number = 0;
void loop(){
  unsigned int now = millis();
  if (now - last < 500) {
    return;
  }
  last = now;
  queen.write((unsigned char)(number++ & 0xFF));
  digitalWrite(LED_RED, ((now >> 8) & 1) ? HIGH : LOW);
  digitalWrite(LED_BLUE, ((now >> 8) & 2) ? HIGH : LOW);
}
