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
}

//-----------------------
void loopOSC() {

  static uint32_t prev_value_ms = millis();
  if (millis() > prev_value_ms + millisOSC) // short active wait method before to send again
  {
    if (!bErrorData) {
      OscWiFi.send(host, send_port, F("/spectrometer/green"), coefVerdor, coefVerdorCali, greenValue, greenValueCali, blueValue, blueValueCali, redValue, redValueCali, violetValue, violetValueCali, yellowValue, yellowValueCali, orangeValue, orangeValueCali);//publish_port
      prev_value_ms = millis();
      if (true) {
        Serial.println("send_port 55555");
        Serial.print("coefVerdor = "); Serial.println(coefVerdor);
        Serial.print("coefVerdorCali = "); Serial.println(coefVerdorCali);
      }
    }
  }
}
