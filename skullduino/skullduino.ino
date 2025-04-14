#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>
#include "Arduino_LED_Matrix.h"

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData;

ArduinoLEDMatrix matrix;

#define ROWS 8
#define COLUMNS_MATRIX 12
#define COLUMNS_SENSOR 8

#define DISTANCE_THRESHOLD 800

uint8_t frame[ROWS][COLUMNS_MATRIX] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

uint8_t pointX = 0, pointY = 0;

int imageResolution = 0;
int imageWidth = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[MMS] Hello, World!");

  matrix.begin();

  Wire.begin(); //This resets to 100kHz I2C
  Wire.setClock(400000); //Sensor has max I2C freq of 400kHz 
  
  Serial.println("Initializing...");
  if (myImager.begin() == false)
  {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1) ;
  }
  
  myImager.setResolution(ROWS * COLUMNS_SENSOR); //Enable all 64 pads
  
  imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution); //Calculate printing width

  myImager.startRanging();

}

void loop() {
  //Poll sensor for new data
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) //Read distance data into array
    {
      pointX = ROWS - 1;
      //The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          pointY = x;
          Serial.print("\t ");
          Serial.print("[");
          Serial.print(pointX);
          Serial.print(",");
          Serial.print(pointY);
          Serial.print("] ");
          Serial.print(measurementData.distance_mm[x + y]);

          if (measurementData.distance_mm[x + y] < DISTANCE_THRESHOLD) {
            frame[pointX][pointY] = 1;
          } else {
            frame[pointX][pointY] = 0;
          }
        }
        pointX--;
        Serial.println();
      }
      Serial.println();
      matrix.renderBitmap(frame, ROWS, COLUMNS_MATRIX);
    }
  }

  delay(5); //Small delay between polling

}
