#include <pgmspace.h>  // Required for PROGMEM

const char wifi_setup_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title id="pageTitle">Power Price Monitor</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
    }

    /* Flags in top-right corner */
    #language-selector {
      position: absolute;
      top: 10px;
      right: 10px;
    }

    #language-selector span {
      cursor: pointer;
      margin-left: 10px;
    }

    h1 {
      text-align: center;
    }

    form {
      margin: 20px auto;
      max-width: 400px;
    }

    /* Table layout for price and color */
    table {
      width: 100%;
      border-collapse: collapse;
      margin-bottom: 20px;
    }

    td, th {
      padding: 10px;
      text-align: center;
    }
/* The checkmark inside the color box */
input[type="radio"]:checked + .color-box::before {
  content: "✔"; /* Checkmark symbol */
  color: black;
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: 1em;
  z-index: 1;
  text-shadow: 
    -1px -1px 0 white, /* Top-left shadow */
     1px -1px 0 white, /* Top-right shadow */
    -1px  1px 0 white, /* Bottom-left shadow */
     1px  1px 0 white; /* Bottom-right shadow */
}
    th {
      background-color: #f4f4f4;
    }

    /* Slider styling */
    .price-slider {
      width: 150px;
    }

    .slider-value {
      font-size: 1.1em;
      margin-left: 10px;
      width: 50px; /* Adjusted width for DKK display */
      display: inline-block;
    }

    .slider-container {
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .color-select {
      width: 100px;
    }

    /* Submit button styling */
    .submit-button {
      width: 100%;
      padding: 10px;
      font-size: 1.2em;
      background-color: #4CAF50;
      color: white;
      border: none;
      cursor: pointer;
    }

    .submit-button:hover {
      background-color: #45a049;
    }

    /* Left align input boxes for tariffs */
    .tariff-input {
      text-align: left;
      width: 100%;
      padding: 5px;
    }
    
    .color-picker {
  display: flex;
  gap: 10px;
}

.form-control {
  display: inline-block;
  position: relative;
  width: 24px;  /* Adjust size for square */
  height: 24px; /* Adjust size for square */
  cursor: pointer;
}

input[type="radio"] {
  /* Completely hide the native radio button */
  display: none;
}
/* The checkmark inside the color box */
input[type="radio"]:checked + .color-box::before {
  content: "✔"; /* Checkmark symbol */
  color: black;
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: 1em;
  z-index: 1;
  text-shadow: 
    -1px -1px 0 rgba(255, 255, 255, 0.7), /* Top-left shadow */
     1px -1px 0 rgba(255, 255, 255, 0.7), /* Top-right shadow */
    -1px  1px 0 rgba(255, 255, 255, 0.7), /* Bottom-left shadow */
     1px  1px 0 rgba(255, 255, 255, 0.7); /* Bottom-right shadow */
}




/* Style for color box */
.color-box {
  display: inline-block;
  width: 100%;
  height: 100%;
  border: 2px solid black;
  box-sizing: border-box;
}

  </style>
</head>
<body>
  <div id="language-selector">
    <span onclick="setLanguage('da')">
      <svg width="30" height="20" viewBox="0 0 16 12" preserveAspectRatio="xMidYMid meet">
        <rect width="16" height="12" fill="#C60C30" />
        <rect x="5" width="2" height="12" fill="white" />
        <rect y="5" width="16" height="2" fill="white" />
      </svg>
    </span>
    <span onclick="setLanguage('en')">
      <svg width="30" height="20" viewBox="0 0 60 30" preserveAspectRatio="xMidYMid slice">
        <clipPath>
          <path d="M0,0 v30 h60 v-30 z" />
        </clipPath>
        <clipPath>
          <path d="M30,15 v-30 h-30 v60 h60 v-60 h-60 v60 z" />
        </clipPath>
        <g clip-path="url(#t)">
          <path d="M0,0 v30 h60 v-30 z" fill="#012169" />
          <path d="M0,0 l60,30 M60,0 l-60,30" stroke="#fff" stroke-width="6" />
          <path d="M0,0 l60,30 M60,0 l-60,30" clip-path="url(#s)" stroke="#C8102E" stroke-width="4" />
          <path d="M30,0 v30 M0,15 h60" stroke="#fff" stroke-width="10" />
          <path d="M30,0 v30 M0,15 h60" stroke="#C8102E" stroke-width="6" />
        </g>
      </svg>
    </span>
  </div>

  <h1 id="pageTitleHeader">Power Price Monitor</h1>
  <form action="/setwifi" method="POST">
    <label for="ssid" id="labelSSID">SSID</label><br />
    <input type="text" name="ssid" /><br />
    <label for="password" id="labelPassword">Password</label><br />
    <input type="password" name="password" /><br />
    <label for="roomName" id="labelRoom">Hue Room</label><br />
    <input type="text" name="roomName" /><br /><br />

    <h2 id="colorSettingsTitle">Colors and Prices (Price in DKK)</h2>

    <table>
        <thead>
          <tr>
            <th id="tableHeaderLevel">Level</th>
            <th id="tableHeaderPrice">Price</th>
            <th id="tableHeaderColor">Color</th>
          </tr>
        </thead>
        <tbody>
          <!-- High Level -->
          <tr>
            <td id="labelHigh">High</td>
            <td>
              <div class="slider-container">
                <span class="slider-value" id="highValue">1.00</span>
                <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceHigh" value="1.00" oninput="updateSliderValue('highValue', this.value)" />
              </div>
            </td>
            <td>
              <div class="color-picker" id="color-picker-high">
                <label class="form-control">
                  <input type="radio" id="red-high" name="colorPriceHigh" value="#FF0000" checked> <!-- Default red for High -->
                  <span class="color-box" style="background-color: #FF0000;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="green-high" name="colorPriceHigh" value="#00FF00">
                  <span class="color-box" style="background-color: #00FF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="blue-high" name="colorPriceHigh" value="#0000FF">
                  <span class="color-box" style="background-color: #0000FF;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="purple-high" name="colorPriceHigh" value="#800080">
                  <span class="color-box" style="background-color: #800080;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="yellow-high" name="colorPriceHigh" value="#FFFF00">
                  <span class="color-box" style="background-color: #FFFF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="orange-high" name="colorPriceHigh" value="#FFA500">
                  <span class="color-box" style="background-color: #FFA500;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="white-high" name="colorPriceHigh" value="#FFFFFF">
                  <span class="color-box" style="background-color: #FFFFFF;"></span>
                </label>
              </div>
            </td>
          </tr>
      
          <!-- Medium Level -->
          <tr>
            <td id="labelMedium">Medium</td>
            <td>
              <div class="slider-container">
                <span class="slider-value" id="mediumValue">0.50</span>
                <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceMedium" value="0.50" oninput="updateSliderValue('mediumValue', this.value)" />
              </div>
            </td>
            <td>
              <div class="color-picker" id="color-picker-medium">
                <label class="form-control">
                  <input type="radio" id="red-medium" name="colorPriceMedium" value="#FF0000">
                  <span class="color-box" style="background-color: #FF0000;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="green-medium" name="colorPriceMedium" value="#00FF00">
                  <span class="color-box" style="background-color: #00FF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="blue-medium" name="colorPriceMedium" value="#0000FF">
                  <span class="color-box" style="background-color: #0000FF;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="purple-medium" name="colorPriceMedium" value="#800080">
                  <span class="color-box" style="background-color: #800080;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="yellow-medium" name="colorPriceMedium" value="#FFFF00" checked> <!-- Default yellow for Medium -->
                  <span class="color-box" style="background-color: #FFFF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="orange-medium" name="colorPriceMedium" value="#FFA500">
                  <span class="color-box" style="background-color: #FFA500;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="white-medium" name="colorPriceMedium" value="#FFFFFF">
                  <span class="color-box" style="background-color: #FFFFFF;"></span>
                </label>
              </div>
            </td>
          </tr>
      
          <!-- Low Level -->
          <tr>
            <td id="labelLow">Low</td>
            <td>
              <div class="slider-container">
                <span class="slider-value" id="lowValue">0.00</span>
                <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceLow" value="0.00" oninput="updateSliderValue('lowValue', this.value)" />
              </div>
            </td>
            <td>
              <div class="color-picker" id="color-picker-low">
                <label class="form-control">
                  <input type="radio" id="red-low" name="colorPriceLow" value="#FF0000">
                  <span class="color-box" style="background-color: #FF0000;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="green-low" name="colorPriceLow" value="#00FF00" checked> <!-- Default green for Low -->
                  <span class="color-box" style="background-color: #00FF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="blue-low" name="colorPriceLow" value="#0000FF">
                  <span class="color-box" style="background-color: #0000FF;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="purple-low" name="colorPriceLow" value="#800080">
                  <span class="color-box" style="background-color: #800080;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="yellow-low" name="colorPriceLow" value="#FFFF00">
                  <span class="color-box" style="background-color: #FFFF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="orange-low" name="colorPriceLow" value="#FFA500">
                  <span class="color-box" style="background-color: #FFA500;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="white-low" name="colorPriceLow" value="#FFFFFF">
                  <span class="color-box" style="background-color: #FFFFFF;"></span>
                </label>
              </div>
            </td>
          </tr>
      
          <!-- Very Low Level -->
          <tr>
            <td id="labelVeryLow">Very Low</td>
            <td>-</td>
            <td>
              <div class="color-picker" id="color-picker-very-low">
                <label class="form-control">
                  <input type="radio" id="red-very-low" name="colorPriceVeryLow" value="#FF0000">
                  <span class="color-box" style="background-color: #FF0000;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="green-very-low" name="colorPriceVeryLow" value="#00FF00">
                  <span class="color-box" style="background-color: #00FF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="blue-very-low" name="colorPriceVeryLow" value="#0000FF">
                  <span class="color-box" style="background-color: #0000FF;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="purple-very-low" name="colorPriceVeryLow" value="#800080">
                  <span class="color-box" style="background-color: #800080;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="yellow-very-low" name="colorPriceVeryLow" value="#FFFF00">
                  <span class="color-box" style="background-color: #FFFF00;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="orange-very-low" name="colorPriceVeryLow" value="#FFA500">
                  <span class="color-box" style="background-color: #FFA500;"></span>
                </label>
                <label class="form-control">
                  <input type="radio" id="white-very-low" name="colorPriceVeryLow" value="#FFFFFF" checked> <!-- Default white for Very Low -->
                  <span class="color-box" style="background-color: #FFFFFF;"></span>
                </label>
              </div>
            </td>
          </tr>
        </tbody>
      </table>
      
      
    

    <!-- Tariff section starts here -->
    <h2 id="tariffSectionTitle">Network Tariffs (Price in DKK)</h2>

    <table>
      <thead>
        <tr>
          <th>Season</th>
          <th>Tariff</th>
          <th>Time Range</th>
          <th>Price</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td rowspan="3">Summer</td>
          <td>Low</td>
          <td>00:00-06:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="summerLowValue">0.14</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="summerLow" value="0.14" oninput="updateSliderValue('summerLowValue', this.value)" />
            </div>
          </td>
        </tr>
        <tr>
          <td>Medium</td>
          <td>06:00-17:00, 21:00-00:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="summerMediumValue">0.21</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="summerMedium" value="0.21" oninput="updateSliderValue('summerMediumValue', this.value)" />
            </div>
          </td>
        </tr>
        <tr>
          <td>High</td>
          <td>17:00-21:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="summerHighValue">0.54</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="summerHigh" value="0.54" oninput="updateSliderValue('summerHighValue', this.value)" />
            </div>
          </td>
        </tr>
        <tr>
          <td rowspan="3">Winter</td>
          <td>Low</td>
          <td>00:00-06:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="winterLowValue">0.14</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="winterLow" value="0.14" oninput="updateSliderValue('winterLowValue', this.value)" />
            </div>
          </td>
        </tr>
        <tr>
          <td>Medium</td>
          <td>06:00-17:00, 21:00-00:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="winterMediumValue">0.41</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="winterMedium" value="0.41" oninput="updateSliderValue('winterMediumValue', this.value)" />
            </div>
          </td>
        </tr>
        <tr>
          <td>High</td>
          <td>17:00-21:00</td>
          <td>
            <div class="slider-container">
              <span class="slider-value" id="winterHighValue">1.24</span>
              <input type="range" min="0" max="3.5" step="0.01" class="price-slider" name="winterHigh" value="1.24" oninput="updateSliderValue('winterHighValue', this.value)" />
            </div>
          </td>
        </tr>
      </tbody>
    </table>

    <input type="submit" class="submit-button" id="submitButton" value="Save and restart device" />
  </form>

  <script>
    // Update the displayed value next to the slider
    function updateSliderValue(id, value) {
      document.getElementById(id).innerText = parseFloat(value).toFixed(2);
    }

    // Automatically handle Danish comma decimal
    document.querySelector('form').addEventListener('submit', function (e) {
      const inputs = document.querySelectorAll('input[type="text"]');
      inputs.forEach(input => {
        if (input.value.includes(',')) {
          input.value = input.value.replace(',', '.');  // Replace comma with period
        }
      });
    });

    // Translation logic for Danish and English color names
    const translations = {
      en: {
        pageTitle: 'Power Price Monitor',
        pageTitleHeader: 'Power Price Monitor',
        labelSSID: 'SSID',
        labelPassword: 'Password',
        labelRoom: 'Hue Room',
        submitButton: 'Save and restart device',
        colorSettingsTitle: 'Colors and Prices (Price in DKK)',
        tariffSectionTitle: 'Network Tariffs (Price in DKK)',
        summerTariffTitle: 'Summer Tariffs',
        winterTariffTitle: 'Winter Tariffs',
        labelSummerLow: 'Low (00:00-06:00):',
        labelSummerMedium: 'Medium (06:00-17:00, 21:00-00:00):',
        labelSummerHigh: 'High (17:00-21:00):',
        labelWinterLow: 'Low (00:00-06:00):',
        labelWinterMedium: 'Medium (06:00-17:00, 21:00-00:00):',
        labelWinterHigh: 'High (17:00-21:00):',
      },
      da: {
        pageTitle: 'Strømpris Overvågning',
        pageTitleHeader: 'Strømpris Overvågning',
        labelSSID: 'SSID',
        labelPassword: 'Adgangskode',
        labelRoom: 'Hue Rum',
        submitButton: 'Gem og genstart enheden',
        colorSettingsTitle: 'Farver og Priser (Pris i DKK)',
        tariffSectionTitle: 'Netværkstariffer (Pris i DKK)',
        summerTariffTitle: 'Sommertariffer',
        winterTariffTitle: 'Vintertariffer',
        labelSummerLow: 'Lav (00:00-06:00):',
        labelSummerMedium: 'Mellem (06:00-17:00, 21:00-00:00):',
        labelSummerHigh: 'Høj (17:00-21:00):',
        labelWinterLow: 'Lav (00:00-06:00):',
        labelWinterMedium: 'Mellem (06:00-17:00, 21:00-00:00):',
        labelWinterHigh: 'Høj (17:00-21:00):',
      }
    };

    function setLanguage(lang) {
      const translation = translations[lang];
      document.getElementById('pageTitle').innerText = translation.pageTitle;
      document.getElementById('pageTitleHeader').innerText = translation.pageTitleHeader;
      document.getElementById('labelSSID').innerText = translation.labelSSID;
      document.getElementById('labelPassword').innerText = translation.labelPassword;
      document.getElementById('labelRoom').innerText = translation.labelRoom;
      document.getElementById('submitButton').value = translation.submitButton;
      document.getElementById('colorSettingsTitle').innerText = translation.colorSettingsTitle;
      document.getElementById('tariffSectionTitle').innerText = translation.tariffSectionTitle;

      // Update tariff labels
      document.getElementById('labelSummerLow').innerText = translation.labelSummerLow;
      document.getElementById('labelSummerMedium').innerText = translation.labelSummerMedium;
      document.getElementById('labelSummerHigh').innerText = translation.labelSummerHigh;
      document.getElementById('labelWinterLow').innerText = translation.labelWinterLow;
      document.getElementById('labelWinterMedium').innerText = translation.labelWinterMedium;
      document.getElementById('labelWinterHigh').innerText = translation.labelWinterHigh;
    }

  </script>
</body>
</html>


)rawliteral";