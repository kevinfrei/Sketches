/*
HC-SR04 Ping distance sensor]
VCC to arduino 5v GND to arduino GND
Echo to Arduino pin 13 Trig to Arduino pin 12
Red POS to Arduino pin 11
Green POS to Arduino pin 10
560 ohm resistor to both LED NEG and GRD power rail
More info at: http://goo.gl/kJ8Gl
Original code improvements to the Ping sketch sourced from Trollmaker.com
Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/

#define trigPin 5
#define echoPin 6
#define led PIN_LED
#define led2 PIN_LED2

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(led,LOW); // When the Red condition is met, the Green LED should turn off
    digitalWrite(led2,HIGH);
  } else {
    digitalWrite(led, HIGH);
    digitalWrite(led2,LOW);
  }
/*  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else*/ {
    Serial.print(distance);
    Serial.print(" cm (");
    Serial.print(duration);
    Serial.println(" us)");
  }
}
