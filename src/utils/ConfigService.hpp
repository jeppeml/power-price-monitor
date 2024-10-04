#ifndef CONFIG_SERVICE_HPP
#define CONFIG_SERVICE_HPP

#include <Preferences.h>

class ConfigService {
public:
    ConfigService();

    bool begin();

    // Getters and setters for WiFi settings
    String loadSSID();
    void saveSSID(const String& ssid);

    String loadPassword();
    void savePassword(const String& password);

    // Getters and setters for room name and API key
    String loadRoomName();
    void saveRoomName(const String& roomName);

    String loadAPIKey();
    void saveAPIKey(const String& apiKey);

    // Getters and setters for price intervals
    double loadPriceHigh();
    void savePriceHigh(double priceHigh);

    double loadPriceMedium();
    void savePriceMedium(double priceMedium);

    double loadPriceLow();
    void savePriceLow(double priceLow);

    // Getters and setters for RGB colors (using uint32_t for saving and uint8_t* for returning)
    uint8_t* loadColorHigh();
    void saveColorHigh(uint32_t colorHigh);

    uint8_t* loadColorMedium();
    void saveColorMedium(uint32_t colorMedium);

    uint8_t* loadColorLow();
    void saveColorLow(uint32_t colorLow);

    uint8_t* loadColorVeryLow();
    void saveColorVeryLow(uint32_t colorVeryLow);

    // Convenience method to save all price intervals and colors at once
    void savePriceColorConfig(double priceHigh, double priceMedium, double priceLow, 
                              uint32_t colorHigh, uint32_t colorMedium, 
                              uint32_t colorLow, uint32_t colorVeryLow);


    // Setting net tariffs. These are fixed, as there is no API available
    void saveSummerPrices(double low, double medium, double high);
    void saveWinterPrices(double low, double medium, double high);

    double loadSummerLow();
    double loadSummerMedium();
    double loadSummerHigh();

    double loadWinterLow();
    double loadWinterMedium();
    double loadWinterHigh();

private:
    Preferences preferences;
    uint8_t colorBufferHigh[3];  // Buffer for high color
    uint8_t colorBufferMedium[3];  // Buffer for medium color
    uint8_t colorBufferLow[3];  // Buffer for low color
    uint8_t colorBufferVeryLow[3];  // Buffer for very low color
};

#endif // CONFIG_SERVICE_HPP
