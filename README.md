
# Power Price Monitor

IoT device that monitors power prices and displays the current **price as a color of light** using an RGB LED on the ESP32. 

It connects to a **Philips Hue bridge** to control grouped lights based on the power price.

In essence every light you put in a predefined room, the color changes according to the price

### Color Indications
- **Red**: High power price
- **Blue**: Moderate power price
- **Green**: Low power price
- **White**: Extremely low price
- **Purple**: No WiFi connection
- **Blue/Red**: Flashing 250ms apart, press button on Hue Bridge
- **Blue/Red**: Flashing slowly, something is wrong, check if access point is setup correctly

This is my first time in years programming in c++, so be very careful about blindly copying my code. Especially in terms of good practices and security, I still have a lot to learn.

This version does not yet use HTTPS for the Hue, only for the call to the price portal (API). **Signify** (Former: Philips lighting) strongly recommends HTTPS. You can see the excellent documentation here:
https://developers.meethue.com/develop/application-design-guidance/using-https/

It does require a login, but you can register for free and it is worth it. The documentation is very to the point and easy to follow with great examples.


## Table of Contents
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [File Descriptions](#file-descriptions)
- [Setup Instructions](#setup-instructions)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Hardware Requirements
- ESP32 board, made for FireBeetle 2 ESP32-E IoT, but any other board should do. You need to change the neopixel RGB led pin from D8 to your relevant GPIO
- RGB LED (Neopixel, built in on the Firebeetle)
- Philips Hue bridge
- Push button (for resetting WiFi credentials, built in on GPIO27 on the Firebeetle)

## Software Requirements
- PlatformIO IDE (or you might change main.cpp to main.ino and get lucky with the Arduino IDE)
- ESP32 board library
- FastLED library
- ArduinoJson library
- ESP32mDNS
- Other basic libs...

## File Descriptions

### Main Files
- **PowerPriceLED.ino**: Main file that initializes the device, connects to WiFi, and starts the main loop.

### Helper Files
- **RGBControl.hpp/cpp**: Contains functions for setting up and controlling the RGB LED (neopixel).
- **TimeHandler.hpp/cpp**: Handles time synchronization with an NTP server.
- **ElprisenRESTReader.hpp/cpp**: Fetches the current power price from `elprisenligenu.dk`.
- **ConfigService.hpp/cpp**: Manages saving and loading WiFi credentials and API keys from non-volatile storage.
- **HueService.hpp/cpp**: Discovers and fetches IP address and port of the Philips Hue bridge.
- **HueLightService.hpp/cpp**: Controls the lights connected to the Philips Hue bridge.
- **HueEventService.hpp/cpp**: Handles events from the Philips Hue bridge. (Currently not integrated, but does work. Needs more work and integration with main.cpp not straightforward)
- **WiFiSetupService.hpp/cpp**: Sets up the device as a WiFi access point to capture WiFi credentials using `Captive Portal` 
- **WiFiUtils.hpp/cpp**: Simply connects to the Wifi, keeps trying forever

## Setup Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/jeppeml/power-price-monitor.git
   cd power-price-monitor
   ```

2. **Install Required Libraries**
   Open PlatformIO and make sure following libraries are installed (they should already be in the ini file):
   - FastLED
   - ArduinoJson
   - ESP32mDNS

3. **Upload**
   - Connect your ESP32 board to your computer
   - Select the appropriate board and port from the Tools menu
   - Upload the sketch to the ESP32

4. **Setup WiFi Credentials**
   - Upon first boot, the device will start as an access point named `PowerPriceMonitor`.
   - Connect to this access point using a phone or computer.
   - You will be redirected to a setup page. Enter your WiFi SSID, password, and the name of the Philips Hue room to control when price changes.
   - The device will save these credentials and restart to connect to the provided WiFi network.

## Usage

- The device fetches the current power price every minute and updates the color of the RGB LED based on the price.
- The Philips Hue lights in the specified room will also change color based on the power price.
- If the button connected to GPIO27 is pressed while booting, the device will reset the WiFi credentials and restart in access point mode.

## Contributing

Contributions are welcome! Please fork the repository and use a feature branch. Pull requests are warmly welcome.

## TODO
- WiFi setup should include setting up the price points for high, medium, low, very low
- WiFi setup should include setting up the colors for price points
- HTTPS should be enabled for the Hue bridge connection
- "Nice to have", would be instead of polling changes in the room, to listen for changes through events. I have a working service for this `HueEventService`, however it needs some work to properly filter events and be integrated with the main.cpp would require some work, because of the async nature of events. The max polling is 1 light change per second according to the documentation, and right now I set it up to 1 minute... so this shouldn't be a problem
