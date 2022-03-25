#define x   A0 // analog pin connected to X output
#define y   A1   // analog pin connected to Y output
#define led 7

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  if (analogRead(x) > 515 || analogRead(x) < 495 || analogRead(y) > 515 || analogRead(y) < 495) {
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
  }
}
