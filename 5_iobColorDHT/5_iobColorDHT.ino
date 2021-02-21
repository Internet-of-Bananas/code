/***************************************************
  Internet of Bananas
  A critical design project to collect data from bananas

  IoB is developed by Artur Cordeiro + Mattia Thibault
  https://github.com/Internet-of-Bananas
  Fortaleza / Tampere, 2021.

  *** 5 - IoB Color, temperatura and humidity***
  This skectch test the color sensor with the temperature and humidity sensor.

  It's required the libraries:
  EWMA from jonnieZG - https://github.com/jonnieZG/EWMA;
  DHT Sensor Library from Adafruit Industries - https://github.com/adafruit/DHT-sensor-library


*/

// *** Setup parameters for the Color Sensor TCS3200 ***
// Create the variable to define the pins
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
Ewma fR(0.1);   // Creates the EWMA filter for the red value.
Ewma fG(0.1);   // Creates the EWMA filter for the green value.
Ewma fB(0.1);   // Creates the EWMA filter for the blue value.

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
// Update the variables below with your reading.
// Use the sensor in white and black paper to take note of the minimum and maximum values for red, green and blue.

int redMin = 54;       // Update this value with your reading of the white paper.
int redMax = 552;      // Update this value with your reading of the black paper.

int greenMin = 55;     // Update this value with your reading of the white paper.
int greenMax = 579;    // Update this value with your reading of the black paper.

int blueMin = 40;      // Update this value with your reading of the white paper.
int blueMax = 448;     // Update this value with your reading of the black paper.

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
}
void loop() {

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
    For that, first we creat a string variables for each RGB values and convert them as hexadecimal,
    and we join them together. And then, it's created an array of char of 8 elements
    (the #RRGGBB color plus a "null terminator" = 8),
    the string with the #RRGGBB is copied to the array of char.
    //
  */
  String strRed = String(redFiltered, HEX);           // Create a string and convert the red to hexadecimal.
  String strGreen = String(greenFiltered, HEX);       // Create a string and convert the green to hexadecimal.
  String strBlue = String(blueFiltered, HEX);         // Create a string and convert the blue to hexadecimal.
  String strRGB = "#" + strRed + strGreen + strBlue;  // Create a string and join the RGB with the #.
  char charArrayRGB[8]; // Create the character array with 8 elements.
  strRGB.toCharArray(charArrayRGB, 8); // Copy the joined string to the array of char.
  Serial.println(charArrayRGB); // Print the color in the Serial Monitor.


  temperature = dht.readTemperature();  // Read the temperature in Celsius.
  humidity = dht.readHumidity();        // Read the relative humidity in %.
  // Print in the Serial Monitor the values of temperature and Humidity.
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\t Humidity: ");
  Serial.println(humidity);


  delay(2000); // Pause.
}


// The function to get the Pulse Width values.
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
