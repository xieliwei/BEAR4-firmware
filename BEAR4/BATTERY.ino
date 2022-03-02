void task_battery() {
  unsigned long curr_time = millis();
  static unsigned long last_time = 0;

  String tstr;
  if (curr_time - last_time >= 5000) {
    uint32_t C1accum = 0, BVaccum = 0;
    for (uint16_t i = 0; i < BATT_AVG_LEN; i++) {
      C1accum += analogRead(ADC_CELL1);
      BVaccum += analogRead(ADC_CELL2);
    }
    Cell1 = (float)C1accum / BATT_AVG_LEN * 0.00118461538461538461538461538462;
    BV = (float)BVaccum / BATT_AVG_LEN * 0.00252045826513911620294599018003;
    Cell2 = BV - Cell1;
    tstr += String(BV);
    tstr += '\t';
    tstr += String(Cell1);
    tstr += '\t';
    tstr += String(Cell2);
    DBGPORT.println(tstr);

    last_time = curr_time;
  }
}

void setup_battery() {
  pinMode(ADC_CELL1, INPUT);
  pinMode(ADC_CELL2, INPUT);
}
