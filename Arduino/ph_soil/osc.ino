

void setupWifi(){
    if(bSerial)Serial.begin(115200);
    Serial.print("setup WiFi start");
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
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network:
      WiFi.begin(ssid, pwd);
  
      // wait 10 second for connection:
      delay(10000);

      // output some diagnostic info
      Serial.println("Status = " + WiFi.status());
    }
  
    if(bSerial)Serial.print("WiFi connected, IP = "); if(bSerial)Serial.println(WiFi.localIP());
}

//-----------------------
void loopOSC(){
    
    static uint32_t prev_value_ms = millis();
    if (millis() > prev_value_ms + 2000) // 1s wait method before to send again
    {
        OscWiFi.send(host, send_port, F("/Soil/data"), phVal, moistureVal, temperatureVal, humidityVal);//all floats -> phVal, moistureVal, temperatureVal, humidityVal,
        prev_value_ms = millis();
        if(false){
          if(bSerial)Serial.println("send_port 55555");
        }
    }
}
