#include "HueLightService.hpp"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// NOTE: This does NOT use HTTPS as adviced by Signify (https://developers.meethue.com/develop/application-design-guidance/using-https/)

HueLightService::HueLightService() : apiKey("") {}

void HueLightService::setAPIKey(const String &key) {
    apiKey = key;
}

String HueLightService::getAPIKey() const {
    return apiKey;
}

void HueLightService::lightControlXY(const String &hueAddress, const String &groupedLightID, float x, float y) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("HueLightService: WiFi not connected!");
        return;
    }

    HTTPClient http;
    String url = "https://" + hueAddress + "/clip/v2/resource/grouped_light/" + groupedLightID;
    http.begin(url); 

    http.addHeader("Content-Type", "application/json");
    http.addHeader("hue-application-key", apiKey);

    String requestBody = buildRequestBody(true, x, y);
    int httpResponseCode = http.PUT(requestBody);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("HueLightService: Error on sending PUT request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void HueLightService::lightControlRGB(const String &hueAddress, const String &groupedLightID, const uint8_t* colorRGB) {
    float x, y;
    rgbToXY(colorRGB[0], colorRGB[1], colorRGB[2], x, y);
    lightControlXY(hueAddress, groupedLightID, x, y);
}

String HueLightService::buildRequestBody(bool onoff, float colorx, float colory) {
    JsonDocument doc;

    doc["on"]["on"] = onoff;
    //doc["dimming"]["brightness"] = 100.0; commmented out to control from app
    JsonObject color = doc["color"].to<JsonObject>();
    JsonObject xy = color["xy"].to<JsonObject>();
    xy["x"] = colorx;
    xy["y"] = colory;

    String output;
    serializeJson(doc, output);
    return output;
}

void HueLightService::rgbToXY(uint8_t r, uint8_t g, uint8_t b, float &x, float &y) {
    float rNorm = r / 255.0;
    float gNorm = g / 255.0;
    float bNorm = b / 255.0;

    rNorm = (rNorm > 0.04045) ? pow((rNorm + 0.055) / (1.055), 2.4) : rNorm / 12.92;
    gNorm = (gNorm > 0.04045) ? pow((gNorm + 0.055) / (1.055), 2.4) : gNorm / 12.92;
    bNorm = (bNorm > 0.04045) ? pow((bNorm + 0.055) / (1.055), 2.4) : bNorm / 12.92;

    float X = rNorm * 0.4124 + gNorm * 0.3576 + bNorm * 0.1805;
    float Y = rNorm * 0.2126 + gNorm * 0.7152 + bNorm * 0.0722;
    float Z = rNorm * 0.0193 + gNorm * 0.1192 + bNorm * 0.9505;

    x = X / (X + Y + Z);
    y = Y / (X + Y + Z);

    if (isnan(x)) x = 0.0;
    if (isnan(y)) y = 0.0;
}

bool HueLightService::provisionDevice(const String &hueAddress) {
    HTTPClient http;
    String url = "https://" + hueAddress + "/api";
    http.begin(url);  

    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["devicetype"] = "power_prices#jepled";
    doc["generateclientkey"] = true;
    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println("HueLightService: Raw response: " + response);

        if (httpResponseCode == 200) {
            JsonDocument responseDoc;
            DeserializationError error = deserializeJson(responseDoc, response);

            if (error) {
                Serial.print("Deserialization failed: ");
                Serial.println(error.c_str());
                return false;
            }

            for (JsonObject result : responseDoc.as<JsonArray>()) {
                if (result.containsKey("success")) {
                    JsonObject success = result["success"];
                    String username = success["username"];
                    String clientkey = success["clientkey"];

                    Serial.println("Provisioning successful!");
                    Serial.print("Username: ");
                    Serial.println(username);
                    Serial.print("Clientkey: ");
                    Serial.println(clientkey);

                    setAPIKey(username);

                    http.end();
                    return true;
                } else if (result.containsKey("error")) {
                    JsonObject error = result["error"];
                    int errorCode = error["type"];
                    String errorMessage = error["description"];
                    Serial.print("Error code: ");
                    Serial.print(errorCode);
                    Serial.print(" - ");
                    Serial.println(errorMessage);
                }
            }
        } else {
            Serial.println("HueLightService: Provisioning failed with HTTP code: " + String(httpResponseCode));
        }
    } else {
        Serial.print("HueLightService: Error on sending POST request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
    return false;
}

bool HueLightService::checkAPIKey(const String &hueAddress) {
    HTTPClient http;
    String url = "https://" + hueAddress + "/clip/v2/resource/device";
    http.begin(url);  

    http.addHeader("Content-Type", "application/json");
    http.addHeader("hue-application-key", apiKey);

    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
        String response = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
            Serial.print("HueLightService: Deserialization failed: ");
            Serial.println(error.c_str());
            return false;
        }

        if (doc.containsKey("data")) {
            JsonArray data = doc["data"];
            if (!data.isNull()) {
                Serial.println("API key is valid.");
                http.end();
                return true;
            }
        }
    } else {
        Serial.print("API key is invalid. Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Raw response: " + response);
    }

    http.end();
    return false;
}

String HueLightService::getRoomID(const String &hueAddress, const String &roomName) {
    HTTPClient http;
    String url = "https://" + hueAddress + "/clip/v2/resource/room";
    http.begin(url);  

    http.addHeader("Content-Type", "application/json");
    http.addHeader("hue-application-key", apiKey);

    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
        String response = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
            Serial.print("Deserialization failed: ");
            Serial.println(error.c_str());
            return "";
        }

        for (JsonObject room : doc["data"].as<JsonArray>()) {
            String name = room["metadata"]["name"];
            Serial.print("Room found: ");
            Serial.println(name);
            if (name == roomName) {
                return room["id"].as<String>();
            }
        }
    } else {
        Serial.print("Error on sending GET request: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Raw response: " + response);
    }

    http.end();
    return "";
}

String HueLightService::getGroupedLightID(const String &hueAddress, const String &roomID) {
    HTTPClient http;
    String url = "https://" + hueAddress + "/clip/v2/resource/room/" + roomID;
    http.begin(url);  

    http.addHeader("Content-Type", "application/json");
    http.addHeader("hue-application-key", apiKey);

    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
        String response = http.getString();
        Serial.println("HueLightService: Raw response 200: " + response); // Debugging
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (error) {
            Serial.print("HueLightService: Deserialization failed: ");
            Serial.println(error.c_str());
            return "";
        }

        JsonArray data = doc["data"];
        if (!data.isNull()) {
            for (JsonObject room : data) {
                for (JsonObject service : room["services"].as<JsonArray>()) {
                    String rtype = service["rtype"].as<String>();
                    String rid = service["rid"].as<String>();
                    Serial.println("HueLightService: Service Type: " + rtype + ", Service ID: " + rid);  // Debugging
                    if (rtype == "grouped_light") {
                        return rid;
                    }
                }
            }
        }
    } else {
        Serial.print("HueLightService: Error on sending GET request: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Raw response: " + response);
    }

    http.end();
    return "";
}
