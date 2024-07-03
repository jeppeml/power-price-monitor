#include "HueService.hpp"

// This class uses mDNS to discover the IP address of the Hue Bridge

HueService::HueService() : ipAddress(""), portNumber(-1) {
  setup();
  loadIPAndPort();
  refresh();
}

void HueService::setup() {
  if (!MDNS.begin("PowerPrice_mDNS")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("MDNS started (begin)");
}

void HueService::refresh() {
  int n = MDNS.queryService("hue", "tcp");
  if (n == 0) {
    Serial.println("no services found");
    return;
  }

  Serial.println("MDNS queryService done");
  ipAddress = MDNS.IP(0).toString();
  portNumber = MDNS.port(0);
  String hostAddress = MDNS.hostname(0) + ":" + String(MDNS.port(0));
  Serial.print("Found hue host: ");
  Serial.println(hostAddress);
  saveIPAndPort(ipAddress, portNumber); // Save IP and port after discovering them
}

String HueService::getIP() {
  return ipAddress;
}

int HueService::getPort() {
  return portNumber;
}

void HueService::saveIPAndPort(const String &ip, int port) {
  preferences.begin("hue", false);
  preferences.putString("ip", ip);
  preferences.putInt("port", port);
  preferences.end();
}

void HueService::loadIPAndPort() {
  preferences.begin("hue", true);
  ipAddress = preferences.getString("ip", "");
  portNumber = preferences.getInt("port", -1);
  preferences.end();

  if (ipAddress != "" && portNumber != -1) {
    Serial.println("Loaded saved IP and port:");
    Serial.print("IP: ");
    Serial.println(ipAddress);
    Serial.print("Port: ");
    Serial.println(portNumber);
  }
}
