void setup() {
  setup_watchdog();
  //delay(3000);

  // Bug fix, pullup DTR so we can boot properly if we reset
  pinMode(PIN_DTR, INPUT_PULLUP);

  setup_heater();

  Serial.begin(115200);

  Serial.println("Wifi");
  setup_wifi();

  Serial.println("RTC");
  setup_rtc();

  Serial.println("Battery");
  setup_battery();

  //i2c_scanner();

  Serial.println("Sensors");
  setup_sensors();
  Serial.println("DRA818");
  setup_dra818();
  Serial.println("GPS");
  setup_gps();
  Serial.println("CAMERA");
  setup_camera();
  Serial.println("SSTV");
  setup_sstv();
  // Will transmit boot message, ensure dra818 is ready
  Serial.println("APRS");
  setup_aprs();

  // Unused pins for expansion ports
  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWrite(PIN_SPI_CS, HIGH);
  pinMode(PIN_SPI_CLK, INPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_MOSI, INPUT);

  pinMode(PIN_ONEWIRE, INPUT);

  Serial.println("Bootup complete");
  tickle_watchdog();
}

void i2c_scanner() {
  DBGPORT.println();
  DBGPORT.println("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      DBGPORT.print("Found address: ");
      DBGPORT.print(String(i, DEC));
      DBGPORT.print(" (0x");
      DBGPORT.print(String(i, HEX));
      DBGPORT.println(")");
      count++;
    }
  }
  DBGPORT.print("Found ");
  DBGPORT.print(String(count, DEC));
  DBGPORT.println(" device(s).");
}
