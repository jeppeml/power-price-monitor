/* External headers */
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
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
HueService *hueService;
HueLightService *hueLightService;
ConfigService *configService;
HueEventService *hueEventService;
WiFiSetupService *wifiSetupService;
ElprisenRESTService *elprisenRESTService;

/* Define colors and prices */
uint8_t red[3] = {255, 0, 0};
uint8_t blue[3] = {0, 0, 255};
uint8_t green[3] = {0, 255, 0};
uint8_t white[3] = {255, 255, 255};
uint8_t purple[3] = {156, 0, 156};
uint8_t yellow[3] = {255, 255, 0};
uint8_t black[3] = {0, 0, 0};

double priceHigh = 1.0;
double priceMedium = 0.5;
double priceLow = 0.0; // anything below this is colorPriceVeryLow

uint8_t *colorPriceHigh = red;
uint8_t *colorPriceMedium = yellow;
uint8_t *colorPriceLow = green;
uint8_t *colorPriceVeryLow = white;

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
unsigned long minutesBetweenLightUpdates = 1;
unsigned long timerDelayLights = minutesBetweenLightUpdates * 60 * 1000;

/* Provisioning setup, if bridge has not recognized the ESP32 */
unsigned long provisioningInterval = 5000;     // Check every 5 seconds for provisioning (Getting API key from the Hue through button press)
unsigned long blinkIntervalProvisioning = 250; // Blink every 250 ms if the button needs to be pressed on the Hue Bridge
unsigned long lastProvisionCheck = 0;

/* ID used for updating all lights in the room easily. From that fetch the light service on that. */
String currentGroupedLightID;

// Define the tariff time spans as constants (24-hour format)
const int LOW_TARIFF_START = 0; // 00:00
const int LOW_TARIFF_END = 6;   // 06:00

const int HIGH_TARIFF_START = 17; // 17:00
const int HIGH_TARIFF_END = 21;   // 21:00

// I can ignore these but setting them anyway for clarity
const int MEDIUM_TARIFF_MORNING_START = 6;  // 06:00
const int MEDIUM_TARIFF_MORNING_END = 17;   // 17:00
const int MEDIUM_TARIFF_EVENING_START = 21; // 21:00
const int MEDIUM_TARIFF_EVENING_END = 24;   // 00:00

const int SUMMER_START_MONTH = 4; // April
const int SUMMER_END_MONTH = 9;   // September

enum Tariff
{
  LowTariff,
  MedTariff,
  HighTariff
};

Tariff getCurrentTariffType()
{
  time_t now = time(nullptr); // Get current time
  struct tm *timeinfo = localtime(&now);
  int currentHour = timeinfo->tm_hour;

  if (currentHour >= LOW_TARIFF_START && currentHour < LOW_TARIFF_END)
  {
    return LowTariff;
  }
  else if (currentHour >= HIGH_TARIFF_START && currentHour < HIGH_TARIFF_END)
  {
    return HighTariff;
  }
  else if ((currentHour >= MEDIUM_TARIFF_MORNING_START && currentHour < MEDIUM_TARIFF_MORNING_END) ||
           (currentHour >= MEDIUM_TARIFF_EVENING_START && currentHour < MEDIUM_TARIFF_EVENING_END))
  {
    return MedTariff;
  }

  return HighTariff; // Should never reach this point, but just in case
}


bool isSummer()
{
  time_t now = time(nullptr); // Get current time
  struct tm *timeinfo = localtime(&now);
  int currentMonth = timeinfo->tm_mon + 1; // tm_mon is 0-based, so add 1

  // If current month is between April (4) and September (9), it's summer
  return currentMonth >= SUMMER_START_MONTH && currentMonth <= SUMMER_END_MONTH;
}


double getCurrentTariff()
{
  bool summer = isSummer();
  Tariff tariffType = getCurrentTariffType();

  if (summer)
  {
    if (tariffType == LowTariff)
    {
      return configService->loadSummerLow();
    }
    else if (tariffType == MedTariff)
    {
      return configService->loadSummerMedium();
    }
    else if (tariffType == HighTariff)
    {
      return configService->loadSummerHigh();
    }
  }
  else
  {
    if (tariffType == LowTariff)
    {
      return configService->loadWinterLow();
    }
    else if (tariffType == MedTariff)
    {
      return configService->loadWinterMedium();
    }
    else if (tariffType == HighTariff)
    {
      return configService->loadWinterHigh();
    }
  }

  return 99999; // Return ultra high price if none found
}

void setColorBasedOnPrice(double servicePrice)
{
  if (currentGroupedLightID == "")
    return;
  
  Serial.print("Current raw price : ");
  Serial.println(servicePrice);

  // Get the current tariff price based on season and time
  double tariffPrice = getCurrentTariff();
  Serial.print("Current tariff : ");
  Serial.println(tariffPrice);
  // Combine the service price with the tariff price
  double totalPrice = servicePrice + tariffPrice;

  Serial.print("Current total price : ");
  Serial.println(servicePrice);

  Serial.println("*** Current price setup ***");
  Serial.print("High price:\t");
  Serial.println(priceHigh);
  Serial.print("Medium price:\t");
  Serial.println(priceMedium);
  Serial.print("Low price:\t");
  Serial.println(priceLow);

  // Set the color based on the combined price
  if (totalPrice > priceHigh)
  {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceHigh);
    setNeopixelColorRGB(colorPriceHigh);
  }
  else if (totalPrice > priceMedium)
  {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceMedium);
    setNeopixelColorRGB(colorPriceMedium);
  }
  else if (totalPrice > priceLow)
  {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceLow);
    setNeopixelColorRGB(colorPriceLow);
  }
  else
  {
    hueLightService->lightControlRGB(hueService->getIP(), currentGroupedLightID, colorPriceVeryLow);
    setNeopixelColorRGB(colorPriceVeryLow);
  }
}

void provisioningBlink()
{
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkIntervalProvisioning)
  {
    if (blinkState)
    {
      setNeopixelColorRGB(red);
    }
    else
    {
      setNeopixelColorRGB(blue);
    }
    blinkState = !blinkState;
    lastBlinkTime = currentTime;
  }
}

void resetPreferences()
{
  Preferences preferences;
  preferences.begin("power-price", false);
  preferences.clear();
  preferences.end();
  ESP.restart();
}

void checkWiFiCredentials(String ssid, String password)
{
  wifiSetupService = new WiFiSetupService(*configService);
  if (ssid == "" || password == "")
  {
    Serial.println("No SSID and password, starting access point...");
    setNeopixelColorRGB(purple);
    wifiSetupService->startAPAndResetWhenDone(); // will loop forever and eventually restart the ESP
  }
  else
  {
    Serial.println("Trying to connect to WiFi with saved credentials...");
    setNeopixelColorRGB(purple);
    connectWiFi(ssid, password);
    setNeopixelColorRGB(black);
    Serial.println("Connected to WiFi.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
}

void checkForPreferencesResetButtonPressed()
{
  pinMode(USER_RESET_BTN, INPUT_PULLUP);

  if (!digitalRead(USER_RESET_BTN))
  { // pushbutton on other side of RST is pressed, maybe it is called EN, on FireBeetle it is called 27
    delay(4000);
    if (!digitalRead(USER_RESET_BTN))
    { // button is still down after 4 secs, a bit hacky ;)
      resetPreferences();
    }
  }
}

void setup()
{
  Serial.begin(115200);

  checkForPreferencesResetButtonPressed();

  setupLED();
  setNeopixelColorRGB(black);

  lastLightsCheck = millis() - timerDelayLights; // start immediately

  configService = new ConfigService();
  if (!configService->begin())
  {
    Serial.println("Failed to initialize ConfigService");
    return;
  }

  String ssid = configService->loadSSID();
  String password = configService->loadPassword();
  String room = configService->loadRoomName();

  // Load price intervals and color settings
  priceHigh = configService->loadPriceHigh();
  priceMedium = configService->loadPriceMedium();
  priceLow = configService->loadPriceLow();

  colorPriceHigh = configService->loadColorHigh();
  colorPriceMedium = configService->loadColorMedium();
  colorPriceLow = configService->loadColorLow();
  colorPriceVeryLow = configService->loadColorVeryLow();

  Serial.println("Loaded configuration:");
  Serial.println("SSID=\"" + ssid + "\", " + "password=" + password); // You should probably not print the password to Serial
  Serial.println("room=" + room);

  // Print Price values
  Serial.print("Price High: ");
  Serial.println(priceHigh);

  Serial.print("Price Medium: ");
  Serial.println(priceMedium);

  Serial.print("Price Low: ");
  Serial.println(priceLow);

  // Define an array of color names and corresponding color variables for looping
  const char *colorNames[] = {"Color Price High", "Color Price Medium", "Color Price Low", "Color Price Very Low"};
  uint8_t *colorValues[] = {colorPriceHigh, colorPriceMedium, colorPriceLow, colorPriceVeryLow};

  // Loop through each color and print its RGB values
  for (int i = 0; i < 4; i++)
  {
    Serial.print(colorNames[i]);
    Serial.print(": ");

    // Print RGB values in a loop
    for (int j = 0; j < 3; j++)
    { // Assuming RGB arrays have 3 components
      Serial.print(colorValues[i][j]);
      if (j < 2)
      {
        Serial.print(", ");
      }
    }
    Serial.println();
  }

  checkWiFiCredentials(ssid, password); // and present AP Portal if not there

  updateTime();

  hueService = new HueService();
  String hueIP = hueService->getIP();
  Serial.println(hueService->getIP());

  String apiKey = configService->loadAPIKey();

  hueLightService = new HueLightService();

  bool provisioned = false;
  if (apiKey != "")
  {
    hueLightService->setAPIKey(apiKey);
    provisioned = hueLightService->checkAPIKey(hueIP);
    if (provisioned)
    {
      Serial.println("Loaded API key from storage and it is valid");
    }
  }

  while (!provisioned)
  {
    provisioningBlink();

    unsigned long currentTime = millis();
    if (currentTime - lastProvisionCheck >= provisioningInterval)
    {
      provisioned = hueLightService->provisionDevice(hueIP);
      if (provisioned)
      {
        configService->saveAPIKey(hueLightService->getAPIKey());
        Serial.println("Provisioning successful, API key saved.");
      }
      lastProvisionCheck = currentTime;
    }
  }

  elprisenRESTService = new ElprisenRESTService();

  String roomID = hueLightService->getRoomID(hueIP, room);
  currentGroupedLightID = hueLightService->getGroupedLightID(hueIP, roomID);
  Serial.print("Grouped Light ID in room '");
  Serial.print(room);
  Serial.println("':");
  Serial.println(currentGroupedLightID);
}

void loop()
{
  if ((millis() - lastLightsCheck) > timerDelayLights)
  {
    // Extra check if WiFi is connected
    Serial.print("Wifi status=");
    Serial.println(String(WiFi.status()));

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi not connected, trying to connect...");
      setNeopixelColorRGB(purple);
      connectWiFi(configService->loadSSID(), configService->loadPassword());
    }

    // Update NTP time every 24 hours
    if ((millis() - lastNTPUpdateTime) > ntpUpdateInterval)
    {
      updateTime();
      lastNTPUpdateTime = millis();
    }

    double currentPrice = elprisenRESTService->getPriceForCurrentHour();

    if (hueService->getIP() != "" && hueService->getPort() != -1)
    {
      if (currentPrice > -99999)
      { // price is -99999 if error in query
        setColorBasedOnPrice(currentPrice);
      }
    }
    else
    {
      Serial.println("Hue service not found.");
    }

    lastLightsCheck = millis();
  }
}
