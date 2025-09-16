// Motor A pins
const int dirPinA = 2;
const int stepPinA = 3;

// Motor B pins
const int dirPinB = 4;
const int stepPinB = 5;

// Constants
const int quarterTurnSteps = 400; // 1/4 turn at 1/8 microstepping (200 steps/rev × 8 ÷ 4)

void setup() {
  // Set all motor pins as outputs
  pinMode(dirPinA, OUTPUT);
  pinMode(stepPinA, OUTPUT);
  pinMode(dirPinB, OUTPUT);
  pinMode(stepPinB, OUTPUT);

  // Move forward (1/4 turn)
  digitalWrite(dirPinA, HIGH);
  digitalWrite(dirPinB, HIGH);

  for (int i = 0; i < quarterTurnSteps; i++) {
    digitalWrite(stepPinA, HIGH);
    digitalWrite(stepPinB, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPinA, LOW);
    digitalWrite(stepPinB, LOW);
    delay(5);
  }

  delay(1000); // Pause briefly before moving back

  // Move backward (return to home)
  digitalWrite(dirPinA, LOW);
  digitalWrite(dirPinB, LOW);

  for (int i = 0; i < quarterTurnSteps; i++) {
    digitalWrite(stepPinA, HIGH);
    digitalWrite(stepPinB, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPinA, LOW);
    digitalWrite(stepPinB, LOW);
    delay(5);
  }
}

void loop() {
  // Nothing here — runs only once
}


