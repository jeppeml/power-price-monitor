#include "ConfigService.hpp"

ConfigService::ConfigService()
{
    // Initialize Preferences or any other storage system
}

bool ConfigService::begin()
{
    return preferences.begin("power-price", false); // Open NVS
}

// WiFi settings
String ConfigService::loadSSID()
{
    return preferences.getString("ssid", "");
}

void ConfigService::saveSSID(const String &ssid)
{
    preferences.putString("ssid", ssid);
}

String ConfigService::loadPassword()
{
    return preferences.getString("password", "");
}

void ConfigService::savePassword(const String &password)
{
    preferences.putString("password", password);
}

// Room name and API key
String ConfigService::loadRoomName()
{
    return preferences.getString("roomName", "");
}

void ConfigService::saveRoomName(const String &roomName)
{
    preferences.putString("roomName", roomName);
}

String ConfigService::loadAPIKey()
{
    return preferences.getString("apiKey", "");
}

void ConfigService::saveAPIKey(const String &apiKey)
{
    preferences.putString("apiKey", apiKey);
}

// Price intervals
double ConfigService::loadPriceHigh()
{
    return preferences.getDouble("priceHigh", 1.0); // Default to 1.0
}

void ConfigService::savePriceHigh(double priceHigh)
{
    preferences.putDouble("priceHigh", priceHigh);
}

double ConfigService::loadPriceMedium()
{
    return preferences.getDouble("priceMedium", 0.5); // Default to 0.5
}

void ConfigService::savePriceMedium(double priceMedium)
{
    preferences.putDouble("priceMedium", priceMedium);
}

double ConfigService::loadPriceLow()
{
    return preferences.getDouble("priceLow", 0.0); // Default to 0.0
}

void ConfigService::savePriceLow(double priceLow)
{
    preferences.putDouble("priceLow", priceLow);
}

// Helper method to convert uint32_t to RGB array
void uint32ToRGB(uint32_t color, uint8_t rgb[3])
{
    rgb[0] = (color >> 16) & 0xFF; // Extract the red component
    rgb[1] = (color >> 8) & 0xFF;  // Extract the green component
    rgb[2] = color & 0xFF;         // Extract the blue component
}

// RGB colors - High
uint8_t *ConfigService::loadColorHigh()
{
    uint32_t color = preferences.getUInt("colorHigh", 0xFF0000); // Default to red (#FF0000)
    uint32ToRGB(color, colorBufferHigh);
    return colorBufferHigh;
}

void ConfigService::saveColorHigh(uint32_t colorHigh)
{
    preferences.putUInt("colorHigh", colorHigh);
}

// RGB colors - Medium
uint8_t *ConfigService::loadColorMedium()
{
    uint32_t color = preferences.getUInt("colorMedium", 0xFFFF00); // Default to yellow (#FFFF00)
    uint32ToRGB(color, colorBufferMedium);
    return colorBufferMedium;
}

void ConfigService::saveColorMedium(uint32_t colorMedium)
{
    Serial.print("Saving Color Medium: ");
    Serial.println(colorMedium, HEX); // Print in HEX format
    preferences.putUInt("colorMedium", colorMedium);
}

// RGB colors - Low
uint8_t *ConfigService::loadColorLow()
{
    uint32_t color = preferences.getUInt("colorLow", 0x00FF00); // Default to green (#00FF00)
    uint32ToRGB(color, colorBufferLow);
    return colorBufferLow;
}

void ConfigService::saveColorLow(uint32_t colorLow)
{
    preferences.putUInt("colorLow", colorLow);
}

// RGB colors - Very Low
uint8_t *ConfigService::loadColorVeryLow()
{
    uint32_t color = preferences.getUInt("colorVeryLow", 0xFFFFFF); // Default to white (#FFFFFF)
    uint32ToRGB(color, colorBufferVeryLow);
    return colorBufferVeryLow;
}

void ConfigService::saveColorVeryLow(uint32_t colorVeryLow)
{
    preferences.putUInt("colorVeryLow", colorVeryLow);
}

// Convenience method for saving all price intervals and colors at once
void ConfigService::savePriceColorConfig(double priceHigh, double priceMedium, double priceLow,
                                         uint32_t colorHigh, uint32_t colorMedium,
                                         uint32_t colorLow, uint32_t colorVeryLow)
{
    savePriceHigh(priceHigh);
    savePriceMedium(priceMedium);
    savePriceLow(priceLow);

    saveColorHigh(colorHigh);
    saveColorMedium(colorMedium);
    saveColorLow(colorLow);
    saveColorVeryLow(colorVeryLow);
}
