#ifndef WIFI_SETUP_SERVICE_HPP
#define WIFI_SETUP_SERVICE_HPP

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "../utils/ConfigService.hpp"

class WiFiSetupService
{
public:
    WiFiSetupService(ConfigService &configService);
    void startAPAndResetWhenDone();

private:
    void begin();
    void loopAndProcess();
    void startAccessPoint();
    void handleRoot();
    void handleSetWiFi();
    void handleNotFound();
    void saveTariffPricesAndColors();
    void loadTariffPricesAndColors();

    float summerLowPrice;
    float summerMedPrice;
    float summerHighPrice;
    float winterLowPrice;
    float winterMedPrice;
    float winterHighPrice;

    String summerLowColor;
    String summerMedColor;
    String summerHighColor;
    String winterLowColor;
    String winterMedColor;
    String winterHighColor;

    uint32_t convertColorToRGB(String hexColor);

    WebServer server;
    DNSServer dnsServer;
    Preferences preferences;
    ConfigService &configService;

    const char *ap_ssid = "PowerPriceMonitor";
    const char *ap_password = "12345678";
    const byte DNS_PORT = 53;

    String urlDecode(const String &text);
};

#endif // WIFI_SETUP_SERVICE_HPP
