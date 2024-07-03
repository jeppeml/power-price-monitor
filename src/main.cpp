#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <LittleFS.h>
#include <vector>

#include "utils/ConfigService.hpp"
#include "hue/HueService.hpp"
#include "hue/HueLightService.hpp"
#include "hue/HueEventService.hpp"
#include "network/WiFiSetupService.hpp"
#include "network/ElprisenRESTService.hpp"
#include "hardware/RGBControl.hpp"
#include "utils/TimeHandler.hpp"
#include "utils/WiFiUtils.hpp"

unsigned long lastTime = 0;
unsigned long minutesBetweenUpdates = 2;  // Update every 2 minutes
unsigned long timerDelay = minutesBetweenUpdates * 60 * 1000;
unsigned long provisioningInterval = 5000;  // Check every 5 seconds for provisioning (Getting API key from the Hue through button press)
unsigned long blinkInterval = 250;          // Blink every 250 ms if the button needs to be pressed on the Hue Bridge

String roomName = "Strøm er dyrt";  // Room name to monitor, this is the default name, must be set when setting up WiFi

struct tm timeinfo;

HueService* hueService;
HueLightService* hueLightService;
ConfigService* configService;
HueEventService* hueEventService;
WiFiSetupService* wifiSetupService;
ElprisenRESTService* elprisenRESTService;

double redPriceMin = 1.7;
double bluePriceMin = 0.5;
double greenPriceMin = 0.05;

bool provisioned = false;
unsigned long lastProvisionCheck = 0;
unsigned long lastBlinkTime = 0;
bool blinkState = false;
unsigned long lastNTPUpdateTime = 0;
unsigned long ntpUpdateInterval = 24 * 60 * 60 * 1000; // 24 hours in milliseconds


const int USER_RESET_BTN = 27;

String currentGroupedLightID;

void setColorBasedOnPrice(double price) {
  if (currentGroupedLightID == "") return;

  if (price > redPriceMin) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, 255, 0, 0);  // Red
    setRGBColor(255, 0, 0);
  } else if (price > bluePriceMin) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, 0, 0, 255);  // Blue
    setRGBColor(0, 0, 255);
  } else if (price > greenPriceMin) {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, 0, 255, 0);  // Green
    setRGBColor(0, 255, 0);
  } else {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, 255, 255, 255);  // White
    setRGBColor(255, 255, 255);
  }
}

void provisioningBlink() {
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkInterval) {
    if (blinkState) {
      setRGBColor(255, 0, 0);  // Red
    } else {
      setRGBColor(0, 0, 255);  // Blue
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
  
  //while (!Serial) {}
  //delay(800); // only for debugging without reset
  lastTime = millis() - timerDelay;  // start immediately

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
    wifiSetupService->loop();
    delay(100);  // small delay to prevent tight loop
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
  if ((millis() - lastTime) > timerDelay) {
    // Extra check if WiFi is connected
    Serial.print("Wifi status=");
    Serial.println(String(WiFi.status()));

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected, trying to connect...");
      setRGBColor(153, 0, 153);
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

    lastTime = millis();
  }
}
