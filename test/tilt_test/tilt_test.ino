#define tilt  8
#define led   9

void setup() {
  pinMode(tilt, INPUT_PULLUP);
  pinMode(led, OUTPUT);
}

void loop() {
  //if (digitalRead(tilt)) {
    //delay(10);
    if (digitalRead(tilt)) {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
    }
  //}
}
