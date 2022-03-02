void setup_wifi() {
  // TODO
}

void task_wifi() {
  WiFi.disconnect(true);
  esp_wifi_stop();
  esp_wifi_set_mode(WIFI_MODE_NULL);
  // TODO
}
