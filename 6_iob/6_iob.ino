/***************************************************
  Internet of Bananas
  A critical design project to collect data from bananas

  IoB is developed by Artur Cordeiro + Mattia Thibault
  https://github.com/Internet-of-Bananas
  Fortaleza / Tampere, 2021.

  *** 6 - IoB Final colde with full functionality***
  This code collects the data from the color, temperature, and humidity sensors and send them to Adafruit.

  This code requires the libraries:
  EWMA from jonnieZG - https://github.com/jonnieZG/EWMA;
  DHT Sensor Library from Adafruit Industries - https://github.com/adafruit/DHT-sensor-library
  MQTT from Adafruit Industries.
 ****************************************************
  Must use ESP8266 Arduino from:
  https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ***************************************************
*/

#include <ESP8266WiFi.h>          //  Add the library ESP8266WiFi.
#include "Adafruit_MQTT.h"        //  Add the library Adafruit_MQTT.
#include "Adafruit_MQTT_Client.h" //  Add the library Adafruit_MQTT_Client.

// IMPORTANT! Before sharing the code, always delete your login and password information.
// When adding your information, insert it between the "", with no extra space.
/************************* WiFi Access Point *********************************/

#define WLAN_SSID      "wifiName"     //  Name of the WiFi network.
#define WLAN_PASS      "wifiPassword" //  WiFi password.

/************************* Adafruit.io Setup *********************************/
// You need an account at the Adafruit IO to publish data.
// If you don't have it, you can create one at
// https://accounts.adafruit.com/users/sign_up

#define AIO_SERVER      "io.adafruit.com"       //  Address of the MQTT server from Adafruit IO, don't change.
#define AIO_SERVERPORT  1883                    //  Number of server port from Adafruit IO, don't change.
#define AIO_USERNAME    "username"  //  Insert your Adafruit IO user name.
#define AIO_KEY         "key"    //  Insert your Adafruit IO user key.

/************ Global State (don't change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
// This setup the feeds for publishing on Adafruit.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish pubColor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/color");
Adafruit_MQTT_Publish pubTemperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish pubHumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

// *** Setup parameters for the Color Sensor TCS3200 ***
// Create the variable to define which pins you are using in your NodeMCU.
const int s0 = D6;    // Set the pin S0 on the NodeMCU.
const int s1 = D7;    // Set the pin S1 on the NodeMCU.
const int s2 = D3;    // Set the pin S2 on the NodeMCU.
const int s3 = D4;    // Set the pin S3 on the NodeMCU.
const int out = D5;   // Set the pin Output on the NodeMCU.

int red;      // The variable to receive the pulse width value of red.
int green;    // The variable to receive the pulse width value of green.
int blue;     // The variable to receive the pulse width value of blue.

//  Add the library Exponentially Weighted Moving Average.
#include <Ewma.h>

//  Create the filter from red, green and blue,
//  and set the respective smoothing factor,
//  in range from 0 to 1, higher the value, less smoothing.
Ewma fR(0.5);   // Creates the EWMA filter for the red value.
Ewma fG(0.5);   // Creates the EWMA filter for the green value.
Ewma fB(0.5);   // Creates the EWMA filter for the blue value.

int redFiltered;    // The variable to receive the filtered value of red.
int greenFiltered;  // The variable to receive the filtered value of green.
int blueFiltered;   // The variable to receive the filtered value of blue.


// *** Setup parameters for the Sensor DHT11 ***
#include "DHT.h"          // Add the library DHT.
#define DHTPIN D1         // Set the pin connected to the DHT sensor.
#define DHTTYPE DHT11     // Select the sensor as DHT11.
DHT dht(DHTPIN, DHTTYPE); // Createe the dht object with the specified settings.
float temperature;        // Creates the variable to receive the temperature
float humidity;           // Creates the variable to receive the humidity


// *** Color calibration***
// This section is used to calibrate your specific color sensor.
// Update the variables below with readings from your sensor.
// Use the sensor on a white and then on a black opaque surface and take note of the minimum and maximum values for red, green and blue.

int redMin = 54;       // Update this value with your red reading of the white paper.
int redMax = 552;      // Update this value with your red reading of the black paper.

int greenMin = 55;     // Update this value with your green reading of the white paper.
int greenMax = 579;    // Update this value with your green reading of the black paper.

int blueMin = 40;      // Update this value with your blue reading of the white paper.
int blueMax = 448;     // Update this value with your blue reading of the black paper.


String strRed;        // String variable for red hexadecimal color.
String strGreen;      // String variable for green hexadecimal color.
String strBlue;       // String variable for blue hexadecimal color.
String strRGB;        // String variable for RGB hexadecimal color.


void setup()
{
  Serial.begin(9600); // Begin the serial communication, to print information in the Serial Monitor.

  dht.begin(); // Begin the instance of the DHT library.

  // Define the "s" pins from the Color Sensor as outputs.
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  // And the output from the Color Sensor as an input pin.
  pinMode(out, INPUT);

  // The s0 and s1 pins set the output frequency scale of the Color Sensor.
  // The output scale can be set to 2%, 20% and 100%,
  // that will result in diferent numeric range.
  // In the IoB project it's used 20%.
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);


  // Connect to WiFi access point, using the name and password previously defined.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);          // Print the name of the WiFi.
  WiFi.begin(WLAN_SSID, WLAN_PASS);   // Connects to the WiFi network.

  // Wait the WiFi connection be well succeed.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print the conection confirmation and the IP number from NodeMCU Esp8266.
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.println("address IP: ");
  Serial.println(WiFi.localIP());

}
void loop() {

  // Connects to the MQTT server and keep it alive.
  // It automatically reconnect when disconnected.
  // See the MQTT_connect() function definition further below.
  MQTT_connect();

  /************************* Color data *********************************/
  color(); // Function to update the variables red, green and blue with the respectives Pulse Width values.
  redFiltered = fR.filter(red);       // Apply the red filter to the red value.
  greenFiltered = fG.filter(green);   // Apply the green filter to the red value.
  blueFiltered = fB.filter(blue);     // Apply the blue filter to the red value.
  /*
    // Print in the Serial Monitor the filtered values of red, green and blue.
    // To calibrate the sensor, use it with a white paper and take note of the lowest values, from red, green and blue.
    // Repeat the proceadure with a black paper, and take note of the highest values, from red, green and blue.
    // These numbers will be necessary for a more accurate color measurement.
    Serial.print(redFiltered);
    Serial.print(",");
    Serial.print(greenFiltered);
    Serial.print(",");
    Serial.println(blueFiltered);
  */


  /*
     It's necessary to convert the filtered value to a numeric range from 0 to 255.
     The map() function calculate a number from the current range to the target range.
     We need to set the lower and higher bounds of the current range,
     and the lower and higher bounds of the desired target range.
     + https://www.arduino.cc/reference/en/language/functions/math/map/

     In the current range, the HIGHER number means DARKER color,
     and in the target range (0-255) the LOWER number means DARKER color.
     So, the map() function must invert,
     we set the redMax as lower bound and the redMin as higher bound of the current range,
     and set 0 as the lower bound and 255 as higher bound of the target range.
     And repeat the same proceadure with green and blue.

  */
  redFiltered = int(map(redFiltered, redMax, redMin, 0, 255));          // Mapping the red filtered value to the range from 0 to 255.
  greenFiltered = int(map(greenFiltered, greenMax, greenMin, 0, 255));  // Mapping the green filtered value to the range from 0 to 255.
  blueFiltered = int(map(blueFiltered, blueMax, blueMin, 0, 255));      // Mapping the blue filtered value to the range from 0 to 255.

  // To avoid numbers outside of the range, we need to constrain them.
  // If there's a value below 0, it will be set to 0,
  // if there's a value above 255, it will set to 255.
  redFiltered = constrain(redFiltered, 0, 255);       // Constrains red.
  greenFiltered = constrain(greenFiltered, 0, 255);   // Constrains green.
  blueFiltered = constrain(blueFiltered, 0, 255);     // Constrains blue.

  // Print in the Serial Monitor the mapped values of red, green and blue in a range from 0 to 255.
  Serial.print(redFiltered);
  Serial.print(",");
  Serial.print(greenFiltered);
  Serial.print(",");
  Serial.println(blueFiltered);

  /*
    In order to visualize the color in the Adafruit IO platform
    it's necessary to convert the color decimal number to the hexadecimal format.
    (The decimal is 0 to 9, and hexadecimal is 0 to F)
    For that, first we create a string of variables for each RGB values and convert them as hexadecimal,
    and we join them together. And then, it's created an array of char of 8 elements
    (the #RRGGBB color plus a "null terminator" = 8),
    the string with the #RRGGBB is copied to the array of char.

    Since it's necessary two digits for each color, if the RGB is "0", it's used an if statement to set as "00".
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

  char charArrayRGB[8]; // Create the character array with 8 elements.
  strRGB.toCharArray(charArrayRGB, 8); // Copy the joined string to the array of char.
  Serial.println(charArrayRGB); // Print the color in the Serial Monitor.

  /************************* Temperature and Humidity data *********************************/
  temperature = dht.readTemperature();  // Read the temperature in Celsius.
  humidity = dht.readHumidity();        // Read the relative humidity in %.
  // Print in the Serial Monitor the values of temperature and Humidity.
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\t Humidity: ");
  Serial.println(humidity);


  /************************* Publish data to Adafruit.io *********************************/
  //  Now the feed will be published to Adafruit IO.
  //  It will print a message on Serial Monitor if there's error or if it was well succeeded to publish.

  // *** Color ***
  Serial.print("Sending pubColor: ");
  Serial.print(charArrayRGB);
  Serial.print("... ");
  if (! pubColor.publish(charArrayRGB)) {
    Serial.println("Error! No data sent.");
  } else {
    Serial.println("Ok, data sent!");
  }

  // *** Temperature ***
  Serial.print("Sending pubTemperature: ");
  Serial.print(temperature);
  Serial.print("... ");
  if (! pubTemperature.publish(temperature)) {
    Serial.println("Error! No data sent.");
  } else {
    Serial.println("Ok, data sent!");
  }

  // *** Humidity ***
  Serial.print("Sending pubHumidity: ");
  Serial.print(humidity);
  Serial.print("... ");
  if (! pubHumidity.publish(humidity)) {
    Serial.println("Error! No data sent.");
  } else {
    Serial.println("Ok, data sent!");
  }

  delay(30000); // Pause for 30 seconds.
}


// *** Function to get the Pulse Width values. ***
void color() {
  // Select the red photodiode
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  // Count in microseconds the pulse width for the red frequency.
  red = pulseIn(out, HIGH); // Update red variable with the time it goes from HIGH to LOW.

  //delay(10); // Pause.

  // Select the green photodiode
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  // Count in microseconds the pulse width for the green frequency.
  green = pulseIn(out, HIGH);// Update green variable with the time it goes from HIGH to LOW.

  //delay(10); // Pause.

  // Select the blue photodiode
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  // Count in microseconds the pulse width for the blue frequency.
  blue = pulseIn(out, HIGH);// Update blue variable with the time it goes from HIGH to LOW.
}


// *** Function to connect and reconnect when necessary on the Adafruit IO server. ***
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  // The connect() will return 0 for connected.
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // Waint 5 seconds.
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
