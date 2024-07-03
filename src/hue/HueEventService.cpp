#include "HueEventService.hpp"
#include <ArduinoJson.h>

// NOTE: This does NOT use HTTPS as adviced by Signify (https://developers.meethue.com/develop/application-design-guidance/using-https/)


HueEventService::HueEventService(const String &hueAddress, const String &apiKey)
    : hueAddress(hueAddress), apiKey(apiKey), roomChangeCallback(nullptr) {
    client.setInsecure();  // Bypass SSL certificate validation for development! BEWARE!
}

void HueEventService::begin() {
    String url = "https://" + hueAddress + "/eventstream/clip/v2";

    if (!client.connect(hueAddress.c_str(), 443)) {
        Serial.println("Connection to bridge failed");
        return;
    }

    client.print(String("GET /eventstream/clip/v2 HTTP/1.1\r\n") +
                 "Host: " + hueAddress + "\r\n" +
                 "Accept: text/event-stream\r\n" +
                 "hue-application-key: " + apiKey + "\r\n" +
                 "Connection: keep-alive\r\n\r\n");

    while (client.connected() && !client.available()) {
        delay(10);  // Wait for the server to send a response
    }

    if (!client.connected()) {
        Serial.println("Disconnected from the bridge");
        return;
    }

    while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line); // Debugging response
        if (line.startsWith("HTTP/1.1 200")) {
            Serial.println("Event stream started successfully");
            return;
        }
    }

    Serial.println("Failed to start event stream");
}

void HueEventService::loop() {
    if (!client.connected()) {
        Serial.println("Reconnecting to bridge...");
        begin();
    }

    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.startsWith("data: ")) {
            String eventData = line.substring(6);
            eventData.trim();
            handleEvent(eventData);
        }
    }
}

void HueEventService::onRoomChange(std::function<void(String, std::vector<String>)> callback) {
    roomChangeCallback = callback;
}

void HueEventService::handleEvent(const String &event) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, event);

    if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
        return;
    }

    Serial.println("Event received: ");
    serializeJsonPretty(doc, Serial);

    if (doc.containsKey("data")) {
        JsonArray data = doc["data"];
        for (JsonObject obj : data) {
            if (obj.containsKey("type") && obj["type"] == "room") {
                String roomID = obj["id"].as<String>();
                std::vector<String> lights = parseLights(event);
                if (roomChangeCallback && !roomID.isEmpty()) {
                    roomChangeCallback(roomID, lights);
                }
            }
        }
    }
}

String HueEventService::parseRoomID(const String &jsonData) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
        return "";
    }

    if (doc.containsKey("data")) {
        JsonArray data = doc["data"];
        for (JsonObject obj : data) {
            if (obj.containsKey("id") && obj["type"] == "room") {
                return obj["id"].as<String>();
            }
        }
    }

    return "";
}

std::vector<String> HueEventService::parseLights(const String &jsonData) {
    std::vector<String> lights;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error) {
        Serial.print("Deserialization failed: ");
        Serial.println(error.c_str());
        return lights;
    }

    if (doc.containsKey("data")) {
        JsonArray data = doc["data"];
        for (JsonObject obj : data) {
            if (obj.containsKey("children")) {
                for (JsonObject child : obj["children"].as<JsonArray>()) {
                    if (child["rtype"] == "device") {
                        lights.push_back(child["rid"].as<String>());
                    }
                }
            }
        }
    }

    return lights;
}
