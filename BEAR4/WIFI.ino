void setup_wifi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(wifissid, wifipsk);
  IPAddress ipaddr = WiFi.softAPIP();
  Serial.print("IP: ");
  Serial.println(ipaddr);
  DynamicArduinoOTA.setHostname("BEAR10");

  DynamicArduinoOTA
  .onStart([]() {
    String type;
    if (DynamicArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  DynamicArduinoOTA.begin();

  udpstream.begin(ipaddr, 45455);

  wifiOn = true;
}

void task_wifi() {
  unsigned long long curr_time = millis();
  static unsigned long long offset = 0;
  if ((curr_time - offset) > WIFI_TIMEOUT) {
    Serial.println("Disabling WiFi");
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    btStop();

    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_NULL);

    wifiOn = false;
  } else {
    DynamicArduinoOTA.handle();
    udpstream.flush();
    udpstream.task();
    if (udpstream.contains('!')) {
      udpstream.clear();
      // Extend Wi-Fi session
      offset = curr_time;
    }
  }
}
