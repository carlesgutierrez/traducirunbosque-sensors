/***************************************************************************
  This is a library for the Adafruit AS7262 6-Channel Visible Light Sensor

  This sketch reads the sensor

  Designed specifically to work with the Adafruit AS7262 breakout
  ----> http://www.adafruit.com/products/3779

  These sensors use I2C to communicate. The device's I2C address is 0x49
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

/*
   Si hay algo en frente brilla más y da mas valor, si el valor es sup a 100 o más empezemos a valorar lo que da sino no.
   El coef, da el mismo valor si está calibrado o no así que no sirve mucho para el resultado de coef de verde usar la calibración.
   TODO aplicar esta lógica de calculo.
   TODO enviar por OSC el resultado. Copy paste del termal camera
*/
/////////////////////////////
// WiFi stuff
#include <ArduinoOSC.h>
#include "arduino_secrets.h";
const IPAddress ip(192, 168, 1, 131);//Instalació 1:192, 168, 1, 121 //Instalación 2: 192, 168, 1, 131
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
// for ArduinoOSC
const char* host = "192.168.1.230";//Instalació 1:192, 168, 1, 220 //Instalación 2: 192, 168, 1, 230
const int recv_port = 54321;
const int send_port = 55555;

int millisOSC = 100;

/////////////////////////////
#include <Wire.h>
#include "Adafruit_AS726x.h"

//create the object
Adafruit_AS726x ams;

//buffer to hold raw values
uint16_t sensorValues[AS726x_NUM_CHANNELS];

#define LED 2

//buffer to hold calibrated values (not used by default in this example)
float calibratedValues[AS726x_NUM_CHANNELS];


//All values
float greenValue, greenValueCali;
float blueValue, blueValueCali;
float redValue, redValueCali;

float violetValue, violetValueCali;
float yellowValue, yellowValueCali;
float orangeValue, orangeValueCali;

//Composed color values
float blueRedVal, blueRedValCali;

//My desired value
float coefVerdor = 0.0f;
float coefVerdorCali = 0.0f;
boolean bErrorData = false;

boolean bPrintAllData = false;

///////////////////////////////////

void setupSpectrometer() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);

  //begin and make sure we can talk to the sensor
  if (!ams.begin()) {
    Serial.println("could not connect to sensor! Please check your wiring.");
    while (1);
  }
}
////////////////////////
void setup() {
  Serial.begin(9600);
  setupSpectrometer();
  setupWifi();

}

void loop() {
  loopSpectrometer();
  loopOSC();
}

////////////////////////
void loopSpectrometer() {

  //read the device temperature
  uint8_t temp = ams.readTemperature();

  //ams.drvOn(); //uncomment this if you want to use the driver LED for readings
  ams.startMeasurement(); //begin a measurement

  //wait till data is available
  bool rdy = false;
  while (!rdy) {
    delay(5);
    rdy = ams.dataReady();
  }
  ams.drvOn(); //uncomment this if you want to use the driver LED for readings

  //read the values!
  ams.readRawValues(sensorValues);
  ams.readCalibratedValues(calibratedValues);

  if (bPrintAllData) {
    Serial.print("Temp: "); Serial.print(temp);
    Serial.print(" Violet: "); Serial.print(sensorValues[AS726x_VIOLET]);
    Serial.print(" Blue: "); Serial.print(sensorValues[AS726x_BLUE]);
    Serial.print(" Green: "); Serial.print(sensorValues[AS726x_GREEN]);
    Serial.print(" Yellow: "); Serial.print(sensorValues[AS726x_YELLOW]);
    Serial.print(" Orange: "); Serial.print(sensorValues[AS726x_ORANGE]);
    Serial.print(" Red: "); Serial.print(sensorValues[AS726x_RED]);
    Serial.println();
  }
  greenValue = sensorValues[AS726x_GREEN];
  blueValue = sensorValues[AS726x_BLUE];
  redValue = sensorValues[AS726x_RED];

  violetValue = sensorValues[AS726x_VIOLET]; 
  yellowValue = sensorValues[AS726x_ORANGE];
  orangeValue = sensorValues[AS726x_ORANGE];
   
  blueRedVal = (blueValue + redValue);

  if (greenValue != 0 && blueValue != 0 && redValue != 0) {
    coefVerdor = greenValue / blueRedVal;
    Serial.print(" Cociente de calidad de verdor: "); Serial.print(coefVerdor, 3);
    bErrorData = false;
  } else {
    Serial.print("Error transmision data ");
    bErrorData = true;
  }
  if (bPrintAllData) {
    Serial.println();

    Serial.print("Calibrated values");
    Serial.print("Temp: "); Serial.print(temp);
    Serial.print(" Violet: "); Serial.print(calibratedValues[AS726x_VIOLET]);
    Serial.print(" Blue: "); Serial.print(calibratedValues[AS726x_BLUE]);
    Serial.print(" Green: "); Serial.print(calibratedValues[AS726x_GREEN]);
    Serial.print(" Yellow: "); Serial.print(calibratedValues[AS726x_YELLOW]);
    Serial.print(" Orange: "); Serial.print(calibratedValues[AS726x_ORANGE]);
    Serial.print(" Red: "); Serial.print(calibratedValues[AS726x_RED]);
    Serial.println();
  }

  greenValueCali = calibratedValues[AS726x_GREEN];
  blueValueCali = calibratedValues[AS726x_BLUE];
  redValueCali = calibratedValues[AS726x_RED];

  violetValueCali = calibratedValues[AS726x_VIOLET]; 
  yellowValueCali = calibratedValues[AS726x_YELLOW];
  orangeValueCali = calibratedValues[AS726x_ORANGE];
  
  blueRedValCali = (blueValueCali + redValueCali);

  if (greenValueCali != 0 && blueValueCali != 0 && redValueCali != 0) {
    coefVerdorCali = greenValueCali / blueRedValCali;
    //Serial.print(" Cociente de calidad de verdor calibrado: "); Serial.print(coefVerdorCali, 3);
    bErrorData = false;
  } else {
    Serial.print("Error data values");
    bErrorData = true;
  }

  Serial.println();
  Serial.println();


}
