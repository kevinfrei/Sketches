#define thepin 17

void setup() {
  pinMode(thepin, OUTPUT);
  Serial.begin(115200);
}


unsigned int last = 0;

void loop() {
  unsigned int now = millis();
  int delta = now - last;
  if (delta > 127) {
    // digitalWrite(thepin, flag ? HIGH : LOW);
    Serial.print(now, DEC); Serial.print(": "); Serial.print(delta, DEC);
    Serial.println(""); 
    last = now;
  }
}
