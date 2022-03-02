void setup_rtc() {
  tyear = todays_date[0];
  tmonth = todays_date[1];
  tday = todays_date[2];
  thour = todays_time[0];
  tmin = todays_time[1];
  tsec = todays_time[2];
  tms = 0;
  tsync = millis();
}

// TODO - Make this interrupt-safe
void task_rtc() {
  unsigned long long curr_time = millis();
  static unsigned long long last_time = 0;

  if (curr_time - last_time >= 250) {
    last_time = curr_time;
    
    unsigned long long tdiff = curr_time - tsync;
    uint8_t numdays;

    tsync = curr_time;

    tdiff += tms;
    tms = tdiff % 1000;
    tdiff /= 1000;

    if (!tdiff) return;

    tdiff += tsec;
    tsec = tdiff % 60;
    tdiff /= 60;

    if (!tdiff) return;

    tdiff += tmin;
    tmin = tdiff % 60;
    tdiff /= 60;

    if (!tdiff) return;

    tdiff += thour;
    thour = tdiff % 24;
    tdiff /= 24;

    if (!tdiff) return;

    tdiff += tday;
    switch (tmonth) {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
        numdays = 31;
        break;
      case 4:
      case 6:
      case 9:
      case 11:
      default:
        numdays = 30;
        break;
      case 2:
        numdays = is_leap_year ? 29 : 28;
        break;
    }
    tday = tdiff % (numdays + 1);
    if (!tday) tday = 1;
    tdiff /= (numdays + 1);

    if (!tdiff) return;

    tdiff += tmonth;
    tmonth = tdiff % 13;
    if (!tmonth) tmonth = 1;
    tdiff /= 13;

    if (!tdiff) return;

    // If we reached here, happy new year!
    tyear++;
    return;
  }
}
