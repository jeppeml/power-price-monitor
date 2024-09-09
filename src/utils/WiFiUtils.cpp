#include "esp_wifi_types.h"
#include <WiFi.h>
#include "WiFiUtils.hpp"

void connectWiFi(String ssid, String password) {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
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
