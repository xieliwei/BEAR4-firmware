void task_heater() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;
  static bool heater_state = false;

  bool last_heater_state = heater_state;

  // Check for cut condition
  if (galtitudeMSL > heater_cut_height_m || paltitudeMSL > heater_cut_height_m) {
    if (timeAltAboveHeaterCut_ms == 0) {
      timeAltAboveHeaterCut_ms = curr_time;
    }
  } else {
    timeAltAboveHeaterCut_ms = 0;
  }

  // Update state only, we don't want to pre-maturely
  // turn heater on when there is possibly a safety
  // cutoff about to be triggered after this.
  if (curr_time - last_time >= 1000) {
    if (cut_now || (timeAltAboveHeaterCut_ms != 0 && (curr_time - timeAltAboveHeaterCut_ms) > heater_cut_height_hysteresis_time_ms)) {
      cut_now = false;
      heater_state = true;
      heaterOn = true;
    }
    last_time = curr_time;
  }

  // Safeties
  if (timeHeaterCutOn_ms != 0 && (curr_time - timeHeaterCutOn_ms) > heater_cut_time_ms) {
    digitalWrite(PIN_HEATER, LOW);
    heater_state = false;
    if (heaterOn) {
      DBGPORT.println("Heater overtime cutoff!");
    }
    heaterOn = false;

    // Reset entire cut state
    // This allows us to retry if the cut does not work
    // Should be safe as this will be cycled by heater_cut_height_hysteresis_time_ms
    timeAltAboveHeaterCut_ms = 0;
    timeHeaterCutOn_ms = 0;
  }

  // Actually turn on/off heater if safeties not triggered
  if (heater_state) {
    if (timeHeaterCutOn_ms == 0) {
      timeHeaterCutOn_ms = curr_time;
    }
    digitalWrite(PIN_HEATER, HIGH);
    if (last_heater_state != heater_state) {
      DBGPORT.println("Heater on!");
    }
  } else {
    timeHeaterCutOn_ms = 0;
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
  timeAltAboveHeaterCut_ms = 0;
  timeHeaterCutOn_ms = 0;
}
