/*
 * <copyright file="EmgMuscleSensor.ino" company="dyadica.co.uk">
 * Please see http://www.dyadica.co.uk/emg-muscle-sensors-and-arduino 
 * for more information. Details provided at end of file.
 */
 
// The max analog data
int max_analog_dta = 300; 

// The min analog data
int min_analog_dta = 100;

// The static analog data
int static_analog_dta   = 0;                

// The pin to which the 
// sensor is attached.
int sensorPin = A0;

// Storage for new sensor data
double newSensorData[32];

// Storage for filtered data
double fltSensorData[32];

// Value to apply during filtering
double ALPHA = 0.15;

// Tolerance value used to trigger
int TOLERANCE = 100;

// Properties used for serial communication

// Buffer for the incoming data
char inData[100];

// Buffer for the parsed data chunks
char *inParse[100];

// Storage for data as string
String inString = "";

// Incoming data id
int index = 0;

// Flag if the string is complete
boolean stringComplete = false; 

// Methods

// The setup routine runs once 
// when you press reset.
void setup() 
{
  // initialize serial communication 
  // at 9600 bits per second
  
  Serial.begin(9600);

  // Get a static/rest analog value

  // 1. first initialise the tmp
  // property sum
  long sum = 0;

  // 2. Take 1000 readings
  for(int j=0; j<1000; j++)
  {
    sum += getAnalog(A0);
    delay(1);
  }

  // 3. Get the average reading
  sum /= 1000;

  // 4. Set the value to == the
  // average reading 
  static_analog_dta = sum;

  // Output the average after the
  // value has been set
  
  Serial.print("static_analog_dta = ");
  Serial.print(static_analog_dta);
  Serial.println();
}

// the loop routine runs over 
// and over again forever.

void loop() 
{
  // Poll for a new serial event 
  // and or input.
  serialEvent();
  
  // Update values when a newline 
  // arrives.
  
  if (stringComplete) 
  {
    // Provide some debug output!
    Serial.print("Input:");
    Serial.print(inString);
    Serial.println();

    // Parse the input string
    ParseSerialData();
    
    // Reset inString to empty
    inString = "";   
     
    // Reset the system for further 
    // input of data   
    stringComplete = false; 
  }
  
  // Poll for a new sensor event 
  // and or input
  sensorEvent();  
}

/*
 * Poll pins for a sensor event / input
 */

void sensorEvent()
{
  // read the input on the sensor.
  // in this instance val is an
  // average value of 32 readings
  
  int val = getAnalog(sensorPin);

  // Example 1 Fire a trigger if the 
  // value is greater than the static 
  // value plus user defined tolerance

  if(val > (static_analog_dta + TOLERANCE))
  {
    // Fire Trigger 1
  }

  // At the same time the getAnalog 
  // function also stores the 32 
  // values. These values can then 
  // be used for other filters etc.

  double flt = LowPassFilter(newSensorData, fltSensorData);

  // Example 2 Fire a trigger if the flt 
  // value is greater than the static value 
  // plus user defined tolerance

  if(flt > (static_analog_dta + TOLERANCE))
  {
    // Fire Trigger 1
  }

  // We could even fire a trigger if
  // the value is lower

  if(flt < (static_analog_dta - TOLERANCE))
  {
    // Fire Trigger 1
  }
}

/*
 * Method used to get an average sensor value and then
 * store this value for filtering if required.
 * Based upon: https://github.com/Seeed-Studio/Grove_EMG_detector_demo_code/blob/master/Grove_EMG_detector_demo_code.ino
 */
 
int getAnalog(int pin)
{
    long sum = 0;

    // Reset the data storage
    // and update the total
    // value.

    for(int i=0; i<32; i++)
    {
      int val = analogRead(pin);
      newSensorData[i] = val;
      sum += val;
    }

    // Average the value
    int dta = sum>>5;

    // Update the maximum and minimum values
    max_analog_dta = dta>max_analog_dta ? dta : max_analog_dta;         // if max data
    min_analog_dta = min_analog_dta>dta ? dta : min_analog_dta;         // if min data

    // Return the average value
    return sum >> 5;
}

/*
 * Simple version of a lowpass filter
 * Based upon: https://gist.github.com/dyadica/518bacfac54b890db6b1
 */
 
double LowPassFilter(double input[], double output[])
{
    if (output == NULL)
    {
      output = input;
      return NULL;
    }

    double filteredValue = 0;

    // filter each of the values
    
    for (int i = 0; i < 32; i++)
    {
        output[i] = output[i] + ALPHA * (input[i] - output[i]);

        // Add to total for filtered output

        filteredValue += output[i];
    }

    // Divide for average value

    return filteredValue /= 32;
}

/*
 * SerialEvent occurs whenever a new data comes in the
 * hardware serial RX.  This routine is run between each
 * time loop() runs, so using delay inside loop can delay
 * response.  Multiple bytes of data may be available.
 * 
 * Source:  http://www.arduino.cc/en/Tutorial/SerialEvent 
 */
 
void serialEvent() 
{
  // Read while we have data
  while (Serial.available() && stringComplete == false) 
  {
    // Read a character
    char inChar = Serial.read(); 
    // Store it in char array
    inData[index] = inChar; 
    // Increment where to write next
    index++;     
    // Also add it to string storage just
    // in case, not used yet :)
    inString += inChar;
    
    // Check for termination character
    if (inChar == '\n') 
    {
      // Reset the index
      index = 0;
      // Set completion of read to true
      stringComplete = true;
    }
  }
}

/*
 * Function used to parse the incoming data and then
 * set the tolerance value and or trigger a described 
 * function.
 * Source: https://github.com/dyadica/Unity_SerialPort/blob/master/Serial_Test_Script/Serial_Test_Script.ino
 */

void ParseSerialData()
{
  // The data to be parsed
  char *p = inData;
  
  // Temp store for each data chunk
  char *str;   
  
  // Id ref for each chunk 
  int count = 0;
    
  // Loop through the data and seperate it into
  // chunks at each "," delimeter
  while ((str = strtok_r(p, ",", &p)) != NULL)
  { 
    // Add chunk to array  
    inParse[count] = str;
    // Increment data count
    count++;      
  }
  
  // If the data has two values then..  
  if(count == 2)
  {
    // Define value 1 as a function identifier
    char *func = inParse[0];
    
    // Define value 2 as a property value
    char *prop = inParse[1];
    
    // Call the relevant identified function 
    // or output. 
    switch(*func)
    {
      case 'T': 
        // Update the tolerance value
        TOLERANCE = atoi( prop);
        // Debug
        Serial.print("Tolerance: ");
        Serial.print(TOLERANCE);
        Serial.println();
        
        break;
        
      case 'S': 
        // Get the static value
        Serial.print("Static: ");
        // In this instance prop is just
        // a dummy value
        Serial.print(static_analog_dta);
        Serial.println(); 
        break;
    }    
  }
  // Clear inData, so there are no leftovers for next cycle
  memset(&inData[0], 0, sizeof(inData));
}

/*
 * Please see http://www.dyadica.co.uk/emg-muscle-sensors-and-arduino for more information
 * 
 * <copyright file="EmgMuscleSensor.ino" company="dyadica.co.uk">
 * 
 * Copyright (c) 2010, 2016 All Right Reserved, http://www.dyadica.co.uk
 * This source is subject to the dyadica.co.uk Permissive License.
 * Please see the http://www.dyadica.co.uk/permissive-license file for more information.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 * </copyright>
 * <author>www.dyadica.co.uk</author>
 * <contact>https://www.facebook.com/ADropInTheDigitalOcean</contact>
 * <date>09.03.2016</date>
 * 
 * <summary>
 * This script contains EMG Sensor demo code for use with both the Grove & MyoWare EMG
 * Sensors. A tolerance value for a muscle trigger event can be set via sending "T,Value"
 * over serial. * 
 * Code is also provided to demonstrate use of both simple average smoothing and very basic
 * low-pass filtering. * 
 * Please note it'll take about serval seconds to detect a session static analog value.
 * During this time you should hold your muscle static.
 * </summary>
 */
