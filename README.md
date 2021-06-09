# Internet of Bananas
## Code

The **Internet of Bananas** is based on the NodeMCU Esp8266, color sensor TCS3200, temperature and humidity sensor DHT11. See the [Material List](https://github.com/Internet-of-Bananas/material-list) for a full description of the components.

The code for the project is in the folder **6_iob**. The other sketches are exercices and test of the sensors.


## 1. Update to fix the color conversion
There is an issue in the code in the color conversion from decimal to hexadecimal. The hexadecimal color has to have the # symbol plus six digits, otherwise the color will be displayed black. The problem happens when the **decimal** value is less than 16, resulting in a **hexadecimal** value with less than six digits. 

For example, the decimal RGB color `201, 94, 13` should be converted as `c9 5e 0d` in the hexadecimal format, but in the IoB code it's converted as `c9 5e d`, the "0" is omitted in the conversion, thus the final number has only 5 digits. To guarantee that the hexadecimal color has always 6 digits, we created an `if` statement that will verify the number and do the correction when it's necessary.

To fix this bug, in the Arduino IDE open the file `6_iob` (if you saved it with another name, open it. Just to remind you, this file should already have your wifi network and password, Adafruit IO user name and key, and the color calibration p ameters). Locate the following code shown bellow:

``` 
  /*
    In order to visualize the color in the Adafrui. Latform
    it's necessary to convert the color decimal number to the hexadecimal format.
    (The decimal is 0 to 9, and hexadecimal is 0 to F)
    For that, first we create a string of variables for each RGB values and convert them as hexadecimal,
    and we join them together. And then, it's created an array of char of 8 elements
    (the #RRGGBB color plus a "null terminator" = 8),
    the string with the #RRGGBB is copied to the array of char.

    Since it's necessary two digits for each color, if the RGB is "0", it's used an if statement to set as "00".
  */
  if (redFiltered == 0) {
    strRed = "00";    // If red is 0, sets the hexadecimal string as 00.
  }
  else {
    strRed = String(redFiltered, HEX);      // Convert the red to hexadecimal.
  }
  if (greenFiltered == 0) {
    strGreen = "00";  // If green is 0, sets the hexadecimal string as 00.
  }
  else {
    strGreen = String(greenFiltered, HEX);  // Convert the green to hexadecimal.
  }
  if (blueFiltered == 0) {
    strBlue = "00";   // If blue is 0, sets the hexadecimal string as 00.
  }
  else {
    strBlue = String(blueFiltered, HEX);    // Convert the blue to hexadecimal.
  }

  String strRGB = "#" + strRed + strGreen + strBlue;  // Create a string and join the RGB with the #.

``` 
And substitute it with:
```
  /*
    In order to visualize the color in the Adafruit IO platform
    it's necessary to convert the color decimal number to the hexadecimal format.
    (The decimal is 0 to 9, and hexadecimal is 0 to F)
    For that, first we create a string of variables for each RGB values and convert them as hexadecimal,
    and we join them together. And then, it's created an array of char of 8 elements
    (the #RRGGBB color plus a "null terminator" = 8),
    the string with the #RRGGBB is copied to the array of char.

    Since it's necessary two digits for each color, if the decimal value is less than "16", 
    it's used an if statement to add a "0", making it a two digits value.
  */

  strRed = String(redFiltered, HEX);      // Convert the red to hexadecimal.
  strGreen = String(greenFiltered, HEX);  // Convert the green to hexadecimal.
  strBlue = String(blueFiltered, HEX);    // Convert the blue to hexadecimal.


  if (strRed.length() == 1) {
    strRed = "0" + strRed; // If strRed has only one character, adds a "0" in the beggining of the string.
  }
  else {
  }
  if (strGreen.length() == 1) {
    strGreen = "0" + strGreen; // If strGreen has only one character, adds a "0" in the beggining of the string.
  }
  else {
  }
  if (strBlue.length() == 1) {
    strBlue = "0" + strBlue;   // If strBlue has only one character, adds a "0" in the beggining of the string.
  }
  else {
  }

  String strRGB = "#" + strRed + strGreen + strBlue;  // Create a string and join the RGB with the #.

```

If you prefer, you can download the [6_iob](https://github.com/Internet-of-Bananas/code) file with the corrections, but then you will need to update it with your wifi network and password, Adafruit IO username and key, and the color calibration values.

Save your file. Plug your NodeMCU board in the computer's USB port, in `Tools > Boards` menu, select the board `NodeMCU 1.0 (ESP-12E Module)` from the list. Again in the `Tools > Port` menu, select the *Com* port of your NodeMCU board. Select the menu `Skecth > Upload`, or press `Ctrl + U` or `Command + U` to upload the code to your board. When the uploading is done, unplug the USB cable from your computer, and your updated IoB station is ready to go.
