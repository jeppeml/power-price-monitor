#ifndef CONFIG_SERVICE_HPP
#define CONFIG_SERVICE_HPP

#include <Preferences.h>

class ConfigService {
public:
    ConfigService();
    bool begin();
    void saveSSID(const String& ssid);
    void savePassword(const String& password);
    String loadSSID();
    String loadPassword();
    void saveAPIKey(const String& apiKey);
    String loadAPIKey();
    void saveRoomName(const String& roomName);  
    String loadRoomName();  

private:
    Preferences preferences;
};

#endif // CONFIG_SERVICE_HPP
