void task_heater() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;
  static bool heater_state = false;

  bool last_heater_state = heater_state;

  // Update state only, we don't want to pre-maturely
  // turn heater on when there is possibly a safety
  // cutoff about to be triggered after this.
  if (curr_time - last_time >= 5000) {
    if (btemp <= TEMP_SP_LOW_DEGC) {
      heater_state = true;
      heaterOn = true;
    } else if (btemp >= TEMP_SP_HIGH_DEGC) {
      heater_state = false;
      heaterOn = false;
    }
    last_time = curr_time;
  }

  // Safeties
  if (btemp > 50.0) {
    digitalWrite(PIN_HEATER, LOW);
    heater_state = false;
    if (heaterOn) {
      DBGPORT.println("Overtemp heater cutoff!");
    }
    heaterOn = false;
  }
  // Battery likely won't have any capacity left when we've
  // been up long enough to reach -20degC. Don't bother using
  // heater as the voltage sag might kill us.
  // Battery won't survive -50degC, likely erronous reading
  if (ptemp < -20.0 || ptemp < -50.0) {
    digitalWrite(PIN_HEATER, LOW);
    heater_state = false;
    if (heaterOn) {
      DBGPORT.println("Undertemp heater cutoff!");
    }
    heaterOn = false;
  }

  // Actually turn on/off heater if safeties not triggered
  if (heater_state) {
    digitalWrite(PIN_HEATER, HIGH);
    if (last_heater_state != heater_state) {
      DBGPORT.println("Heater on!");
    }
  } else {
    digitalWrite(PIN_HEATER, LOW);
    if (last_heater_state != heater_state) {
      DBGPORT.println("Heater off!");
    }
  }
}

void setup_heater() {
  pinMode(PIN_HEATER, OUTPUT);
  digitalWrite(PIN_HEATER, LOW);
  heaterOn = false;
}
