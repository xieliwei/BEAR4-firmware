inline void generate_fskid_str(String s) {
  for (uint8_t i = 0; i < s.length(); i++) {
    generate_fskid_char(s.c_str()[i] - 0x20);
  }
}

inline void generate_fskid_char(uint8_t c) {
  uint8_t i = 0b1;
  while (i & 0b111111) {
    if (c & i) {
      set_dac_freq(1900);
    } else {
      set_dac_freq(2100);
    }
    delay(22);
    i <<= 1;
  }
}

inline uint8_t colour_lookup(uint8_t val, uint8_t comp) {
  if (comp < 3) {
    return im_cm[val][2 - comp];
  } else {
    return im_cm[val][3];
  }
}

void robot_img(const uint8_t image[HEIGHT][WIDTH][COMPONENTS]) {
  //wake_dra818();
  tune_dra818("145.5000");
  digitalWrite(PIN_RAD_PTT, LOW);
  delay(500);
  // VOX Signal
  set_dac_freq(1900);
  dac_cw_generator_enable();
  delay(100);
  set_dac_freq(1500);
  delay(100);
  set_dac_freq(1900);
  delay(100);
  set_dac_freq(1500);
  delay(100);
  set_dac_freq(2300);
  delay(100);
  set_dac_freq(1500);
  delay(100);
  set_dac_freq(2300);
  delay(100);
  set_dac_freq(1500);
  delay(100);

  // VIS + Sync
  set_dac_freq(1900);
  delay(300);
  set_dac_freq(1200);
  delayMicroseconds(10000);
  set_dac_freq(1900);
  delay(300);
  set_dac_freq(1200);
  delayMicroseconds(30000);
  // Robot32
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1100);
  delayMicroseconds(30000);
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1300);
  delayMicroseconds(30000);
  set_dac_freq(1100);
  delayMicroseconds(30000);
  set_dac_freq(1200);
  delayMicroseconds(30000);

  // Image
  for (uint8_t i = 0; i < HEIGHT; i += 2) {
    set_dac_freq(1200);
    delayMicroseconds(9000);
    set_dac_freq(1500);
    delayMicroseconds(3000);
    for (uint16_t j = 0; j < WIDTH; j++) {
      float freq = 1500.0 + ((16.0 + (0.003906 * ((65.738 * colour_lookup(image[i][j][0], 0)) + (129.057 * colour_lookup(image[i][j][0], 1)) + (25.064 * colour_lookup(image[i][j][0], 2))))) * 3.1372549);
      set_dac_freq(freq);
      delayMicroseconds(260); // 275
    }
    set_dac_freq(1500);
    delayMicroseconds(4500); // 4500
    set_dac_freq(1900);
    delayMicroseconds(1500); // 1500 // 2250
    for (uint16_t j = 0; j < HALFWIDTH; j++) {
      const float hi0 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 0) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 0) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 0) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 0)) / 4.0;
      const float hi1 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 1) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 1) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 1) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 1)) / 4.0;
      const float hi2 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 2) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 2) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 2) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 2)) / 4.0;
      float freq = 1500.0 + ((128.0 + (0.003906 * ((112.439 * hi0) + (-94.154 * hi1) + (-18.285 * hi2)))) * 3.1372549);
      set_dac_freq(freq);
      delayMicroseconds(255); // 275
    }

    set_dac_freq(1200);
    delayMicroseconds(9000);
    set_dac_freq(1500);
    delayMicroseconds(3000);
    for (uint16_t j = 0; j < WIDTH; j++) {
      float freq = 1500.0 + ((16.0 + (0.003906 * ((65.738 * colour_lookup(image[i + 1][j][0], 0)) + (129.057 * colour_lookup(image[i + 1][j][0], 1)) + (25.064 * colour_lookup(image[i + 1][j][0], 2))))) * 3.1372549);
      set_dac_freq(freq);
      delayMicroseconds(260); // 275
    }
    set_dac_freq(2300);
    delayMicroseconds(4500); // 4500
    set_dac_freq(1900);
    delayMicroseconds(1500); // 1500 / 2250
    for (uint16_t j = 0; j < HALFWIDTH; j++) {
      const float hi0 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 0) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 0) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 0) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 0)) / 4.0;
      const float hi1 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 1) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 1) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 1) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 1)) / 4.0;
      const float hi2 =
        (float)((uint16_t)colour_lookup(image[i][2 * j][0], 2) +
                (uint16_t)colour_lookup(image[i + 1][2 * j][0], 2) +
                (uint16_t)colour_lookup(image[i][2 * j + 1][0], 2) +
                (uint16_t)colour_lookup(image[i + 1][2 * j + 1][0], 2)) / 4.0;
      float freq = 1500.0 + ((128.0 + (0.003906 * ((-37.945 * hi0) + (-74.494 * hi1) + (112.439 * hi2)))) * 3.1372549);
      set_dac_freq(freq);
      delayMicroseconds(255); // 275
    }
  }

  // EOF
  set_dac_freq(1500);
  delay(300);
  set_dac_freq(1900);
  delay(100);
  set_dac_freq(1500);
  delay(100);
  set_dac_freq(1900);
  delay(100);
  set_dac_freq(1500);
  delay(100);

  // FSKID
  generate_fskid_char(0x20);
  generate_fskid_char(0x2A);
  generate_fskid_str(callsign + callsign_suffix);
  generate_fskid_char(0x01);
  dac_cw_generator_disable();
  delay(100);

  digitalWrite(PIN_RAD_PTT, HIGH);
  tune_dra818("144.3900");
  //sleep_dra818();
}

void task_sstv() {
  // RBF - Set this to false before flight
  static bool run_now = false;

  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  // RBF - Set this to true before flight
  bool inhibit_sstv = true;

  // Only allow SSTV after SSTV_inhibit_height_m height
  if (galtitudeMSL > SSTV_inhibit_height_m || paltitudeMSL > SSTV_inhibit_height_m) {
    inhibit_sstv = false;
  }
  // Or after it has been more than SSTV_inhibit_time_ms time
  if (curr_time > SSTV_inhibit_time_ms) {
    inhibit_sstv = false;
  }

  bool use_gps = false;
  if (fixType == 0) use_gps = false;        // No Fix
  else if (fixType == 1) use_gps = false;   // Dead reckoning
  else if (fixType == 2) use_gps = true;    // 2D
  else if (fixType == 3) use_gps = true;    // 3D
  else if (fixType == 4) use_gps = true;    // GNSS + Dead reckoning
  else if (fixType == 5) use_gps = false;   // Time only

  // No matter what, if the GPS is not functioning, we don't SSTV to give GPS a chance to lock on
  if (!use_gps) inhibit_sstv = true;

  if (run_now || (!inhibit_sstv && (((tmin % SSTV_mod_m) == 0) && (curr_time - last_time) >= ((SSTV_mod_m * 60000L) - 60000L)))) {
    char sbuf[128] = {0}
    ;
    memcpy(im_buf, im_ref, WIDTH * HEIGHT * COMPONENTS);
    im.setCursor(0, 0);
    im.setTextSize(4);
    im.print(callsign + callsign_suffix);

    im.setCursor(194, 207);
    im.setTextSize(4);
    im.print("BEAR4");

    // We start custom output at 150x35
    // That gives us 170x65 (without belly) or 170x135 of usable space
    // Font size 2: 15x4 chars | Font size 1: 34x8
    im.setCursor(149, 37);
    im.setTextSize(2);

    // Line 1: Date Time
    sprintf(sbuf, "%02d/%02d/%02d %02d:%02d", tday, tmonth, tyear % 100, thour, tmin, tsec);
    im.print(sbuf);

    // Line 2: Lat
    im.setCursor(149, 54);
    sprintf(sbuf, "Lat %.4f", glatitude);
    im.print(sbuf);

    // Line 3: Lon
    im.setCursor(149, 71);
    sprintf(sbuf, "Lon %.4f", glongitude);
    im.print(sbuf);

    // Line 4: Alti Speed
    im.setCursor(149, 88);
    sprintf(sbuf, "%.0fm %.0fkm/h", galtitudeMSL, gspeed);
    im.print(sbuf);

    // Line 5: Temp Press
    im.setCursor(162, 105);
    sprintf(sbuf, "%d*C %.0fhPa", (int8_t)ptemp, ppress);
    im.print(sbuf);
    
    robot_img(im_buf);
    last_time = curr_time;
  }

  run_now = false;
}

void setup_sstv() {
  im.setTextColor(0);
  // Font is 5x8 per character
  im.setTextSize(1);
  im.setTextWrap(true);
}
