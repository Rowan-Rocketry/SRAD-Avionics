#include <Arduino_LSM6DSOX.h>

float ax, ay, az;	// Accelerometer readings
float gx, gy, gz;	// Gyro readings

void setup() {
	// Initialize serial communications
	Serial.begin(9600);
	while(!Serial);

	// Attempt to initialize IMU
	if (!IMU.begin())
	{
		Serial.println("Failed to initialize IMU");
		while(1);
	}
}

void loop() {
	if (IMU.accelerationAvailable())
	{
		IMU.readAcceleration(ax, ay, az);
	}
	if (IMU.gyroscopeAvailable())
	{
		IMU.readGyroscope(gx, gy, gz);
	}
	Serial.print("Accelerometer: ");
	Serial.print(ax);
	Serial.print(", ");
	Serial.print(ay);
	Serial.print(", ");
	Serial.print(az);

	Serial.print("\tGyroscope: ");
	Serial.print(gx);
	Serial.print(", ");
	Serial.print(gy);
	Serial.print(", ");
	Serial.print(gz);
	Serial.print("\r");
}
