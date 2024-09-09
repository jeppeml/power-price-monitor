#include "WiFiSetupService.hpp"

WiFiSetupService::WiFiSetupService(ConfigService& configService)
    : server(80), configService(configService) {}

void WiFiSetupService::startAPAndResetWhenDone(){
    begin();
    startAccessPoint();
    loopAndProcess();
}

void WiFiSetupService::begin() {
    server.on("/", std::bind(&WiFiSetupService::handleRoot, this));
    server.on("/setwifi", HTTP_POST, std::bind(&WiFiSetupService::handleSetWiFi, this));
    server.onNotFound(std::bind(&WiFiSetupService::handleNotFound, this));
    server.begin();
}

void WiFiSetupService::loopAndProcess() {
    while(true){
        dnsServer.processNextRequest();
        server.handleClient();
    }
}

void WiFiSetupService::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access Point started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    begin();
}

void WiFiSetupService::handleRoot() {
    String html = "<html>\
    <head>\
    <meta charset=\"UTF-8\">\
    <title>JepLed Power Price Monitor</title>\
    </head>\
    <body>\
    <h1>WiFi Setup</h1>\
    <form action=\"/setwifi\" method=\"POST\">\
      <label for=\"ssid\">SSID:</label><br>\
      <input type=\"text\" id=\"ssid\" name=\"ssid\"><br>\
      <label for=\"password\">Password:</label><br>\
      <input type=\"password\" id=\"password\" name=\"password\"><br>\
      <label for=\"roomName\">Room Name:</label><br>\
      <input type=\"text\" id=\"roomName\" name=\"roomName\"><br><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    </body>\
    </html>";
    server.send(200, "text/html", html);
}

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
}

void WiFiSetupService::handleNotFound() {
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
}

// Function to decode URL-encoded strings, enables foreign characters like "ø"
String WiFiSetupService::urlDecode(const String &text) {
    String decoded = "";
    char temp[] = "0x00";
    unsigned int len = text.length();
    unsigned int i = 0;

    while (i < len) {
        char c = text.charAt(i);
        if (c == '+') {
            decoded += ' ';
        } else if (c == '%') {
            if (i + 2 < len) {
                temp[2] = text.charAt(i + 1);
                temp[3] = text.charAt(i + 2);
                decoded += strtol(temp, nullptr, 16);
                i += 2;
            }
        } else {
            decoded += c;
        }
        i++;
    }

    return decoded;
}
