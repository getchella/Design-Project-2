void setup() {
  // put your setup code here, to run once:
  pinMode(9, OUTPUT);
  pinMode(8, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(digitalRead(8)) {
    digitalWrite(9, HIGH);
    delay(1000);
    digitalWrite(9, LOW);
  }
}