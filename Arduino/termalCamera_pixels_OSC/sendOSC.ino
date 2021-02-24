
void onOscReceived(const OscMessage& m)
{
    Serial.print(m.remoteIP()); Serial.print(" ");
    Serial.print(m.remotePort()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    //Serial.print(m.arg<int>(0)); Serial.print(" ");
    //Serial.print(m.arg<float>(1)); Serial.print(" ");
    Serial.print(m.arg<String>(2)); Serial.println();

    Serial.print("Lets Reset ESP"); 
    ESP.restart();
}

void setupWifi() {
  Serial.begin(115200);
  delay(2000);

  // WiFi stuff (no timeout setting for WiFi)
#ifdef ESP_PLATFORM
  WiFi.disconnect(true, true); // disable wifi, erase ap info
  delay(1000);
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SECRET_SSID);
    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);

    // wait 10 second for connection:
    delay(10000);

    // output some diagnostic info
    Serial.println("Status = " + WiFi.status());
  }
  Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());

  OscWiFi.subscribe(recv_port, "/reset/thermalCam", onOscReceived);
  Serial.println("/reset/thermalCam -> onOscReceived"); 
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

//-----------------------
void loopOSC(){
    OscWiFi.update(); // should be called to receive + send osc
        // or do that separately
    // OscWiFi.parse(); // to receive osc
    // OscWiFi.post(); // to publish osc
    
    static uint32_t prev_value_ms = millis();
    if (millis() > prev_value_ms + millisOSC) // short active wait method before to send again
    {
        int i = xx_time_get_time();
        float f = 1000000.f / (float)(micros() - prev_value_ms * 1000);
        String s = pixelsString; //String("testing");
        OscWiFi.send(host, send_port, F("/tempetarure/thermalCamLeaves"), i, f, s);//publish_port
        prev_value_ms = millis();
        if(false){
          Serial.println("send_port 55555");
          Serial.print("-> timeStamp Millis = ");Serial.println(i);
          Serial.print("time = ");Serial.println(f);
          Serial.print("data = ");Serial.println(s);
        }
    }
}
