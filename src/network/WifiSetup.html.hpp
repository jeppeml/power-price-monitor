#include <pgmspace.h>  // Required for PROGMEM

const char wifi_setup_html[] PROGMEM = R"rawliteral(
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
      width: 50px;
      display: inline-block;
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

    <h2 id="colorSettingsTitle">Colors and Prices</h2>

    <table>
      <thead>
        <tr>
          <th id="tableHeaderLevel">Level</th>
          <th id="tableHeaderPrice">Price</th>
          <th id="tableHeaderColor">Color</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td id="labelHigh">High</td>
          <td>
            <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceHigh" value="1.0" oninput="updateSliderValue('highValue', this.value)" />
            <span class="slider-value" id="highValue">1.0</span>
          </td>
          <td>
            <select class="color-select" name="colorPriceHigh">
              <option value="#FF0000" selected>Red</option> <!-- Default Red -->
              <option value="#00FF00">Green</option>
              <option value="#0000FF">Blue</option>
              <option value="#800080">Purple</option>
              <option value="#FFFF00">Yellow</option>
              <option value="#FFA500">Orange</option>
            </select>
          </td>
        </tr>
        <tr>
          <td id="labelMedium">Medium</td>
          <td>
            <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceMedium" value="0.5" oninput="updateSliderValue('mediumValue', this.value)" />
            <span class="slider-value" id="mediumValue">0.5</span>
          </td>
          <td>
            <select class="color-select" name="colorPriceMedium">
              <option value="#FFFF00" selected>Yellow</option> <!-- Default Yellow -->
              <option value="#FF0000">Red</option>
              <option value="#00FF00">Green</option>
              <option value="#0000FF">Blue</option>
              <option value="#800080">Purple</option>
              <option value="#FFA500">Orange</option>
            </select>
          </td>
        </tr>
        <tr>
          <td id="labelLow">Low</td>
          <td>
            <input type="range" min="-2" max="5" step="0.1" class="price-slider" name="priceLow" value="0.0" oninput="updateSliderValue('lowValue', this.value)" />
            <span class="slider-value" id="lowValue">0.0</span>
          </td>
          <td>
            <select class="color-select" name="colorPriceLow">
              <option value="#00FF00" selected>Green</option> <!-- Default Green -->
              <option value="#FF0000">Red</option>
              <option value="#0000FF">Blue</option>
              <option value="#800080">Purple</option>
              <option value="#FFFF00">Yellow</option>
              <option value="#FFA500">Orange</option>
            </select>
          </td>
        </tr>
        <tr>
          <td id="labelVeryLow">Very Low</td>
          <td>-</td>
          <td>
            <select class="color-select" name="colorPriceVeryLow">
              <option value="#FFFFFF" selected>White</option> <!-- Default White -->
              <option value="#FF0000">Red</option>
              <option value="#00FF00">Green</option>
              <option value="#0000FF">Blue</option>
              <option value="#800080">Purple</option>
              <option value="#FFFF00">Yellow</option>
              <option value="#FFA500">Orange</option>
            </select>
          </td>
        </tr>
      </tbody>
    </table>

    <input type="submit" class="submit-button" id="submitButton" value="Save and restart device" />
  </form>

  <script>
    // Update the displayed value next to the slider
    function updateSliderValue(id, value) {
      document.getElementById(id).innerText = value;
    }
    
    // Translation logic for Danish and English color names
    const translations = {
      en: {
        pageTitle: 'Power Price Monitor',
        pageTitleHeader: 'Power Price Monitor',
        labelSSID: 'SSID',
        labelPassword: 'Password',
        labelRoom: 'Hue Room',
        submitButton: 'Save and restart device',
        colorSettingsTitle: 'Colors and Prices',
        labelHigh: 'High',
        labelMedium: 'Medium',
        labelLow: 'Low',
        labelVeryLow: 'Very Low',
        tableHeaderLevel: 'Level',
        tableHeaderPrice: 'Price',
        tableHeaderColor: 'Color',
        colors: {
          red: 'Red',
          green: 'Green',
          blue: 'Blue',
          purple: 'Purple',
          yellow: 'Yellow',
          orange: 'Orange',
          white: 'White'
        }
      },
      da: {
        pageTitle: 'Strømpris Overvågning',
        pageTitleHeader: 'Strømpris Overvågning',
        labelSSID: 'SSID',
        labelPassword: 'Adgangskode',
        labelRoom: 'Hue Rum',
        submitButton: 'Gem og genstart enheden',
        colorSettingsTitle: 'Farver og Priser',
        labelHigh: 'Høj',
        labelMedium: 'Mellem',
        labelLow: 'Lav',
        labelVeryLow: 'Meget Lav',
        tableHeaderLevel: 'Niveau',
        tableHeaderPrice: 'Pris',
        tableHeaderColor: 'Farve',
        colors: {
          red: 'Rød',
          green: 'Grøn',
          blue: 'Blå',
          purple: 'Lilla',
          yellow: 'Gul',
          orange: 'Orange',
          white: 'Hvid'
        }
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
      document.getElementById('labelHigh').innerText = translation.labelHigh;
      document.getElementById('labelMedium').innerText = translation.labelMedium;
      document.getElementById('labelLow').innerText = translation.labelLow;
      document.getElementById('labelVeryLow').innerText = translation.labelVeryLow;
      document.getElementById('tableHeaderLevel').innerText = translation.tableHeaderLevel;
      document.getElementById('tableHeaderPrice').innerText = translation.tableHeaderPrice;
      document.getElementById('tableHeaderColor').innerText = translation.tableHeaderColor;

      // Update color names in dropdowns
      const options = document.querySelectorAll('select.color-select option');
      options.forEach(option => {
        const colorValue = option.value;
        if (colorValue === '#FF0000') option.text = translation.colors.red;
        else if (colorValue === '#00FF00') option.text = translation.colors.green;
        else if (colorValue === '#0000FF') option.text = translation.colors.blue;
        else if (colorValue === '#800080') option.text = translation.colors.purple;
        else if (colorValue === '#FFFF00') option.text = translation.colors.yellow;
        else if (colorValue === '#FFA500') option.text = translation.colors.orange;
        else if (colorValue === '#FFFFFF') option.text = translation.colors.white;
      });
    }
  </script>
</body>
</html>

)rawliteral";