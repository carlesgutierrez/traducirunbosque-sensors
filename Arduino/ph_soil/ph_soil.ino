/////////////////////////////////////////
#include <ArduinoOSC.h>
#include "arduino_secrets.h";

// WiFi stuff
const IPAddress ip(192, 168, 1, 223);//id 2 phSoil and Co //192.168.1.100 //192, 168, 43, 202
const IPAddress gateway(192, 168, 1, 1); //192, 168, 43, 1
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
const char* host = "192.168.1.230";//"192.168.43.244";
const int recv_port = 55551;
const int send_port = 55555;
int millisOSC = 2000;

//Plotter
bool bDebugInfo = true;
bool bDebugVal = true;
bool bDebugSpace = true;
bool bNormalized = false;

bool bModePhHexaReading = false;

//Serial
bool bSerial = true;

/////////////////////////////////////////
#include <ArduinoModbus.h>
#include <Wire.h>
#include <Adafruit_AM2315.h>

//Led<
bool ledState = false;

// The time at which the sensors were last read.
unsigned long lastMillis = 0;
// How often (in milliseconds) the sensors will be read.
const unsigned long REPORT_INTERVAL = 1000 * 2.88; // 500 muestras repartidas en 24 horas, una muestra cada 2.88 segundos

//
float rangeMax = 100;
float rangeMin = 0;

//ph 485
float phVal;
float phMax = 900;
float phMin = 300;

/////////////////////////////////////////////////
//I2C temp and humity
// Connect RED of the AM2315 sensor to 5.0V
// Connect BLACK to Ground
// Connect WHITE to i2c clock
// Connect YELLOW to i2c data

Adafruit_AM2315 am2315;
float temperatureVal, humidityVal, tempRaw, humRaw;
float tempMax = 40;
float tempMin = 0;

float humMax = 100;
float humMin = 0;

/////////////////////
//Analogs
//moisture analog
float moistureVal = 0;
int sensorPinMoisture = A0;
float moistMax_Drain = 800;
float moistMin_Wet = 400;

//Control vars
bool bHumTempSensorAvailable = false;
bool bMoistureSensorAvailable = true; // Just analog
bool bPhSensorAvailable = false;


/////////////////////////////////////////////////
void setup() {
  //
  setupWifi();

  if (bSerial)Serial.begin(9600);
  //if(bSerial)while(!Serial);
  if (bSerial)Serial.println("Start ArduinoMKR with 3 Sensors");

  if (!ModbusRTUClient.begin(9600)) {
    if (bSerial)Serial.println("Failed to start Modbus RTU Client!");
    //while (1);
  } else bPhSensorAvailable = true;

  //if(bSerial)Serial.print("Use led to reflect connected to PH sensor: ");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //am2315 a Hum Temp Sensor
  if (! am2315.begin()) {
    if (bSerial)Serial.println("Sensor am2315 not found, check wiring & pullups I2C!");
    //while (1);
  } else bHumTempSensorAvailable = true;

  //Define serial plotter values

  if (bSerial)Serial.println("PH moistureVal TempC Hum% vibrationVal");

}


/////////////////////////////////////////////////
void loop() {

  //Stable values and more long term sensors
  if (millis() - lastMillis > REPORT_INTERVAL) {
    lastMillis = millis();

    // The Modbus RTU ph sensor:
    // Address: 0x01
    // Holding Register: 0x03 ? // 0x00
    // Read Length: 1 ?
    // ph = result[0]

    if (bPhSensorAvailable) {
      if (!ModbusRTUClient.requestFrom(0x01, HOLDING_REGISTERS, 0x03, 1)) {
        if (bDebugInfo) {
          if (bSerial)Serial.print("failed to read registers! ");
        }
        if (bDebugInfo) {
          if (bSerial)Serial.println(ModbusRTUClient.lastError());
        }
      }
      else {

        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(100);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(100);
        // If the request succeeds, the sensor sends the readings, that are
        // stored in the holding registers. The read() method can be used to
        // get the raw temperature and the humidity values.
        int rawPH = ModbusRTUClient.read();
        String textoPH = (String)rawPH;
        if (bModePhHexaReading) {
          phVal = hexToDec(textoPH);
        }
        else {
          phVal = textoPH.toInt();
        }
        
        if (bSerial)Serial.print("textoPH: ");
        if (bSerial)Serial.println(textoPH);
        // The raw values //TODO map values to [-1 .. 1]
        // if (bNormalized)phVal = map(phVal, phMin, phMax, rangeMin, rangeMax); // between 0 ( good ph -> Alkaline ) and 100 ( bad -> Ph Acidic )

        if (bDebugInfo) {
          if (bSerial)Serial.print("Ph[0.100](3..9):");
          if (bSerial)Serial.print(phVal);
          if (bSerial)Serial.print(" ");
        }
      }
    }

    //Moisture Sensor
    if (bMoistureSensorAvailable) {
      float moistureRaw = analogRead(sensorPinMoisture);
      if (bNormalized)moistureVal = map(moistureRaw, moistMin_Wet, moistMax_Drain, rangeMax, rangeMin); // between 1( wet ) and 0 ( drain )
      else moistureVal = moistureRaw;

      if (bDebugInfo) {
        if (bSerial)Serial.print("Moisture(0..100):");
      }
      if (bDebugVal) {
        if (bSerial)Serial.print(moistureVal);
      }
      if (bDebugSpace) {
        if (bSerial)Serial.print(" ");
      }
    }

    if (bHumTempSensorAvailable) {

      if (! am2315.readTemperatureAndHumidity(&tempRaw, &humRaw)) {
        //if(bSerial)Serial.println("Failed to read data from AM2315");
        return;
      } else {
        if (bNormalized)temperatureVal = map(tempRaw, tempMin, tempMax, rangeMin, rangeMax); // between 0 and 1
        else temperatureVal = tempRaw;

        if (bNormalized)humidityVal = map(humRaw, humMin, humMax, rangeMin, rangeMax); // between 0 and 1
        else humidityVal = humRaw;
      }
      if (bDebugInfo) {
        if (bSerial)Serial.print("Temp*C(0..40):");
      }
      if (bDebugVal) {
        if (bSerial)Serial.print(temperatureVal);
      }
      if (bDebugSpace) {
        if (bSerial)Serial.print(" ");
      }

      if (bDebugInfo) {
        if (bSerial)Serial.print("Hum%(0..100):");
      }
      if (bDebugVal) {
        if (bSerial)Serial.print(humidityVal);
      }
      if (bDebugSpace) {
        if (bSerial)Serial.print(" ");
      }
    }

    if (bSerial)Serial.println();
  }

  //real time sensors
  loopOSC();
}
