
//-----------------------
void loopOSC(){
    
    static uint32_t prev_value_ms = millis();
    if (millis() > prev_value_ms + 16) // short active wait method before to send again
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
