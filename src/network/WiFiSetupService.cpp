#include "WiFiSetupService.hpp"
#include "WifiSetup.html.hpp" // HTML code disguised as a header. This is to avoid using flash storage as this can be deleted when resetting the device

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
    server.send(200, "text/html", wifi_setup_html);
}

/*
void WiFiSetupService::handleRoot() {
    String html = "<html>\
    <head>\
    <meta charset=\"UTF-8\">\
    <title>Power Price Monitor</title>\
    <style>\
      body { font-family: Arial, sans-serif; }\
      #language-selector { position: absolute; top: 10px; right: 10px; }\
      svg { cursor: pointer; margin: 0 5px; }\
      select option { background-color: white; color: black; padding: 5px; }\
      option.red { background-color: #FF0000; color: white; }\
      option.green { background-color: #00FF00; color: white; }\
      option.blue { background-color: #0000FF; color: white; }\
      option.purple { background-color: #800080; color: white; }\
      option.yellow { background-color: #FFFF00; color: black; }\
      option.orange { background-color: #FFA500; color: white; }\
      option.white { background-color: #FFFFFF; color: black; }\
    </style>\
    </head>\
    <body>\
    <div id=\"language-selector\">\
      <span onclick=\"setLanguage('da')\">\
        <svg width=\"30\" height=\"20\" viewBox=\"0 0 16 12\">\
          <rect width=\"16\" height=\"12\" fill=\"#C60C30\" />\
          <rect x=\"5\" width=\"2\" height=\"12\" fill=\"white\" />\
          <rect y=\"5\" width=\"16\" height=\"2\" fill=\"white\" />\
        </svg>\
      </span>\
      <span onclick=\"setLanguage('en')\">\
        <svg width=\"30\" height=\"20\" viewBox=\"0 0 16 12\">\
          <rect width=\"16\" height=\"12\" fill=\"#00247D\" />\
          <rect y=\"4\" width=\"16\" height=\"1.5\" fill=\"white\" />\
          <rect x=\"5\" width=\"1.5\" height=\"12\" fill=\"white\" />\
        </svg>\
      </span>\
    </div>\
    <h1 id=\"pageTitle\">WiFi Setup</h1>\
    <form action=\"/setwifi\" method=\"POST\">\
      <label for=\"ssid\" id=\"labelSSID\">SSID:</label><br>\
      <input type=\"text\" id=\"ssid\" name=\"ssid\"><br>\
      <label for=\"password\" id=\"labelPassword\">Password:</label><br>\
      <input type=\"password\" id=\"password\" name=\"password\"><br>\
      <label for=\"roomName\" id=\"labelRoom\">Room Name:</label><br>\
      <input type=\"text\" id=\"roomName\" name=\"roomName\"><br><br>\
      <h2 id=\"colorSettingsTitle\">Color and Price Interval Settings</h2>\
      <label for=\"priceHigh\" id=\"labelPriceHigh\">Price High (kr/kWh):</label><br>\
      <input type=\"number\" id=\"priceHigh\" name=\"priceHigh\" value=\"1.0\" step=\"0.01\"><br>\
      <label for=\"colorPriceHigh\" id=\"labelColorPriceHigh\">Color for High Price:</label><br>\
      <select id=\"colorPriceHigh\" name=\"colorPriceHigh\">\
        <option class=\"red\" value=\"#FF0000\">Red</option>\
        <option class=\"green\" value=\"#00FF00\">Green</option>\
        <option class=\"blue\" value=\"#0000FF\">Blue</option>\
        <option class=\"purple\" value=\"#800080\">Purple</option>\
        <option class=\"yellow\" value=\"#FFFF00\">Yellow</option>\
        <option class=\"orange\" value=\"#FFA500\">Orange</option>\
      </select><br>\
      <label for=\"priceMedium\" id=\"labelPriceMedium\">Price Medium (kr/kWh):</label><br>\
      <input type=\"number\" id=\"priceMedium\" name=\"priceMedium\" value=\"0.5\" step=\"0.01\"><br>\
      <label for=\"colorPriceMedium\" id=\"labelColorPriceMedium\">Color for Medium Price:</label><br>\
      <select id=\"colorPriceMedium\" name=\"colorPriceMedium\">\
        <option class=\"red\" value=\"#FF0000\">Red</option>\
        <option class=\"green\" value=\"#00FF00\">Green</option>\
        <option class=\"blue\" value=\"#0000FF\">Blue</option>\
        <option class=\"purple\" value=\"#800080\">Purple</option>\
        <option class=\"yellow\" value=\"#FFFF00\">Yellow</option>\
        <option class=\"orange\" value=\"#FFA500\">Orange</option>\
      </select><br>\
      <label for=\"priceLow\" id=\"labelPriceLow\">Price Low (kr/kWh):</label><br>\
      <input type=\"number\" id=\"priceLow\" name=\"priceLow\" value=\"0.0\" step=\"0.01\"><br>\
      <label for=\"colorPriceLow\" id=\"labelColorPriceLow\">Color for Low Price:</label><br>\
      <select id=\"colorPriceLow\" name=\"colorPriceLow\">\
        <option class=\"red\" value=\"#FF0000\">Red</option>\
        <option class=\"green\" value=\"#00FF00\">Green</option>\
        <option class=\"blue\" value=\"#0000FF\">Blue</option>\
        <option class=\"purple\" value=\"#800080\">Purple</option>\
        <option class=\"yellow\" value=\"#FFFF00\">Yellow</option>\
        <option class=\"orange\" value=\"#FFA500\">Orange</option>\
      </select><br>\
      <label for=\"colorPriceVeryLow\" id=\"labelColorPriceVeryLow\">Color for Very Low Price:</label><br>\
      <select id=\"colorPriceVeryLow\" name=\"colorPriceVeryLow\">\
        <option class=\"white\" value=\"#FFFFFF\">White</option>\
        <option class=\"red\" value=\"#FF0000\">Red</option>\
        <option class=\"green\" value=\"#00FF00\">Green</option>\
        <option class=\"blue\" value=\"#0000FF\">Blue</option>\
        <option class=\"purple\" value=\"#800080\">Purple</option>\
        <option class=\"yellow\" value=\"#FFFF00\">Yellow</option>\
        <option class=\"orange\" value=\"#FFA500\">Orange</option>\
      </select><br><br>\
      <input type=\"submit\" value=\"Submit\" id=\"submitButton\">\
    </form>\
    <script>\
      const translations = {\
          en: {\
              pageTitle: 'WiFi Setup',\
              labelSSID: 'SSID:',\
              labelPassword: 'Password:',\
              labelRoom: 'Room Name:',\
              submitButton: 'Submit',\
              colorSettingsTitle: 'Color and Price Interval Settings',\
              labelPriceHigh: 'Price High (kr/kWh):',\
              labelColorPriceHigh: 'Color for High Price:',\
              labelPriceMedium: 'Price Medium (kr/kWh):',\
              labelColorPriceMedium: 'Color for Medium Price:',\
              labelPriceLow: 'Price Low (kr/kWh):',\
              labelColorPriceLow: 'Color for Low Price:',\
              labelColorPriceVeryLow: 'Color for Very Low Price (kr/kWh):'\
          },\
          da: {\
              pageTitle: 'WiFi Indstillinger',\
              labelSSID: 'SSID:',\
              labelPassword: 'Adgangskode:',\
              labelRoom: 'Rum Navn:',\
              submitButton: 'Indsend',\
              colorSettingsTitle: 'Farve- og Prisintervalindstillinger',\
              labelPriceHigh: 'Pris Høj (kr/kWh):',\
              labelColorPriceHigh: 'Farve for Høj Pris:',\
              labelPriceMedium: 'Pris Mellem (kr/kWh):',\
              labelColorPriceMedium: 'Farve for Mellem Pris:',\
              labelPriceLow: 'Pris Lav (kr/kWh):',\
              labelColorPriceLow: 'Farve for Lav Pris:',\
              labelColorPriceVeryLow: 'Farve for Meget Lav Pris (kr/kWh):'\
          }\
      };\
      function setLanguage(lang) {\
          document.getElementById('pageTitle').innerText = translations[lang].pageTitle;\
          document.getElementById('labelSSID').innerText = translations[lang].labelSSID;\
          document.getElementById('labelPassword').innerText = translations[lang].labelPassword;\
          document.getElementById('labelRoom').innerText = translations[lang].labelRoom;\
          document.getElementById('submitButton').value = translations[lang].submitButton;\
          document.getElementById('colorSettingsTitle').innerText = translations[lang].colorSettingsTitle;\
          document.getElementById('labelPriceHigh').innerText = translations[lang].labelPriceHigh;\
          document.getElementById('labelColorPriceHigh').innerText = translations[lang].labelColorPriceHigh;\
          document.getElementById('labelPriceMedium').innerText = translations[lang].labelPriceMedium;\
          document.getElementById('labelColorPriceMedium').innerText = translations[lang].labelColorPriceMedium;\
          document.getElementById('labelPriceLow').innerText = translations[lang].labelPriceLow;\
          document.getElementById('labelColorPriceLow').innerText = translations[lang].labelColorPriceLow;\
          document.getElementById('labelColorPriceVeryLow').innerText = translations[lang].labelColorPriceVeryLow;\
          document.cookie = \"lang=\" + lang + \"; path=/\";\
      }\
      window.onload = function() {\
          let lang = getCookie('lang') || 'da';\
          setLanguage(lang);\
      };\
      function getCookie(name) {\
          let dc = document.cookie;\
          let prefix = name + \"=\";\
          let begin = dc.indexOf(\"; \" + prefix);\
          if (begin == -1) {\
              begin = dc.indexOf(prefix);\
              if (begin != 0) return null;\
          } else {\
              begin += 2;\
          }\
          let end = document.cookie.indexOf(\";\", begin);\
          if (end == -1) {\
              end = dc.length;\
          }\
          return decodeURI(dc.substring(begin + prefix.length, end));\
      }\
    </script>\
    </body>\
    </html>";
    server.send(200, "text/html", html);
}
*/
/*
void WiFiSetupService::handleSetWiFi() {
    if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("roomName")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");
        String roomName = server.arg("roomName");

        ssid = urlDecode(ssid);
        password = urlDecode(password);
        roomName = urlDecode(roomName);

        configService.saveSSID(ssid);
        configService.savePassword(password);
        configService.saveRoomName(roomName);

        String response = "Data saved. Restarting device...";
        server.send(200, "text/html", response);

        delay(4000); // Wait so the user can see the restart message
        ESP.restart();
    } else {
        server.send(400, "text/html", "Invalid request");
    }
}*/
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

        server.send(200, "text/html", "Configuration saved! Device will reboot now.");
        delay(4000);
        ESP.restart(); // Restart the ESP32 to apply new settings
    }
    else
    {
        server.send(400, "text/html", "Missing required fields!");
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
