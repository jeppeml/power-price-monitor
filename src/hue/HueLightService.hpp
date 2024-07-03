#ifndef HUELIGHTSERVICE_HPP
#define HUELIGHTSERVICE_HPP

#include <Arduino.h>
#include <HTTPClient.h>
#include <vector>

class HueLightService {
public:
    HueLightService();
    void setAPIKey(const String &key);
    String getAPIKey() const;

    void lightControlXY(const String &hueAddress, const String &groupedLightID, float x, float y);
    void lightControlRGB(const String &hueAddress, const String &groupedLightID, uint8_t r, uint8_t g, uint8_t b);

    bool provisionDevice(const String &hueAddress);
    bool checkAPIKey(const String &hueAddress);
    String getRoomID(const String &hueAddress, const String &roomName);
    String getGroupedLightID(const String &hueAddress, const String &roomID);

private:
    String apiKey;
    String buildRequestBody(bool onoff, float colorx, float colory);
    void rgbToXY(uint8_t r, uint8_t g, uint8_t b, float &x, float &y);
};

#endif // HUELIGHTSERVICE_HPP
