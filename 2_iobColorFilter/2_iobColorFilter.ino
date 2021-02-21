/***************************************************
  Internet of Bananas
  A critical design project to collect data from bananas

  IoB is developed by Artur Cordeiro + Mattia Thibault
  https://github.com/Internet-of-Bananas
  Fortaleza / Tampere, 2021.

  *** IoB Color test - filter EWMA ***
  This skectch filter the color reading using
  "Exponentially Weighted Moving Average" (EWMA) to decrease noise.
  https://en.wikipedia.org/wiki/Moving_average

  In the code is used the library EWMA from jonnieZG,
  https://github.com/jonnieZG/EWMA.

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
Ewma fR(0.05);   // Creates the EWMA filter for the red value.
Ewma fG(0.05);   // Creates the EWMA filter for the green value.
Ewma fB(0.05);   // Creates the EWMA filter for the blue value.

int redFiltered;    // The variable to receive the filtered value of red.
int greenFiltered;  // The variable to receive the filtered value of green.
int blueFiltered;   // The variable to receive the filtered value of blue.


void setup()
{
  Serial.begin(9600); // Begin the serial communication, to print information in the Serial Monitor.

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

  redFiltered = fR.filter(red); // Apply the red filter to the red value.
  greenFiltered = fG.filter(green); // Apply the green filter to the red value.
  blueFiltered = fB.filter(blue); // Apply the blue filter to the red value.

  // Print in the Serial Monitor the filtered values of red, green and blue.
  // To calibrate the sensor, use it with a white paper and take note of the lowest values, from red, green and blue.
  // Repeat the proceadure with a black paper, and take note of the highest values, from red, green and blue.
  // These numbers will be necessary for a more accurate color measurement.
  Serial.print(redFiltered);
  Serial.print(",");
  Serial.print(greenFiltered);
  Serial.print(",");
  Serial.println(blueFiltered);


  // Print in the Serial Monitor the raw values of red, green and blue.
  //  Serial.print(red);
  //  Serial.print(",");
  //  Serial.print(green);
  //  Serial.print(",");
  //  Serial.println(blue);

  delay(500); // Pause.
}


// The function to get the Pulse Width values.
void color() {
  // Select the red photodiode
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  // Count in microseconds the pulse width for the red frequency.
  red = pulseIn(out, HIGH); // Update red variable with the time it goes from HIGH to LOW.

  delay(10); // Pause.

  // Select the green photodiode
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  // Count in microseconds the pulse width for the green frequency.
  green = pulseIn(out, HIGH);// Update green variable with the time it goes from HIGH to LOW.

  delay(10); // Pause.

  // Select the blue photodiode
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  // Count in microseconds the pulse width for the blue frequency.
  blue = pulseIn(out, HIGH);// Update blue variable with the time it goes from HIGH to LOW.
}
