#include "ConfigService.hpp"

ConfigService::ConfigService() {}

bool ConfigService::begin() {
    return preferences.begin("wifi", false);
}

void ConfigService::saveSSID(const String& ssid) {
    preferences.putString("ssid", ssid);
}

void ConfigService::savePassword(const String& password) {
    preferences.putString("password", password);
}

String ConfigService::loadSSID() {
    return preferences.getString("ssid", "");
}

String ConfigService::loadPassword() {
    return preferences.getString("password", "");
}

void ConfigService::saveAPIKey(const String& apiKey) {
    preferences.putString("apiKey", apiKey);
}

String ConfigService::loadAPIKey() {
    return preferences.getString("apiKey", "");
}

void ConfigService::saveRoomName(const String& roomName) {
    preferences.putString("roomName", roomName);
}

String ConfigService::loadRoomName() {
    return preferences.getString("roomName", "");
}
