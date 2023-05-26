void task_sensors() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  if (curr_time - last_time >= 5000) {
    //int32_t ax = 0, ay = 0, az = 0;
    /*
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
    */
    //imu sensor events
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    //RAW ACCEL VALUES(M/S^2)
    float ax = a.acceleration.x;
    float ay = a.acceleration.y;
    float az = a.acceleration.z;

    accel_x = ax;
    accel_y = ay;
    accel_z = az;

    //Serial.print(ax);
    //Serial.print(" ");
    //Serial.print(ay);
    //Serial.print(" ");
    //Serial.println(az);

    //RAW GYRO VALUES (RAD/S)+/- zero rate offset
    //float gx = g.gyro.x - 0.0090;
    //float gy = g.gyro.y - 0.0377;
    //float gz = g.gyro.z + 0.0074;

    //gyro_x=gx;
    //gyro_y=gy;
    //gyro_z=gz;
  


    bmp388.getTempPres(ptemp, ppress);
    paltitudeMSL = ((float)powf((local_QNH) / ppress, 0.190223f) - 1.0f) * (ptemp + 273.15f) / 0.0065f;  // Calculate the altitude in metres
    /*
    if (millis() > ow_lastconv + sensors.millisToWaitForConversion(OW_SENSOR_RES)) {
      btemp = sensors.getTempC(deviceAddress);
      sensors.requestTemperatures(); // Send the command to get temperatures
      DBGPORT.print(btemp);
      DBGPORT.println(F("*C "));
      ow_lastconv = millis();
    }
    */
  }
}

void setup_sensors() {


  
 

///////////////////////////////////////////////////////////////////////////////

  //setup i2c sensors
  pinMode(PIN_I2C_SDA, INPUT);
  pinMode(PIN_I2C_SCL, INPUT);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  bmp388.begin();
  bmp388.setTimeStandby(TIME_STANDBY_1280MS);
  bmp388.startNormalConversion();
  /*  
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setIntDataReadyEnabled(true);
  accelgyro.setIntFreefallEnabled(true);
  accelgyro.setIntEnabled(true);
*/
  //initialise IMU
  //This is a ternary operator ((condition) ? (result if true) : (result if false))
  Serial.println(mpu.begin() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
  int i = 0;
  while ((!sensors.getDS18Count() || !sensors.isParasitePowerMode()) && (millis() < 5000)) {
    Serial.print(++i);
    Serial.print(" ");
    sensors.begin();
  }
  Serial.println();
  while (!sensors.getAddress(deviceAddress, 0) && (millis() < 5000))
    ;
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
