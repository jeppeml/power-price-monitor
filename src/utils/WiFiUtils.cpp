#include "esp_wifi_types.h"
#include <WiFi.h>
#include "WiFiUtils.hpp"
#include <Preferences.h>

void connectWiFi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("With ssid: " + String(ssid));
  Serial.println(" and password: " + String(password));
}
