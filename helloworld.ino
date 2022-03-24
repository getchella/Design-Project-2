void setup() {
  // put your setup code here, to run once:
  pinMode(9, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  //jack rozzo here, how's it going
}

void loop() {
  // put your main code here, to run repeatedly:
  while(digitalRead(8)) {
    digitalWrite(9, HIGH);
    delay(1000);
    digitalWrite(9, LOW);
  }
}

// This is a comment. The author of this comment is Curtis. I am very good at writing comments
// This is a comment by Alex Getchell. I am better than Curtis at writing comments
