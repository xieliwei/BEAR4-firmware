void task_sensors() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  if (curr_time - last_time >= 5000) {
    int32_t ax = 0, ay = 0, az = 0;

    bmp388.getMeasurements(ptemp, ppress, paltitudeMSL);
    DBGPORT.print(ptemp);
    DBGPORT.print(F("*C "));
    DBGPORT.print(ppress);
    DBGPORT.print(F("hPa "));
    DBGPORT.print(paltitudeMSL);
    DBGPORT.println(F("m"));

    DBGPORT.print(F("Accel "));
    bool did_free_fall = false;
    for (uint16_t i = 0; i < SEN_AVG_LEN; i++) {
      int16_t tax, tay, taz;
      uint8_t s;
      do {
        s = accelgyro.getIntStatus();
        if (!did_free_fall) {
          did_free_fall = s & (1 << MPU6050_INTERRUPT_FF_BIT);
        }
      } while (!(s & (1 << MPU6050_INTERRUPT_DATA_RDY_BIT)) && ((millis() - curr_time) < 1000));
      accelgyro.getAcceleration(&tax, &tay, &taz);
      ax += tax;
      ay += tay;
      az += taz;
    }
    if (!freefall) freefall = did_free_fall;
    accel_x = ax / SEN_AVG_LEN;
    accel_y = ay / SEN_AVG_LEN;
    accel_z = az / SEN_AVG_LEN;
    DBGPORT.print(accel_x);
    DBGPORT.print(F(" "));
    DBGPORT.print(accel_y);
    DBGPORT.print(F(" "));
    DBGPORT.print(accel_z);
    DBGPORT.print(F(" "));
    DBGPORT.print(freefall);
    DBGPORT.println();

    if (millis() > ow_lastconv + sensors.millisToWaitForConversion(OW_SENSOR_RES)) {
      btemp = sensors.getTempC(deviceAddress);
      sensors.requestTemperatures(); // Send the command to get temperatures
      DBGPORT.print(btemp);
      DBGPORT.println(F("*C "));
      ow_lastconv = millis();
    }

    last_time = curr_time;
  }
}

void setup_sensors() {
  pinMode(PIN_I2C_SDA, INPUT);
  pinMode(PIN_I2C_SCL, INPUT);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  bmp388.begin();
  bmp388.setTimeStandby(TIME_STANDBY_1280MS);
  bmp388.startNormalConversion();

  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setIntDataReadyEnabled(true);
  accelgyro.setIntFreefallEnabled(true);
  accelgyro.setIntEnabled(true);

  int i = 0;
  while ((!sensors.getDS18Count() || !sensors.isParasitePowerMode()) && (millis() < 5000)) {
    Serial.print(++i);
    Serial.print(" ");
    sensors.begin();
  }
  Serial.println();
  while (!sensors.getAddress(deviceAddress, 0) && (millis() < 5000));
  Serial.print("Onewire sensor: ");
  for (i = 0; i < sizeof(deviceAddress); i++) {
    Serial.print(deviceAddress[i], HEX);
  }
  Serial.println();
  sensors.setResolution(deviceAddress, OW_SENSOR_RES, false);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  ow_lastconv = millis();
}
