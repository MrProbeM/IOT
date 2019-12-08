#include "HX711.h"

HX711 scale(6, 5);

float calibration_factor = 375; // this calibration factor is adjusted according to my load cell
float units;
float ounces;

const int numReadings = 3;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void setup() {
   Serial.begin(115200);
   for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  //Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;

  
  
  total = total - readings[readIndex];
  readings[readIndex] = units;

  /*for(int i = 0; i <10; i++)
{
  //Serial.println(readings[i]);
}*/
   
 
  // add the reading to the total:
  total = total + readings[readIndex];
  
  
  
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

   // calculate the average:
  average = total / numReadings;
  Serial.println(average);
  //Serial.println(" grams");
  //delay(1);        // delay in between reads for stability
   
  
 delay(100);

  
}
