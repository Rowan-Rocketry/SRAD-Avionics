// Define stepper motor 1 connections and steps per revolution:
#define dirPin1 2
#define stepPin1 3

// Define stepper motor 2 connections:
#define dirPin2 4
#define stepPin2 5

#define stepsPerRevolution 200

void setup() {
  // Declare pins as outputs for motor 1:
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);

  // Declare pins as outputs for motor 2:
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
}

void loop() {
  // Set both motors to spin clockwise:
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);

  // Spin both motors 1 revolution slowly:
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(2000);
  }

  delay(1000);

  // Set both motors to spin counterclockwise:
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);

  // Spin both motors 1 revolution quickly:
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(1000);
  }

  delay(1000);

  // Set motors to opposite directions:
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);

  // Spin both motors 5 revolutions fast:
  for (int i = 0; i < 5 * stepsPerRevolution; i++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(500);
  }

  delay(1000);
}
