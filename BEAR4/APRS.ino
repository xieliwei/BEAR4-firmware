void send_report(bool use_gps) {
  static uint8_t page = 0;
  uint8_t packet[200];
  uint8_t sz = 0;

  bool tempb;
  uint8_t tempu8;
  uint16_t tempu16;
  uint32_t tempu32;
  //  int8_t tempi8;
  //  int16_t tempi16;
  //  int32_t tempi32;
  float tempf;
  //  String temps;

  float thealti = 0;

  packet[sz++] = use_gps ? '@' : '>';

  sz += sprintf((char*)&(packet[sz]), "%02d%02d%02d", tday % 32, thour % 60, tmin % 60);

  if (use_gps) {
    packet[sz++] = 'z';

    tempf = glatitude;
    tempb = tempf >= 0;
    if (!tempb) tempf *= -1.0;
    tempu8 = tempf;
    tempf -= tempu8;
    tempf *= 60;
    tempu16 %= 91;
    sz += sprintf((char*)&(packet[sz]), "%02d%02.2f", tempu8, tempf);
    packet[sz++] = tempb ? 'N' : 'S';

    // Main symbol table
    packet[sz++] = '/';

    tempf = glongitude;
    tempb = tempf >= 0;
    if (!tempb) tempf *= -1.0;
    tempu16 = tempf;
    tempf -= tempu16;
    tempf *= 60;
    tempu16 %= 181;
    sz += sprintf((char*)&(packet[sz]), "%03d%02.2f", tempu16, tempf);
    packet[sz++] = tempb ? 'E' : 'W';

    // Balloon symbol
    packet[sz++] = 'O';

    sz += sprintf((char*)&(packet[sz]), "%03d", gheading % 360);

    packet[sz++] = '/';

    tempf = gspeed;
    tempf /= 1.852;
    tempu16 = tempf;
    sz += sprintf((char*)&(packet[sz]), "%03d", tempu16 % 1000);

    thealti = galtitudeMSL;

    //
    //    tempf = myGNSS.getAltitudeMSL();
    //    if (tempf < 0) tempf = 0.0;
    //    tempf *= 3.280839895;
    //    tempf /= 1000;
    //    tempu32 = tempf;
    //    tempu32 %= 1000000L;
    //    temps = String(tempu32);
    //    for (tempu8 = 6; tempu8 > 0; tempu8--) {
    //      if (tempu8 > temps.length()) {
    //        packet[sz++] = '0';
    //      } else {
    //        packet[sz++] = temps.c_str()[temps.length() - tempu8];
    //      }
    //    }
  } else {
    thealti = paltitudeMSL;
  }

  packet[sz++] = '/';

  packet[sz++] = 'A';
  packet[sz++] = '=';

  tempf = thealti;
  tempf *= 3.280839895;
  if (tempf <= 0.0) {
    tempu32 = 0;
  } else {
    tempu32 = tempf;
  }
  sz += sprintf((char*)&(packet[sz]), "%06lld", tempu32 % 1000000LL);

  packet[sz++] = ' ';

  sz += sprintf((char*)&(packet[sz]), "%06dTx", msg_id++);

  packet[sz++] = 'C' + page;
  packet[sz++] = ' ';
  switch (page) {
    default:
    case 0:
      sz += sprintf((char*)&(packet[sz]), "%.2fC %.2fhPa %.2fV %02dS", ptemp, ppress, BV, gSIV);
      break;
    case 1:
      sz += sprintf((char*)&(packet[sz]), "%d %02d %0.2fV1 %0.2fV2 %c %.2fC", fixType, gPDOP % 100, Cell1, Cell2, heaterOn ? 'H' : 'X', btemp);
      break;
    case 2:
      sz += sprintf((char*)&(packet[sz]), "%+05dx %+05dy %+05dz %c %c", accel_x, accel_y, accel_z, freefall ? 'F' : 'X', heaterOn ? 'H' : 'X');
      freefall = false;
      break;
  }
  if (++page == 3) {
    page = 0;
  }

  packet[sz++] = ' ';
  for (tempu8 = 0; tempu8 < comment_suffix.length(); tempu8++) {
    packet[sz++] = comment_suffix.c_str()[tempu8];
  }

  // And send the update
  //wake_dra818();
  // Adjust path depending on altitude)
  if (thealti > 2000) {
    APRS_sendPkt(packet, sz, 3);
  } else {
    APRS_sendPkt(packet, sz, 4);
  }
  //sleep_dra818();
}

void task_aprs() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  bool use_gps = false;
  if (fixType == 0) use_gps = false;        // No Fix
  else if (fixType == 1) use_gps = false;   // Dead reckoning
  else if (fixType == 2) use_gps = true;    // 2D
  else if (fixType == 3) use_gps = true;    // 3D
  else if (fixType == 4) use_gps = true;    // GNSS + Dead reckoning
  else if (fixType == 5) use_gps = false;   // Time only

  //if (((tsec % APRS_mod_s) == 0) && (curr_time - last_time) >= ((APRS_mod_s * 1000) - 1000)) {
  if ((curr_time - last_time) >= (APRS_mod_s * 1000)) {
    send_report(use_gps);
    DBGPORT.println("APRS sent");
    last_time = curr_time;
  }
}

void setup_aprs() {
  APRS_init();

  APRS_setCallsign(callsign.c_str(), callsign_ssid);

  APRS_useAlternateSymbolTable(false);
  APRS_setSymbol('O');

  APRS_printSettings(Serial);

  //wake_dra818();
  APRS_sendMsg(boot_message.c_str(), boot_message.length());
  //sleep_dra818();
}
