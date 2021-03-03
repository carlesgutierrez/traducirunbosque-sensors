/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera

  This sketch tries to read the pixels from the sensor

  Designed specifically to work with the Adafruit AMG88 breakout
  ----> http://www.adafruit.com/products/3538

  These sensors use I2C to communicate. The device's I2C address is 0x69

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
// WiFi stuff
#include <ArduinoOSC.h>
#include "arduino_secrets.h";

const IPAddress ip(192, 168, 1, 132);// instalación 1 -> 192, 168, 1, 122 .... instalación 2 -> 192, 168, 1, 132
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
const char* host = "192.168.1.230";// instalación 1 -> 192, 168, 1, 220 .... instalación 2 -> 192, 168, 1, 230
const int recv_port = 55551;
const int send_port = 55555;

int millisOSC = 100;


//Thermal
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
String pixelsString = "";

//-----------------------
void setup() {
  setupThermalCam();
  setupWifi();

}
//-----------------------
void loop() {
  loopThermalCam();
  loopOSC();
}

//-----------------------
int64_t xx_time_get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}


//-----------------------
void printDataArray() {
  Serial.print("[");
  for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    Serial.print(pixels[i - 1]);
    Serial.print(", ");
    if ( i % 8 == 0 ) Serial.println();
  }
  Serial.println("]");
  Serial.println();
}
//------------------------
void loopThermalCam() {
  //read all the pixels
  amg.readPixels(pixels);
  pixelsString = "";

  for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    pixelsString += pixels[i - 1];
    if (i != AMG88xx_PIXEL_ARRAY_SIZE)pixelsString += ",";
  }
  //if(true)Serial.println(pixelsString);
  //if(true)printDataArray();
}
