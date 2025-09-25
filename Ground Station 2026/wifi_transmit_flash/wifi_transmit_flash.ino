#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32-Network";
const char* password = "12345678";

WiFiUDP udp;
IPAddress receiverIP(192, 168, 4, 1);  // default IP of ESP32 AP
#define DEST_PORT 4210

void setup() {
  Serial.begin(115200);

  // Connect to the ESP32 AP (created by receiver)
  WiFi.begin(ssid, password);
  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to AP!");
}

void loop() {
  // Send a test message every 2 seconds
  udp.beginPacket(receiverIP, DEST_PORT);
  udp.print("Hello, other ESP!");
  udp.endPacket();

  Serial.println("Sent: Hello, other ESP!");
  delay(2000);
}
