/***************************************************
  Internet of Bananas
  A critical design project to collect data from bananas

  IoB is developed by Artur Cordeiro + Mattia Thibault
  https://github.com/Internet-of-Bananas
  Fortaleza / Tampere, 2021.

  *** IoB DHT11 test ***
  This skectch tests the temperature and humidity Sensor DHT11,
  and prints in the Serial Monitor the values from temperatura and humidity.

  In the code is used the library DHT Sensor Library from Adafruit Industries.

*/

// *** Setup parameters for the Sensor DHT11 ***
#include "DHT.h"          // Add the library DHT.
#define DHTPIN D1         // Set the pin connected to the DHT sensor.
#define DHTTYPE DHT11     // Select the sensor as DHT11.
DHT dht(DHTPIN, DHTTYPE); // Createe the dht object with the specified settings.
float temperature;        // Creates the variable to receive the temperature
float humidity;           // Creates the variable to receive the humidity


void setup()
{
  Serial.begin(9600); // Begin the serial communication, to print information in the Serial Monitor.

  dht.begin(); // Begin the instance of the DHT library.
}
void loop() {

  temperature = dht.readTemperature();  // Read the temperature in Celsius.
  humidity = dht.readHumidity();        // Read the relative humidity in %.

  // Print in the Serial Monitor the values of temperature and Humidity.
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("\t Humidity: ");
  Serial.println(humidity);

  // It's recommended a pause at least of two seconds
  // between each reading of the DHT11 sensor.
  delay(2000); // Pause.
}
