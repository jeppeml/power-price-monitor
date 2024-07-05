/* External headers */
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <LittleFS.h>
#include <vector>

/* Headers that are part of this project */
#include "hue/HueService.hpp"
#include "hue/HueLightService.hpp"
#include "hue/HueEventService.hpp"
#include "network/WiFiSetupService.hpp"
#include "network/ElprisenRESTService.hpp"
#include "hardware/RGBControl.hpp"
#include "utils/TimeHandler.hpp"
#include "utils/WiFiUtils.hpp"
#include "utils/ConfigService.hpp"

/* Declare all services used in the program, initialized on setup and reused throughout */
HueService* hueService;
HueLightService* hueLightService;
ConfigService* configService;
HueEventService* hueEventService;
WiFiSetupService* wifiSetupService;
ElprisenRESTService* elprisenRESTService;

/* Define colors and prices */
const uint8_t red[3] = {255,0,0};
const uint8_t blue[3] = {0,0,255};
const uint8_t green[3] = {0,255,0};
const uint8_t white[3] = {255,255,255};
const uint8_t purple[3] = {156,0,156};

// TODO: Would like this to be setup from WiFiSetupService and saved to Preferences through the ConfigService
//       Colors would be nice to define with a color picker on the WiFi setup for each color, at least for the prices.
const double priceHigh = 1.7;
const double priceMedium = 0.5;
const double priceLow = 0.05; // anything below this is colorPriceVeryLow

const uint8_t* colorPriceHigh = red;
const uint8_t* colorPriceMedium = blue;
const uint8_t* colorPriceLow = green;
const uint8_t* colorPriceVeryLow = white;

/* Shared timeinfo */
struct tm timeinfo;

/* Hardware */
const int USER_RESET_BTN = 27;

/* Blinking for missing WiFi */
unsigned long lastBlinkTime = 0;
bool blinkState = false;

/* NTP Server timer */
unsigned long lastNTPUpdateTime = 0;
unsigned long ntpUpdateInterval = 24 * 60 * 60 * 1000; // 24 hours in milliseconds for updating the time from NTP

/* Setup the main update time for the lights. How often should it check the bridge for new lights in the room */
unsigned long lastLightsCheck = 0;
unsigned long minutesBetweenLightUpdates = 2;  // Update every 2 minutes
unsigned long timerDelayLights = minutesBetweenLightUpdates * 60 * 1000;

/* Provisioning setup, if bridge has not recognized the ESP32 */
unsigned long provisioningInterval = 5000;  // Check every 5 seconds for provisioning (Getting API key from the Hue through button press)
unsigned long blinkIntervalProvisioning = 250;          // Blink every 250 ms if the button needs to be pressed on the Hue Bridge
bool provisioned = false;
unsigned long lastProvisionCheck = 0;

/* Temp name for the room, will try to load this from config file, part of initial setup */
String roomName = "Strøm er dyrt";  // Room name to monitor, this is the default name, must be set when setting up WiFi

/* ID used for updating all lights in the room easily. From that fetch the light service on that. */
String currentGroupedLightID; 

void setColorBasedOnPrice(double price) {
  if (currentGroupedLightID == "") return;

  if (price > priceHigh) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceHigh);  
    setNeopixelColorRGB(colorPriceHigh);
  } else if (price > priceMedium) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceMedium); 
    setNeopixelColorRGB(colorPriceMedium);
  } else if (price > priceLow) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceLow);  
    setNeopixelColorRGB(colorPriceLow);
  } else {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceVeryLow);
    setNeopixelColorRGB(colorPriceVeryLow);
  }
}

void provisioningBlink() {
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkIntervalProvisioning) {
    if (blinkState) {
      setNeopixelColorRGB(red);  
    } else {
      setNeopixelColorRGB(blue);  
    }
    blinkState = !blinkState;
    lastBlinkTime = currentTime;
  }
}

void resetPreferences() {
  Preferences preferences;
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  ESP.restart();
}

void setup() {
  setupLED();
  Serial.begin(115200);
  pinMode(USER_RESET_BTN, INPUT_PULLUP);
  if (!digitalRead(USER_RESET_BTN)) { // pushbutton on other side of RST is pressed, maybe it is called EN
    delay(4000);
    if (!digitalRead(USER_RESET_BTN)) {  // button is still down after 4 secs, a bit hacky ;)
      resetPreferences();
    }
  }
  
  lastLightsCheck = millis() - timerDelayLights;  // start immediately

  configService = new ConfigService();
  if (!configService->begin()) {
    Serial.println("Failed to initialize ConfigService");
    return;
  }
  Serial.println("Started ConfigService");

  String ssid = configService->loadSSID();
  String password = configService->loadPassword();
  String room = configService->loadRoomName();

  Serial.println("SSID status=" + ssid);
  Serial.println("password status=" + password);
  Serial.println("room=" + room);
  if (room != "") {
    roomName = room;
  }

  wifiSetupService = new WiFiSetupService(*configService);
  if (ssid == "" || password == "") {
    Serial.println("No SSID and password, starting access point...");
    wifiSetupService->startAccessPoint();
  } else {
    Serial.println("Trying to connect to WiFi with saved credentials...");
    connectWiFi(ssid.c_str(), password.c_str());
  }
  Serial.print("Wifi status=");
  Serial.println(String(WiFi.status()));
  while (WiFi.status() != WL_CONNECTED) {
    connectWiFi(ssid.c_str(), password.c_str());
  }

  Serial.println("Connected to WiFi.");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  updateTime();

  hueLightService = new HueLightService();
  hueService = new HueService();
  String hueIP = hueService->getIP();
  Serial.println(hueService->getIP());

  String apiKey = configService->loadAPIKey();
  if (apiKey != "") {
    hueLightService->setAPIKey(apiKey);
    provisioned = hueLightService->checkAPIKey(hueIP);
    if (provisioned) {
      Serial.println("Loaded API key from storage and it is valid");
    }
  }

  if (!provisioned) {
    while (!provisioned) {
      provisioningBlink();

      unsigned long currentTime = millis();
      if (currentTime - lastProvisionCheck >= provisioningInterval) {
        provisioned = hueLightService->provisionDevice(hueIP);
        if (provisioned) {
          configService->saveAPIKey(hueLightService->getAPIKey());
          Serial.println("Provisioning successful, API key saved.");
        }
        lastProvisionCheck = currentTime;
      }
    }
  }

  hueEventService = new HueEventService(hueIP, hueLightService->getAPIKey());
  hueEventService->begin();

  elprisenRESTService = new ElprisenRESTService();

  String roomID = hueLightService->getRoomID(hueIP, roomName);
  currentGroupedLightID = hueLightService->getGroupedLightID(hueIP, roomID);
  Serial.print("Grouped Light ID in room '");
  Serial.print(roomName);
  Serial.println("':");
  Serial.println(currentGroupedLightID);
}

void loop() {
  if ((millis() - lastLightsCheck) > timerDelayLights) {
    // Extra check if WiFi is connected
    Serial.print("Wifi status=");
    Serial.println(String(WiFi.status()));

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected, trying to connect...");
      setNeopixelColorRGB(purple); 
      connectWiFi(configService->loadSSID().c_str(), configService->loadPassword().c_str());
    }

    // Update NTP time every 24 hours
    if ((millis() - lastNTPUpdateTime) > ntpUpdateInterval) {
      updateTime();
      lastNTPUpdateTime = millis();
    }

    double currentPrice = elprisenRESTService->getPriceForCurrentHour();

    if (hueService->getIP() != "" && hueService->getPort() != -1) {
      if (currentPrice > -99999) {  // price is -99999 if error in query
        setColorBasedOnPrice(currentPrice);
      }
    } else {
      Serial.println("Hue service not found.");
    }

    lastLightsCheck = millis();
  }
}
