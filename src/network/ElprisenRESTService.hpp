#ifndef ELPRISENRESTSERVICE_HPP
#define ELPRISENRESTSERVICE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

class ElprisenRESTService {
public:
    ElprisenRESTService();
    double getPriceForCurrentHour();
    void fetchDailyData();

private:
    JsonDocument prices;   
    int lastFetchDay;
    String getRequestDatePartialURL();
    String fetchDataFromAPI();
};

#endif // ELPRISENRESTSERVICE_HPP