#include "WiFiSetupService.hpp"
#include "WifiSetup.html.hpp" // HTML code disguised as a header. Not sure if this is a good idea, but here we go anyway

WiFiSetupService::WiFiSetupService(ConfigService &configService)
    : server(80), configService(configService) {}

void WiFiSetupService::startAPAndResetWhenDone()
{
    startAccessPoint();
    begin();
    loopAndProcess();
}

void WiFiSetupService::begin()
{
    server.on("/", std::bind(&WiFiSetupService::handleRoot, this));
    server.on("/setwifi", HTTP_POST, std::bind(&WiFiSetupService::handleSetWiFi, this));
    server.onNotFound(std::bind(&WiFiSetupService::handleNotFound, this));
    server.begin();
}

void WiFiSetupService::loopAndProcess()
{
    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();
        delay(10);
    }
}

void WiFiSetupService::startAccessPoint()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access Point started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    // begin();
}

void WiFiSetupService::handleRoot()
{
    // Serve the embedded HTML content
    Serial.println("Serving root HTML page.");
    server.send(200, "text/html", wifi_setup_html);
}

void hexToRGB(const String &hexColor, uint8_t rgb[3])
{
    long number = strtol(hexColor.c_str() + 1, NULL, 16); // Skip the '#' character
    rgb[0] = (number >> 16) & 0xFF;                       // Extract the red component
    rgb[1] = (number >> 8) & 0xFF;                        // Extract the green component
    rgb[2] = number & 0xFF;                               // Extract the blue component
}

uint32_t rgbToUint32(const uint8_t rgb[3])
{
    return ((uint32_t)rgb[0] << 16) | ((uint32_t)rgb[1] << 8) | rgb[2];
}

void WiFiSetupService::handleSetWiFi()
{
    Serial.println("Handling WiFi setup request.");
    Serial.print("Number of arguments received: ");
    Serial.println(server.args());

    // Loop through all arguments and print the name and value
    for (int i = 0; i < server.args(); i++)
    {
        Serial.print("Argument name: ");
        Serial.println(server.argName(i)); // Print the name of the argument
        Serial.print("Argument value: ");
        Serial.println(server.arg(i)); // Print the value of the argument
    }

    if (server.hasArg("ssid") && server.hasArg("password"))
    {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        // Save WiFi configuration
        configService.saveSSID(ssid);
        configService.savePassword(password);

        if (server.hasArg("roomName"))
        {
            String roomName = server.arg("roomName");
            configService.saveRoomName(roomName);
        }

        if (server.hasArg("apiKey"))
        {
            String apiKey = server.arg("apiKey");
            configService.saveAPIKey(apiKey);
        }

        // Handle color and price settings
        if (server.hasArg("priceHigh") && server.hasArg("priceMedium") && server.hasArg("priceLow"))
        {
            double priceHigh = server.arg("priceHigh").toDouble();
            double priceMedium = server.arg("priceMedium").toDouble();
            double priceLow = server.arg("priceLow").toDouble();

            String colorHighHex = server.arg("colorPriceHigh");
            String colorMediumHex = server.arg("colorPriceMedium");
            String colorLowHex = server.arg("colorPriceLow");
            String colorVeryLowHex = server.arg("colorPriceVeryLow");

            uint8_t colorHighRGB[3], colorMediumRGB[3], colorLowRGB[3], colorVeryLowRGB[3];
            hexToRGB(colorHighHex, colorHighRGB);
            hexToRGB(colorMediumHex, colorMediumRGB);
            hexToRGB(colorLowHex, colorLowRGB);
            hexToRGB(colorVeryLowHex, colorVeryLowRGB);

            uint32_t colorHigh = rgbToUint32(colorHighRGB);
            uint32_t colorMedium = rgbToUint32(colorMediumRGB);
            uint32_t colorLow = rgbToUint32(colorLowRGB);
            uint32_t colorVeryLow = rgbToUint32(colorVeryLowRGB);

            // Save the price and color configuration
            configService.savePriceColorConfig(priceHigh, priceMedium, priceLow,
                                               colorHigh, colorMedium, colorLow, colorVeryLow);
        }

        // Check if all tariff prices are present
        if (server.hasArg("summerLow") && server.hasArg("summerMedium") &&
            server.hasArg("summerHigh") && server.hasArg("winterLow") &&
            server.hasArg("winterMedium") && server.hasArg("winterHigh"))
        {
            Serial.println("tariff HANDLING BEING DONE!");
            // Fetch tariff prices into local variables
            double summerLowPrice = server.arg("summerLow").toDouble();
            double summerMedPrice = server.arg("summerMedium").toDouble();
            double summerHighPrice = server.arg("summerHigh").toDouble();
            double winterLowPrice = server.arg("winterLow").toDouble();
            double winterMedPrice = server.arg("winterMedium").toDouble();
            double winterHighPrice = server.arg("winterHigh").toDouble();

            // Save the summer and winter tariffs
            configService.saveSummerPrices(summerLowPrice, summerMedPrice, summerHighPrice);
            configService.saveWinterPrices(winterLowPrice, winterMedPrice, winterHighPrice);
        }
        else{
            Serial.println("tariff HANDLING FAILED MISERABLY");
        }

        server.send(200, "text/html", "Configuration saved! Device will reboot now.");
        delay(4000);
        ESP.restart(); // Restart the ESP32 to apply new settings
    }
    else
    {
        server.send(400, "text/html", "Missing required SSID, password, or room name fields!");
    }
}

uint32_t WiFiSetupService::convertColorToRGB(String hexColor)
{
    // Remove the leading '#' if present
    if (hexColor.startsWith("#"))
    {
        hexColor.remove(0, 1);
    }

    // Convert the string to an unsigned 32-bit integer
    uint32_t rgb = (uint32_t)strtol(hexColor.c_str(), NULL, 16);
    return rgb;
}

void WiFiSetupService::handleNotFound()
{
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
}

// Function to decode URL-encoded strings, enables foreign characters like "ø"
String WiFiSetupService::urlDecode(const String &text)
{
    String decoded = "";
    char temp[] = "0x00";
    unsigned int len = text.length();
    unsigned int i = 0;

    while (i < len)
    {
        char c = text.charAt(i);
        if (c == '+')
        {
            decoded += ' ';
        }
        else if (c == '%')
        {
            if (i + 2 < len)
            {
                temp[2] = text.charAt(i + 1);
                temp[3] = text.charAt(i + 2);
                decoded += strtol(temp, nullptr, 16);
                i += 2;
            }
        }
        else
        {
            decoded += c;
        }
        i++;
    }

    return decoded;
}
