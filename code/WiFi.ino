void initWifi()
{
  WiFi.begin(ssid, pass);
  displayText(1, 0, 10, "Reconnecting...", true);
  delay(2000);
}

void sendtoServer(char * nmea)
{
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
                           
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    String httpRequestData = "senten="+String(nmea);           
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    displayText(1, 0, 10, "Sending...", true);
    // Send HTTP POST request in the form of JSON
    // http.addHeader("Content-Type", "application/json");
    // int httpResponseCode = http.POST("{\"nmea\":"+String(nmea)+"}");
    
    if (httpResponseCode>0) 
    {
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();
      // Serial.println(payload);
      displayText(1, 0, 10, "NETSEND OK", true);
      offline = 0;
    }
    else 
    {
      // Serial.print("Error code: ");
      // Serial.println(httpResponseCode);
      displayText(1, 0, 10, "NET FAIL", true);
      offline = 1;
    }
    // Free resources
    http.end();
  }
  else 
  {
    // Serial.println("WiFi Disconnected");
  }  
}

void WiFiEvent(WiFiEvent_t event) 
{
  switch(event) 
  {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      // Serial.println("WiFi connected");
      // Serial.println("IP address: ");
      // Serial.println(WiFi.localIP());
      xTimerStop(wifiReconnectTimer, 0); 
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      // Serial.println("WiFi lost connection");
      displayText(1, 0, 10, "Offline", true);
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void initWiFiTimer()
{
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(initWifi));
}