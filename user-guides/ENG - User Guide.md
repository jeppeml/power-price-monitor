# User Guide for Hue Power Monitoring

## First Setup

### Setting up Wi-Fi and selecting a room
1. Turn on the device. The LED will be **solid purple** when it is ready.
2. Connect your phone or computer to the Wi-Fi network named `PowerPriceMonitor`. The browser should open automatically.
3. Enter your Wi-Fi details and choose a **Hue room name** (this room will follow the electricity prices).
4. In the browser, you can configure both **color settings** and **price intervals**:
   - Default colors:
     - **Red**: High electricity price
     - **Yellow**: Medium electricity price
     - **Green**: Low electricity price
     - **White**: Very low electricity price
   - Default price intervals:
     - Above 1.0 DKK/kWh: High electricity price
     - 0.65 - 1.0 DKK/kWh: Medium electricity price
     - 0.2 - 0.65 DKK/kWh: Low electricity price
     - Below 0.2 DKK/kWh: Very low electricity price
5. The device will restart and attempt to connect to your Wi-Fi.

### Connecting to the Hue Bridge
1. The LED will blink **red and blue alternately**.
2. Press the **button on your Hue Bridge** within 30 seconds to connect the device.
3. When the connection is complete, the LED will stop blinking, and the device will start controlling the lights based on the electricity prices.

---

## Resetting the Device

1. To reset the Wi-Fi settings or other configurations, press and hold the **Button** for at least **4 seconds** while the device is starting up.
2. The device will restart to be configured again.

---

## LED Indicators

- **Purple (solid)**: The device is ready to be connected to Wi-Fi.
- **Red and blue (blinking alternately)**: Waiting for approval from the Hue Bridge (press the button on the hue bridge).
- **Other colors**: The device is showing electricity prices using the selected colors (e.g. Red, Yellow, Green, White).
