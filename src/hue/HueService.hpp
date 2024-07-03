#ifndef _HUESERVICE_
#define _HUESERVICE_

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

class HueService {
public:
    HueService();
    void refresh();
    String getIP();
    int getPort();
    void saveIPAndPort(const String &ip, int port);
    void loadIPAndPort();

private:
    String ipAddress;
    int portNumber;
    void setup();
    Preferences preferences;
};

#endif
