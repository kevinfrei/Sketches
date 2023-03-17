#include <Adafruit_MCP23X17.h>
#include <Wire.h>

Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// "Fast" mode. MCP23017 docs say it supports up to 1.7MHz...
// Teensy 3.2 supports upt to 1MHz
#define I2C_CLOCK_SPEED 1000000 
#define ADDR1 0x24
#define ADDR2 0x27
#define PINA 8
#define PINB 0

bool success1 = false;
bool success2 = false;
#define ON HIGH
#define OFF LOW

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  delay(2000);

  Serial.println("MCP23017_test");
  
  Wire.setClock(I2C_CLOCK_SPEED);
  Wire.begin();
  Wire.setClock(I2C_CLOCK_SPEED);
  
  success1 = mcp1.begin_I2C(ADDR1, &Wire);
  success2 = mcp2.begin_I2C(ADDR2, &Wire);
  
  if (!success1) {
    Serial.println("MCP1 Begin failed");
  }
  if (!success2) {
    Serial.println("MCP2 Begin failed");
  }
  if (success1) {
    mcp1.pinMode(PINA, INPUT_PULLUP);
    mcp1.pinMode(PINB, OUTPUT);
    mcp1.digitalWrite(PINB, OFF);
  }
  if (success2) {
    mcp2.pinMode(PINB, INPUT_PULLUP);
    mcp2.pinMode(PINA, OUTPUT);
    mcp2.digitalWrite(PINA, OFF);   
  }
}

void handlePair(Adafruit_MCP23X17 &mcp, uint8_t in, uint8_t out) {
  mcp.digitalWrite(out, ON);
  delay(5);
  Serial.print(mcp.digitalRead(in) ? "T" : "F");
  Serial.print(in);
  delay(5);
  mcp2.digitalWrite(out, OFF);
  delay(5);
  Serial.print(mcp.digitalRead(in) ? "-t" : "-f");
  Serial.println(in);
  delay(5);  
}

uint32_t last = 0;
void loop() {
  if (last == (millis() & 0x400)) {
    return;
  }
  last = millis() & 0x400;
  digitalWrite(LED_BUILTIN, last ? HIGH : LOW);    
  if (!success1 && !success2) {
    return;
  }
  Serial.print("Time: ");
  Serial.println(millis() >> 10);
  if (success2) {
    handlePair(mcp2, PINA, PINB);
  }
  if (success1) {
    handlePair(mcp1, PINB, PINA);
  }
}
