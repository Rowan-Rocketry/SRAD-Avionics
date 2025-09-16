#include <math.h>

// Stepper motor pin definitions
#define dirPin1 2  // Azimuth
#define stepPin1 3
#define dirPin2 4  // Elevation
#define stepPin2 5

#define stepsPerRevolution 200
#define motorStepAngle 1.8  // degrees per full step (adjust if using microstepping)
#define gearRatio 1.0       // adjust if you're using a gearbox

// Current motor angles (in degrees)
float currentAzimuth = 0.0;
float currentElevation = 0.0;

void setup() {
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Dummy coordinates (observer and target)
  double lat1 = 20.0000;     // Observer latitude
  double lon1 = -83.0000;    // Observer longitude
  double lat2 = 40.0020;     // Target latitude
  double lon2 = -33.0010;    // Target longitude
  double alt2 = 200;         // Target altitude relative to observer (meters)

  float azimuth, elevation;
  computeAzEl(lat1, lon1, lat2, lon2, alt2, &azimuth, &elevation);

  // Round to nearest degree to avoid jitter
  azimuth = round(azimuth);
  elevation = round(elevation);

  // Move motors
  moveToAngle(stepPin1, dirPin1, currentAzimuth, azimuth, "Azimuth");
  moveToAngle(stepPin2, dirPin2, currentElevation, elevation, "Elevation");

  // Update state
  currentAzimuth = azimuth;
  currentElevation = elevation;

  delay(5000); // Wait before next pointing cycle
}

// Compute azimuth and elevation angle from observer to target
void computeAzEl(double lat1, double lon1, double lat2, double lon2, double alt2, float* azimuth, float* elevation) {
  double dLon = radians(lon2 - lon1);
  double dLat = radians(lat2 - lat1);
  double phi1 = radians(lat1);
  double phi2 = radians(lat2);

  // Azimuth
  double y = sin(dLon) * cos(phi2);
  double x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(dLon);
  double bearing = atan2(y, x);
  *azimuth = fmod((degrees(bearing) + 360.0), 360.0);  // Normalize 0–360

  // Distance on Earth's surface (haversine formula)
  double R = 6371000.0; // Earth radius in meters
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(phi1) * cos(phi2) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double horizontalDistance = R * c;

  // Elevation angle
  *elevation = degrees(atan2(alt2, horizontalDistance));
}

// Move stepper motor to a target angle
void moveToAngle(int stepPin, int dirPin, float currentAngle, float targetAngle, const char* label) {
  float delta = targetAngle - currentAngle;

  // Ignore small changes under 1°
  if (abs(delta) < 1.0) {
    Serial.print(label); Serial.println(": Skip (under 1°)");
    return;
  }

  int direction = (delta >= 0) ? HIGH : LOW;
  int steps = round(abs(delta) / motorStepAngle * gearRatio);

  Serial.print(label);
  Serial.print(" | From: ");
  Serial.print(currentAngle);
  Serial.print("° -> ");
  Serial.print(targetAngle);
  Serial.print("° | Steps: ");
  Serial.println(steps);

  digitalWrite(dirPin, direction);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800); // adjust if needed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
}
