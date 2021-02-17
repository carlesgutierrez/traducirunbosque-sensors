import processing.serial.*;    // Importing the serial library to communicate with the Arduino 
Serial myPort;      // Initializing a vairable named 'myPort' for serial communication
float background_color ;   // Variable for changing the background color

float lightVal;
boolean bLight = false;
float noiseVal;
boolean bNoise = false;
float batVal;

float humVal;
boolean bHum = false;
float tempVal;
boolean bTemp = false;

float VOCVal = 1;
boolean bVOC = false;
float CO2Val = 1;
boolean bCO2 = false;

//Serial vars
boolean bReadMainSerial = false;
boolean bInfoRaw = false;
boolean bInfoVal = false;

int timeRead = 0;
float timeNoReading = 0;
float maxTimeNoRead = 31; //This board read every 30 seconds unless we ask for data automatically TODO
boolean showErrorOnce = true;
float timeBetweenReads = 0;

String myDesiredArduPORT = "COM41";
int recievePORTOSC = 12000;
int sendPORTOSC = 55555;
boolean serialPortStatus = false;
boolean fastDataActive = false;

//TODO ask data auto everuy 100ms for quick data
//Check loadJson if not serial data is available

//OSC
import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

//Gui
import controlP5.*;
ControlP5 cp5;

//-----------------------------------------
void fastData(boolean theFlag) {
  if(theFlag==true) {
    fastDataActive = true;
    println("fastDataActive = true;");
  } else {
    fastDataActive = false;
    println("fastDataActive = false;");
  }
}

//------------------------------------------
void setup ( ) {

  size (500, 500);     // Size of the serial window, you can increase or decrease as you want
  surface.setTitle("SmartCity Sensors");
  surface.setResizable(false);
  surface.setLocation(0, 0);
  
  //Serial
  myPort  =  new Serial (this, myDesiredArduPORT, 115200); // COM36 // Set the com port and the baud rate according to the Arduino ID
  myPort.bufferUntil ( '\n' );   // Receiving the data from the Arduino IDE
  timeRead = millis();

  //Osc
  /* start oscP5, listening for incoming messages at port 12000 */
  oscP5 = new OscP5(this, recievePORTOSC);
  myRemoteLocation = new NetAddress("127.0.0.1", sendPORTOSC);
  
  //GUI
  cp5 = new ControlP5(this);
    // create a toggle and change the default look to a (on/off) switch look
  cp5.addToggle("fastData")
     .setPosition(width - 150, 85)
     .setSize(50,50)
     .setValue(false)
     ;
} 
//------------------------------------------
void draw ( ) {

  update();

  background ( 50 );   // Initial background color, when we will open the serial window 

  /*
   myMessage.add(noiseVal);
   myMessage.add(lightVal);
   myMessage.add(VOCVal);
   myMessage.add(CO2Val);
   myMessage.add(tempVal);
   myMessage.add(humVal);
   */

  int posDataX = 20;
  int posDataY = 100;
  int deltaY = 50;
  int countData = 0;
  //textSize(30);
  //fill(255, 0, 0);
  //text("SmartCitizen Data ", posDataX, +40);
  fill(255);
  textSize(28);
  text("Noise ............. "+str(noiseVal), posDataX, posDataY+countData*deltaY);
  countData++;
  text("Light .............. "+str(lightVal), posDataX, posDataY+countData*deltaY);
  countData++;
  text("VOC   ............. "+str(VOCVal), posDataX, posDataY+countData*deltaY);
  countData++;
  text("CO2  .............. "+str(CO2Val), posDataX, posDataY+countData*deltaY);
  countData++;
  text("Temperature .. "+str(tempVal), posDataX, posDataY+countData*deltaY);
  countData++;
  text("Humity ........... "+str(humVal), posDataX, posDataY+countData*deltaY);
  countData++;

  if (timeNoReading > maxTimeNoRead )fill(255, 0, 0);
  else fill(255);
  text("timeNoReading -> "+nf(timeNoReading, 0, 1)+" s", posDataX, height - 80);
  fill(255);
  text("IP -> "+oscP5.ip()+ "/:"+str(myRemoteLocation.port()), posDataX, height - 40);
  //text("localhost -> "+myRemoteLocation.address()+ "/:"+str(recievePORTOSC), posDataX, height - 10);


  if ( mousePressed  &&  ( mouseButton  ==  LEFT ) ) { // if the left mouse button is pressed

    //myPort.write ( "help\r" ) ;
  } 

  if  ( mousePressed  &&  ( mouseButton == RIGHT ) ) {  // if the right mouse button is pressed

    myPort.write ( "read noise\r" ) ; 
    myPort.write ( "read light\r" ) ;
  }
}

//------------------------------------------
void update() {
  timeNoReading = (millis() - timeRead)/1000.0f;
  if (timeNoReading > maxTimeNoRead ) {
    if (showErrorOnce) {
      println("Error-> Long time no data there -->Let's Close port and Reset this port");
      showErrorOnce = false;
      timeRead = millis();
      myPort.stop();
    } else {
      reOpenSerialPort();
      showErrorOnce = true;
    }
  }else if(fastDataActive && timeNoReading > 0.1){
    udpateFastData();
  }


  updateOSCSend();
}

//------------------------------------------
void udpateFastData() {
  myPort.write ( "read noise\r" ) ; 
  myPort.write ( "read light\r" ) ;
}

//------------------------------------------
void updateOSCSend() {
  /* TODO for loadJson its ok do all at time
   if (bSendAllData) {
   OscMessage myMessage = new OscMessage("/smartcity/data");
   
   myMessage.add(noiseVal);
   myMessage.add(lightVal);
   myMessage.add(VOCVal);
   myMessage.add(CO2Val);
   myMessage.add(tempVal);
   myMessage.add(humVal);
   
   oscP5.send(myMessage, myRemoteLocation);
   
   println("sent all data OSC");
   bSendAllData = false;
   }
   */
  if (bNoise) {
    OscMessage myMessage = new OscMessage("/smartcity/data/noise");
    myMessage.add(noiseVal);
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bNoise = false;
  }
  if (bLight) {
    OscMessage myMessage = new OscMessage("/smartcity/data/light");
    myMessage.add(lightVal);
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bLight = false;
  }
  if (bVOC) {
    OscMessage myMessage = new OscMessage("/smartcity/data/VOC");
    myMessage.add(VOCVal);
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bVOC = false;
  }
  if (bCO2) {

    OscMessage myMessage = new OscMessage("/smartcity/data/CO2");
    myMessage.add(CO2Val);
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bCO2 = false;
  }
  if (bTemp) {

    OscMessage myMessage = new OscMessage("/smartcity/data/Temp");
    myMessage.add(tempVal); /* add an float to the osc message */
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bTemp = false;
  }
  if (bHum) {

    OscMessage myMessage = new OscMessage("/smartcity/data/Hum");
    myMessage.add(humVal); /* add an float to the osc message */
    oscP5.send(myMessage, myRemoteLocation);
    //print("send OSC");
    bHum = false;
  }
}


//------------------------------------------
void exit() {
  println("trying to exit ... ");
  //myPort.stop();
  //oscP5.stop();
}
//------------------------------------------
void reOpenSerialPort() {
  println("Re Open Serial port... ");
  serialPortStatus = myPort.active();
  println("Is port active? "+str(serialPortStatus));

  String [] portsAvailable = myPort.list();
  boolean myPortCOMIsAvailable = false;
  for (int i=0; i<portsAvailable.length; i++) {
    boolean aval = myDesiredArduPORT.equals(portsAvailable[i]);
    println("aval ->"+ aval+ " is "+str(aval));
    myPortCOMIsAvailable = (myPortCOMIsAvailable || aval);
    println("portsAvailable[i] ->"+ portsAvailable[i]+ " is the one? "+str(myPortCOMIsAvailable));
  }
  if (myPortCOMIsAvailable) {
    println("Good Lets open ->"+ myDesiredArduPORT);
    print(portsAvailable);
    myPort  =  new Serial (this, myDesiredArduPORT, 115200);
    myPort.bufferUntil('\n');
    timeRead = millis();
  }
}
//------------------------------------------
void keyPressed() {

  if (key == 'O') {
    reOpenSerialPort();
  }

  if (key == 'C') {
    println("Manual Serial close port ... ");
    serialPortStatus = myPort.active();
    print("Is port active?"+str(serialPortStatus));
    myPort.stop();
    //exit();
    serialPortStatus = myPort.active();
    print("Is port active?"+str(serialPortStatus));
  }
}

//------------------------------------------
void serialEvent  (Serial myPort) {

  String info  =  (myPort.readStringUntil ( '\n' ) ) ;  // Changing the background color according to received data
  if (bReadMainSerial)println("-----------------------------------");
  if (bReadMainSerial)println(info);
  if (bReadMainSerial)print("--->");

  timeRead = millis();

  int pBat = info.indexOf("Battery:");
  if (pBat>=0) {
    //println(pBat);
    String infoBat = info.substring(pBat+8, pBat+8+6);
    if (bInfoRaw)print("Battery: ");
    if (bInfoRaw)println(infoBat);
    batVal = float(trim(infoBat));
    if (bInfoVal)print("batVal: ");
    if (bInfoVal)println(batVal);
  }
  int pNoise = info.indexOf("Noise:");
  if (pNoise>=0) {
    //println(pNoise);
    String infoNoise = info.substring(pNoise+7, pNoise+7+6);
    if (bInfoRaw)print("Noise: ");
    if (bInfoRaw)println(infoNoise);
    noiseVal = float(trim(infoNoise));
    if (bInfoVal)print("noiseVal: ");
    if (bInfoVal)println(noiseVal);
    if (noiseVal > 0)bNoise = true;
  }

  int pHumidity = info.indexOf("Humidity:");
  if (pHumidity>=0) {
    //println(pNoise);
    String infoHumidity = info.substring(pHumidity+10, pHumidity+10+6);
    if (bInfoRaw)print("Humidity: ");
    if (bInfoRaw)println(infoHumidity);
    humVal = float(trim(infoHumidity));
    if (bInfoVal)print("humVal: ");
    if (bInfoVal)println(humVal);
    if (humVal > 0)bHum = true;
  }

  int pTemperature = info.indexOf("Temperature:");
  if (pTemperature>=0) {
    String infoTemperature = info.substring(pTemperature+13, pTemperature+13+5);
    if (bInfoRaw)print("Temperature: ");
    if (bInfoRaw)println(infoTemperature);
    tempVal = float(trim(infoTemperature));
    if (bInfoVal)print("tempVal: ");
    if (bInfoVal)println(tempVal);
    if (tempVal > 0)bTemp = true;
  }

  int pLight = info.indexOf("Light:");
  if (pLight>=0) {
    String infoLight = info.substring(pLight+7, pLight+7+6);
    if (bInfoRaw)print("Light: ");
    if (bInfoRaw)println(infoLight);
    lightVal = float(trim(infoLight));
    if (bInfoVal)print("LightVal: ");
    if (bInfoVal)println(lightVal);
    if (lightVal > 0)bLight = true;
  }

  /* Terminal example
   Time: 2021-02-09T16:04:22Z 
   Battery: 100.00 %
   Noise: 47.21 dBc
   Humidity: 56.13 %
   Temperature: 21.19 C
   Light: 39.63 Lux
   */
} 

//------------------------------------------
/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());

  if (theOscMessage.checkAddrPattern("/serial/close")==true) {
    //int firstValue = theOscMessage.get(0).intValue(); 
    //exit();//not exingn , issues with osc I guess
  }
  if (theOscMessage.checkAddrPattern("/serial/restart")==true) {
    /*
    serialPortStatus = myPort.active();
     println("Is port active? "+str(serialPortStatus));
     String [] portsAvailable = myPort.list();
     println("Lets open!");
     print(portsAvailable);
     myPort  =  new Serial (this, myDesiredArduPORT, 115200);
     myPort.bufferUntil('\n');
     */
  }
}
