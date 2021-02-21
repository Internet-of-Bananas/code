/***************************************************
  Internet of Bananas
  A critical design project to collect data from bananas

  IoB is developed by Artur Cordeiro + Mattia Thibault
  https://github.com/Internet-of-Bananas
  Fortaleza / Tampere, 2021.

  *** IoB Color test ***
  This skectch tests the Color Sensor TCS3200,
  and prints in the Serial Monitor the values from red, green and blue color.

  The TCS3200 Color Sensor has light-to-frequency converters,
  the output is a digital wave with frequency proportional to light intensity.
  So a brighter light will result in a higher frequency, and a darker light in a lower frequency.
  More information on the datasheet: https://www.alldatasheet.com/datasheet-pdf/pdf/454462/TAOS/TCS3200.html

  Arduino has the function pulseIn() that reads in microseconds the pulse of a pin,
  that means the function measure the time that the pulse goes from HIGH to LOW, or vice-versa.

  This is also called Pulse Width, which is the inverse of frequency,
  so a lower pulse width value means a higher frequency,
  and a higher pulse width value, means a lower frequency.

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

  // Print in the Serial Monitor the values of red, green and blue.
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print(", Green: ");
  Serial.print(green);
  Serial.print(", Blue: ");
  Serial.println(blue);

  delay(1000); // Pause of 1000 milisseconds, or 1 second.
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
