#define uS_TO_S_FACTOR 1000000

void loop() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;
  static unsigned long last_time2 = 0;

  // WARNING - Task order does matter!
  task_sensors();

  task_heater();

  task_battery();

  task_gps();

  task_rtc();

  task_aprs();

  task_sstv();

  task_wifi();

  if ((curr_time - last_time2) > 1000) {
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
  
  if ((curr_time - last_time) > (HP_SLEEP_S * 1000)) {
    last_time = curr_time;
    DBGPORT.print(millis());
    DBGPORT.println(": Going to sleep");
    Serial.flush();
    //sleep_dra818();
    esp_sleep_enable_timer_wakeup(LP_SLEEP_S * uS_TO_S_FACTOR);
    esp_light_sleep_start();
    DBGPORT.print(millis());
    DBGPORT.println(": Wake up!");
  }

  // This should be sufficient for now
  // In the future we might not do this and only
  // tickle the watchdog at critical junctures
  tickle_watchdog();
}
