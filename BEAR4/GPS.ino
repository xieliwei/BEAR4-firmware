void set_ublox_pms(uint8_t v) {
  // UBX-CFG-PMS
  // 0x00 -> Full Power
  // 0x01 ->Balanced
  // 0x02 -> Interval
  // 0x03 -> Aggressive with 1 Hz (lowest power with continuous operation)
  // 0x04 -> Aggressive with 2 Hz
  // 0x05 -> Aggressive with 4 Hz
  
  uint8_t setPMS[8] = {0};
  setPMS[1] = v;

  ubxPacket customCfg;
  customCfg.cls = UBX_CLASS_CFG;
  customCfg.id = UBX_CFG_PMS;
  customCfg.len = 8;
  customCfg.counter = 0;
  customCfg.startingSpot = 0;
  customCfg.payload = setPMS;
  customCfg.checksumA = 0;
  customCfg.checksumB = 0;
  customCfg.valid = SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED;
  customCfg.classAndIDmatch = SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED;

  if (myGNSS.sendCommand(&customCfg) != SFE_UBLOX_STATUS_DATA_SENT) {
    Serial.println(F("Attempt to set low power-mode failed!"));
  } else {
    Serial.print(F("Set GPS to power mode "));
    Serial.println(v, HEX);
  }
}

void task_gps() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  myGNSS.getPVT();

  if (curr_time - last_time > 5000) {
    last_time = curr_time;

    tyear = myGNSS.getYear();
    tmonth = myGNSS.getMonth() % 13;
    tday = myGNSS.getDay() % 32;

    thour = myGNSS.getHour() % 24;
    tmin = myGNSS.getMinute() % 60;
    tsec = myGNSS.getSecond() % 60;
    tms = myGNSS.getMillisecond() % 1000;
    tsync = millis();

    fixType = myGNSS.getFixType();

    glatitude = (float)myGNSS.getLatitude() / 10000000;
    glongitude = (float)myGNSS.getLongitude() / 10000000;
    galtitudeMSL = (float)myGNSS.getAltitudeMSL() / 1000;

    gheading = myGNSS.getHeading() / 100000;
    gspeed = (float)myGNSS.getGroundSpeed() * 3600 / 1000000;

    gSIV = myGNSS.getSIV();
    gPDOP = myGNSS.getPDOP() / 100;

    DBGPORT.print(tyear);
    DBGPORT.print('/');
    DBGPORT.print(tmonth);
    DBGPORT.print('/');
    DBGPORT.print(tday);
    DBGPORT.print(' ');
    DBGPORT.print(thour);
    DBGPORT.print(':');
    DBGPORT.print(tmin);
    DBGPORT.print(':');
    DBGPORT.print(tsec);

    DBGPORT.print(F(" Lat: "));
    DBGPORT.print(glatitude);

    DBGPORT.print(F(" Long: "));
    DBGPORT.print(glongitude);
    DBGPORT.print(F(" (degrees)"));

    DBGPORT.print(F(" AltMSL: "));
    DBGPORT.print(galtitudeMSL);
    DBGPORT.print(F(" (m)"));

    DBGPORT.print(F(" Heading: "));
    DBGPORT.print(gheading);
    DBGPORT.print(F(" (degrees)"));

    DBGPORT.print(F(" Speed: "));
    DBGPORT.print(gspeed);
    DBGPORT.print(F(" (km/h)"));

    DBGPORT.print(F(" SIV: "));
    DBGPORT.print(gSIV);

    DBGPORT.print(F(" PDOP: "));
    DBGPORT.print(gPDOP);

    DBGPORT.print(F(" Fix: "));
    if (fixType == 0) DBGPORT.print(F("No fix"));
    else if (fixType == 1) DBGPORT.print(F("Dead reckoning"));
    else if (fixType == 2) DBGPORT.print(F("2D"));
    else if (fixType == 3) DBGPORT.print(F("3D"));
    else if (fixType == 4) DBGPORT.print(F("GNSS + Dead reckoning"));
    else if (fixType == 5) DBGPORT.print(F("Time only"));

    DBGPORT.println();
  }
}

void setup_gps() {
  uint8_t gnss_tries = 4;
  do {
    Serial.println("GNSS: trying 460800 baud");
    Serial2.begin(460800);
    if (myGNSS.begin(Serial2) == true) break;

    delay(100);
    Serial.println("GNSS: trying 9600 baud");
    Serial2.begin(9600);
    if (myGNSS.begin(Serial2) == true) {
      Serial.println("GNSS: connected at 9600 baud, switching to 38400");
      myGNSS.setSerialRate(460800);
      delay(100);
    } else {
      //myGNSS.factoryReset();
      delay(500); //Wait a bit before trying again to limit the Serial output
    }
    gnss_tries -= 1;
  } while (gnss_tries);
  Serial.println("GNSS serial connected");

  myGNSS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
  myGNSS.setNavigationFrequency(1);
  myGNSS.setAutoPVT(true);
  myGNSS.saveConfiguration(); //Save the current settings to flash and BBR

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Find where the AssistNow data for today starts and ends
  uint16_t payloadSize = assistnow_sz;
  const uint8_t *payload = assistnow_dat;

  size_t todayStart = 0; // Default to sending all the data
  size_t tomorrowStart = (size_t)payloadSize;

  // Uncomment the next line to enable the 'major' debug messages on Serial so you can see what AssistNow data is being sent
  //myGNSS.enableDebugging(Serial, true);

  // Find the start of today's data
  todayStart = myGNSS.findMGAANOForDate(payload, (size_t)payloadSize, todays_date[0], todays_date[1], todays_date[2]);
  if (todayStart < (size_t)payloadSize) {
    //Serial.print(F("Found the data for today starting at location "));
    //Serial.println(todayStart);
  } else {
    Serial.println("Could not find the data for today. This will not work well. The GNSS needs help to start up quickly.");
  }

  // Find the start of tomorrow's data
  tomorrowStart = myGNSS.findMGAANOForDate(payload, (size_t)payloadSize, todays_date[0], todays_date[1], todays_date[2], 1);
  if (tomorrowStart < (size_t)payloadSize) {
    //Serial.print(F("Found the data for tomorrow starting at location "));
    //Serial.println(tomorrowStart);
  } else {
    Serial.println("Could not find the data for tomorrow. (Today's data may be the last?)");
  }

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Push the assisted time and location to the module
  myGNSS.setUTCTimeAssistance(todays_date[0], todays_date[1], todays_date[2],
                              todays_time[0], todays_time[1], todays_time[2], 0,
                              65535, 0);
  myGNSS.setPositionAssistanceLLH(13000000L, 1037500000L, 60000, 10000000L);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Push the AssistNow data for today to the module - without the time

  // ***** Use the UBX_MGA_ACK_DATA0 messages *****
  // Tell the module to return UBX_MGA_ACK_DATA0 messages when we push the AssistNow data
  myGNSS.setAckAiding(1);

  // Push the AssistNow data for today.
  myGNSS.pushAssistNowData(todayStart, true, payload, tomorrowStart - todayStart, SFE_UBLOX_MGA_ASSIST_ACK_YES, 100);

  // Set aggressive power saving (1Hz)
  set_ublox_pms(0x03);
}
