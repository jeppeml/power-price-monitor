#ifndef ELPRISENRESTSERVICE_HPP
#define ELPRISENRESTSERVICE_HPP

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <time.h>

class ElprisenRESTService {
public:
    ElprisenRESTService();
    double getPriceForCurrentHour();
    void fetchDailyData();

private:
    JSONVar prices;
    int lastFetchDay;
    String getRequestDatePartialURL();
    String fetchDataFromAPI();
};

#endif // ELPRISENRESTSERVICE_HPP
