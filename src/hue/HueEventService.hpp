#ifndef HUEEVENTSERVICE_HPP
#define HUEEVENTSERVICE_HPP

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <functional>
#include <vector>

class HueEventService {
public:
    HueEventService(const String &hueAddress, const String &apiKey);
    void begin();
    void loop();
    void onRoomChange(std::function<void(String, std::vector<String>)> callback);

private:
    String hueAddress;
    String apiKey;
    WiFiClientSecure client;
    std::function<void(String, std::vector<String>)> roomChangeCallback;

    void handleEvent(const String &event);
    String parseRoomID(const String &jsonData);
    std::vector<String> parseLights(const String &jsonData);
};

#endif // HUEEVENTSERVICE_HPP
