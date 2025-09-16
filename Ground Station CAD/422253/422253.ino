#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

AccelStepper stepper(motorInterfaceType, stepPin, dirPin);

// Motor and gearing config
const float stepsPerRevolution = 200.0;
const float microstepping = 16.0;
const float gearRatio = 1.0;
const float totalSteps = stepsPerRevolution * microstepping * gearRatio;

// Converts angle to steps
long angleToSteps(float angle) {
  return (long)((angle / 360.0) * totalSteps);
}

// Converts steps to angle
float stepsToAngle(long steps) {
  return (float(steps) / totalSteps) * 360.0;
}

// Normalize to -180 to +180 range
float normalizeAngle(float angle) {
  while (angle > 180.0) angle -= 360.0;
  while (angle < -180.0) angle += 360.0;
  return angle;
}

float getBearing(float lat1, float lon1, float lat2, float lon2) {
  float phi1 = radians(lat1);
  float phi2 = radians(lat2);
  float deltaLambda = radians(lon2 - lon1);

  float y = sin(deltaLambda) * cos(phi2);
  float x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(deltaLambda);
  float theta = atan2(y, x);
  return fmod((degrees(theta) + 360.0), 360.0);
}

void setup() {
  stepper.setMaxSpeed(200);
  stepper.setAcceleration(50);

  float homeLat = 40.8000;
  float homeLon = -82.8500;
  float targetLat = 40.8120;
  float targetLon = -82.8600;

  float targetBearing = getBearing(homeLat, homeLon, targetLat, targetLon); // 0–360°
  float currentAngle = stepsToAngle(stepper.currentPosition());             // Actual motor angle
  float deltaAngle = normalizeAngle(targetBearing - currentAngle);         // Shortest turn

  long stepOffset = angleToSteps(deltaAngle);                              // How many steps to move
  stepper.move(stepOffset);
  stepper.runToPosition();
}

void loop() {
  // Nothing here for now
}

