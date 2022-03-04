#define uS_TO_S_FACTOR 1000000

void loop() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;
  static unsigned long last_time2 = 0;

  // WARNING - Task order does matter!
  // NOTE - Actual task running rate will be
  // affected by cyclic sleep.
  // Every 10000ms for 60s
  // With always-running worker when Wi-Fi is active
  task_wifi();
  // Every 5000ms
  task_sensors();
  // Every 5000ms
  // With always-running safeties
  task_heater();
  // Every 5000ms
  task_battery();
  // Every 5000ms
  // With always-running parser
  task_gps();
  // Every 250ms so we can have 1s accuracy
  task_rtc();
  // Every APRS_mod_s (default was 30s)
  task_aprs();
  // Every time minute is a multiple of SSTV_mod_m
  task_sstv();

  if ((curr_time - last_time2) > 1000) {
    DBGPORT.print(msg_id);
    DBGPORT.print(' ');
    DBGPORT.print(tday);
    DBGPORT.print('/');
    DBGPORT.print(tmonth);
    DBGPORT.print('/');
    DBGPORT.print(tyear);
    DBGPORT.print(' ');
    DBGPORT.print(thour);
    DBGPORT.print(':');
    DBGPORT.print(tmin);
    DBGPORT.print(':');
    DBGPORT.println(tsec);

    last_time2 = curr_time;
  }

  // Cyclic sleep: HP_SLEEP_S of wake time, LP_SLEEP_S of sleep
  // WiFi inhibits sleep, only do cyclic sleep when WiFi has deactivated
  if (!wifiOn && ((curr_time - last_time) > (HP_SLEEP_S * 1000))) {
    DBGPORT.print(millis());
    DBGPORT.println(": Going to sleep");
    Serial.flush();
    //sleep_dra818();
    esp_sleep_enable_timer_wakeup(LP_SLEEP_S * uS_TO_S_FACTOR);
    esp_light_sleep_start();
    curr_time = millis();
    DBGPORT.print(curr_time);
    DBGPORT.println(": Wake up!");
    last_time = curr_time;
  }

  // This should be sufficient for now
  // In the future we might not do this and only
  // tickle the watchdog at critical junctures
  tickle_watchdog();
}
