#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "ESP32-Network";
const char* password = "12345678";

WiFiUDP udp;
#define LISTEN_PORT 4210

void setup() {
  Serial.begin(115200);

  // Start ESP32 as WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start listening for UDP packets
  udp.begin(LISTEN_PORT);
  Serial.println("UDP server listening...");
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incoming[255];
    int len = udp.read(incoming, sizeof(incoming) - 1);
    if (len > 0) {
      incoming[len] = '\0';
    }

    Serial.print("Received: ");
    Serial.println(incoming);
  }
}

