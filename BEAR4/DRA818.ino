void sine_setup(uint32_t freq) {
  dac_cw_config_t dac_cw_config = {
    .en_ch = DAC_CHANNEL_1,
    .scale = DAC_CW_SCALE_1,
    .phase = DAC_CW_PHASE_0,
    .freq = freq << 3,
    .offset = 1
  };
  dac_cw_generator_config(&dac_cw_config);
  REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, 7);
  dac_cw_generator_enable();
  dac_output_enable(DAC_CHANNEL_1);
}

inline void set_dac_freq(uint32_t freq) {
  dac_ll_cw_set_freq(freq << 3);
}

inline void sleep_dra818() {
  digitalWrite(PIN_RAD_EN, LOW);
}

inline void tune_dra818(String freq) {
  for (int i = 0; i < 3; i++) {
    radioCtrl.print("AT+DMOSETGROUP=0," + freq + ",144.3900,0000,2,0000\r\n");
    //radioCtrl.print("AT+DMOSETGROUP=0,144.3900,144.3900,0000,2,0000\r\n");
    //radioCtrl.print("AT+DMOSETGROUP=0,145.5000,144.3900,0000,2,0000\r\n");
    char tc = 0;
    char atp[3] = {0};
    bool atdetected = false;
    unsigned long atp_start = millis();
    do {
      while (!radioCtrl.available() && ((millis() - atp_start) < 5000));
      tc = radioCtrl.read();
      Serial.write(tc);
      atp[0] = atp[1];
      atp[1] = atp[2];
      atp[2] = tc;
      if (atp[0] == '+' && atp[1] == 'D' && atp[2] == 'M') atdetected = true;
    } while ((tc != '\n' || !atdetected) && ((millis() - atp_start) < 5000));
    if (atdetected) break;
  }
  Serial.println();
}

inline void wake_dra818() {
  digitalWrite(PIN_RAD_EN, HIGH);
  delay(600);
  for (int i = 0; i < 3; i++) {
    radioCtrl.print("AT+DMOCONNECT\r\n");
    char tc = 0;
    char atp[3] = {0};
    bool atdetected = false;
    unsigned long atp_start = millis();
    do {
      while (!radioCtrl.available() && ((millis() - atp_start) < 5000));
      tc = radioCtrl.read();
      Serial.write(tc);
      atp[0] = atp[1];
      atp[1] = atp[2];
      atp[2] = tc;
      if (atp[0] == '+' && atp[1] == 'D' && atp[2] == 'M') atdetected = true;
    } while ((tc != '\n' || !atdetected) && ((millis() - atp_start) < 5000));
    if (atdetected) break;
  }
  Serial.println();

  for (int i = 0; i < 3; i++) {
    radioCtrl.print("AT+DMOSETVOLUME=8\r\n");
    char tc = 0;
    char atp[3] = {0};
    bool atdetected = false;
    unsigned long atp_start = millis();
    do {
      while (!radioCtrl.available() && ((millis() - atp_start) < 5000));
      tc = radioCtrl.read();
      Serial.write(tc);
      atp[0] = atp[1];
      atp[1] = atp[2];
      atp[2] = tc;
      if (atp[0] == '+' && atp[1] == 'D' && atp[2] == 'M') atdetected = true;
    } while ((tc != '\n' || !atdetected) && ((millis() - atp_start) < 5000));
    if (atdetected) break;
  }
  Serial.println();
}

void setup_dra818() {
  radioCtrl.begin(9600);

  pinMode(PIN_RAD_PTT, OUTPUT);
  digitalWrite(PIN_RAD_PTT, HIGH);

  pinMode(PIN_RAD_EN, OUTPUT);
  wake_dra818();
  tune_dra818("144.3900");

  pinMode(PIN_RAD_PW, OUTPUT);
  digitalWrite(PIN_RAD_PW, LOW);

  pinMode(PIN_TX_AUD, INPUT);
  pinMode(PIN_RX_AUD, INPUT);

  pinMode(PIN_RAD_SQL, INPUT);

  sine_setup(400);

  // DRA818 Power: 1W
  //pinMode(PIN_RAD_PW, INPUT);

  // DRA818 Power: 0.5W
  pinMode(PIN_RAD_PW, OUTPUT);
  digitalWrite(PIN_RAD_PW, LOW);
}
