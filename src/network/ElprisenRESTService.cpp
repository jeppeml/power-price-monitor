#include "ElprisenRESTService.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

extern struct tm timeinfo;
extern int getCurrentHour();

// Root cert for elprisenligenu.dk
const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
                                "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                                "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
                                "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
                                "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
                                "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
                                "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
                                "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
                                "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
                                "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
                                "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
                                "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
                                "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
                                "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
                                "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
                                "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
                                "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
                                "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
                                "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
                                "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
                                "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
                                "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
                                "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
                                "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
                                "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
                                "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
                                "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
                                "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
                                "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
                                "-----END CERTIFICATE-----\n";

ElprisenRESTService::ElprisenRESTService() : lastFetchDay(-1) {}

double ElprisenRESTService::getPriceForCurrentHour()
{
    int hour = getCurrentHour();

    int currentDay = timeinfo.tm_mday;
    if (currentDay != lastFetchDay)
    {
        fetchDailyData();
        lastFetchDay = currentDay;
    }

    JSONVar priceAtHour = prices[hour]["DKK_per_kWh"];
    double price = double(priceAtHour);
    Serial.println("Price at " + String(hour) + " : " + String(price));
    return price;
}

void ElprisenRESTService::fetchDailyData()
{
    String payload = fetchDataFromAPI();
    if (payload == "ERROR")
    {
        Serial.println("Failed to fetch data from API.");
        return;
    }

    prices = JSON.parse(payload);
    if (JSON.typeof(prices) != "array")
    {
        Serial.println("Invalid JSON received");
        prices = JSONVar();
    }
}

String ElprisenRESTService::fetchDataFromAPI()
{
    String serverName = "https://www.elprisenligenu.dk/api/v1/prices/" + getRequestDatePartialURL() + "_DK1.json"; // DK1 is west Denmark, look at the API docs at elprisenligenu
    String payload = "ERROR";
    if (WiFi.status() == WL_CONNECTED)
    {
        std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
        client->setCACert(rootCACertificate);

        HTTPClient https;
        if (https.begin(*client, serverName))
        {
            int httpCode = https.GET();
            if (httpCode > 0)
            {
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    payload = https.getString();
                }
            }
            else
            {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        }
        else
        {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
    return payload;
}

String ElprisenRESTService::getRequestDatePartialURL()
{
    char timeYear[5];
    strftime(timeYear, 5, "%Y", &timeinfo);
    String year = String(timeYear);

    int dayNum = timeinfo.tm_mday;
    String day = String(dayNum);
    if (dayNum <= 9)
    {
        day = "0" + String(dayNum);
    }

    int monthNum = timeinfo.tm_mon + 1;
    String month = String(monthNum);
    if (monthNum <= 9)
    {
        month = "0" + String(monthNum);
    }
    
    // 2024/06-15_DK1.json
    String req_date = year + "/" + month + "-" + day;
    Serial.println("Current date: " + day + "/" + month + "-" + year + "   request date (for url): " + req_date);
    return req_date;
}
