#include <MCP23017.h>
#include <Wire.h>

MCP23017 MCP(0x20);

#define I2C_CLOCK_SPEED 100000
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  delay(2000);
  Serial.begin(115200*8);
  Serial.print("MCP23017_test version: ");
  Serial.println(MCP23017_LIB_VERSION);

  Wire.setClock(I2C_CLOCK_SPEED);
  Wire.begin();
  Wire.setClock(I2C_CLOCK_SPEED);
  MCP.begin();
  Serial.print("Begin error: ");
  Serial.println(MCP.lastError());
  for (int pin = 0; pin < 16; pin++) {
    MCP.pinMode(pin, pin < 8 ? INPUT_PULLUP : OUTPUT);
    Serial.print("Last error: ");
    Serial.println(MCP.lastError());
  }
  Serial.print("isConnected: ");
  Serial.println(MCP.isConnected());
  while (!MCP.isConnected()) {
    delay(1000);
    MCP.begin();
    Serial.print("Begin error: ");
    Serial.println(MCP.lastError());    
  }
}

void loop()
{
  // Serial.println("  01234567");
  uint64_t res = 0;
  uint8_t pos = 0;
  uint32_t now = millis();
  for (int opin = 8; opin < 16; opin++) {
    MCP.digitalWrite(opin, LOW);
    //delay(10);
    //Serial.print(opin-8);
    //Serial.print(":");
    
    for (int ipin = 0; ipin < 8; ipin++) {
      int val = !MCP.digitalRead(ipin);
      if (val) {
        res = res | (1ULL << pos);
      } else {
        res = res & ~(1ULL << pos);
      }
      pos++;
      //Serial.print(val);
    }
    MCP.digitalWrite(opin, HIGH);
    //delay(10);
  }
  uint32_t time = millis() - now;
  Serial.print("Time: ");
  Serial.print(time);
  Serial.print(" Data: ");
  Serial.println(res);
  delay(10);
}

// void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, ((millis() & 512) == 0) ? HIGH : LOW); 
  //Serial.printf("time: %d\n", millis());
//}
