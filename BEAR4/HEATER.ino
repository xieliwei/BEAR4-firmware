void task_heater() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  if (curr_time - last_time >= 5000) {
    if (ptemp <= TEMP_SP_LOW_DEGC) {
      digitalWrite(PIN_HEATER, HIGH);
      DBGPORT.println("Heater on!");
      heaterOn = true;
    } else if (ptemp >= TEMP_SP_HIGH_DEGC) {
      digitalWrite(PIN_HEATER, LOW);
      DBGPORT.println("Heater off!");
      heaterOn = false;
    }
    last_time = curr_time;
  }

  // Safeties
  if (ptemp > 40.0) {
    digitalWrite(PIN_HEATER, LOW);
    if (heaterOn) {
      DBGPORT.println("Overtemp heater cutoff!");
    }
    heaterOn = false;
  }
  // Battery won't survive this, likely erronous reading
  if (ptemp < -50.0) {
    digitalWrite(PIN_HEATER, LOW);
    if (heaterOn) {
      DBGPORT.println("Undertemp heater cutoff!");
    }
    heaterOn = false;
  }
}

void setup_heater() {
  pinMode(PIN_HEATER, OUTPUT);
  digitalWrite(PIN_HEATER, LOW);
  heaterOn = false;
}
