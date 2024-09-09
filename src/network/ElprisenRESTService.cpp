#include "ElprisenRESTService.hpp"
extern struct tm timeinfo;
extern int getCurrentHour();

ElprisenRESTService::ElprisenRESTService() : lastFetchDay(-1) { } // Initialize with a small buffer for prices

double ElprisenRESTService::getPriceForCurrentHour() {
    int hour = getCurrentHour();

    int currentDay = timeinfo.tm_mday;
    if (currentDay != lastFetchDay) {
        fetchDailyData();
        lastFetchDay = currentDay;
    }

    // Access the price for the current hour from the parsed JSON
    double price = prices[hour]["DKK_per_kWh"].as<double>();
    Serial.println("Price at " + String(hour) + " : " + String(price));
    return price;
}

void ElprisenRESTService::fetchDailyData() {
    String payload = fetchDataFromAPI();
    if (payload == "ERROR") {
        Serial.println("Failed to fetch data from API.");
        return;
    }

    // Parse the JSON response without predefining a size
    DeserializationError error = deserializeJson(prices, payload);

    if (error) {
        Serial.println("Failed to parse JSON");
        return;
    }
}

String ElprisenRESTService::fetchDataFromAPI() {
    String serverName = "https://www.elprisenligenu.dk/api/v1/prices/" + getRequestDatePartialURL() + "_DK1.json"; 
    String payload = "ERROR";

    if (WiFi.status() == WL_CONNECTED) {
        std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
        client->setInsecure(); // Skipping certificate verification

        HTTPClient https;
        if (https.begin(*client, serverName)) {
            int httpCode = https.GET();

            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK) {
                    payload = https.getString();
                }
            } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        } else {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
    } else {
        Serial.println("WiFi Disconnected");
    }
    return payload;
}

String ElprisenRESTService::getRequestDatePartialURL() {
    char timeYear[5];
    strftime(timeYear, 5, "%Y", &timeinfo);
    String year = String(timeYear);

    int dayNum = timeinfo.tm_mday;
    String day = String(dayNum);
    if (dayNum <= 9) {
        day = "0" + String(dayNum);
    }

    int monthNum = timeinfo.tm_mon + 1;
    String month = String(monthNum);
    if (monthNum <= 9) {
        month = "0" + String(monthNum);
    }
    
    String req_date = year + "/" + month + "-" + day;
    Serial.println("Current date: " + day + "/" + month + "-" + year + "   request date (for url): " + req_date);
    return req_date;
}