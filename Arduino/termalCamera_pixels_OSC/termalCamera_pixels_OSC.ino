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
const char* ssid = "TP-Link_71B4";
const char* pwd = "47824096";
const IPAddress ip(192, 168, 1, 122);//201 //192.168.1.129 // 192, 168, 43, 201
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
const char* host = "192.168.1.220";//"192.168.1.129";//.200
const int recv_port = 54321;
const int bind_port = 54345;
const int send_port = 55555;
const int publish_port = 54445;

//Thermal
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
String pixelsString = "";

void setupWifi() {
  Serial.begin(115200);
  delay(2000);

  // WiFi stuff (no timeout setting for WiFi)
#ifdef ESP_PLATFORM
  WiFi.disconnect(true, true); // disable wifi, erase ap info
  delay(1000);
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());
}
void setupThermalCam() {
  Serial.println("Setup");
  Serial.println(F("AMG88xx pixels"));

  bool status;

  // default settings
  status = amg.begin();
  if (!status) {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Pixels Test --");
  Serial.print("AMG status--> ");
  Serial.println(status);
  Serial.println();
  delay(100); // let sensor boot up
}
void setup() {
  setupThermalCam();
  setupWifi();

}

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
