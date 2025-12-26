// RTC Helper Functions (from sewucombo.ino)

char* getHari() {
  return (char*)HARI[dow];
}

char* getBulan() {
  return (char*)BULAN[bln];
}

void readRTC() {
  if (!Rtc.IsDateTimeValid()) {
    return;
  }
  
  RtcDateTime now = Rtc.GetDateTime();
  jam = now.Hour();
  menit = now.Minute();
  detik = now.Second();
  tgl = now.Day();
  bln = now.Month();
  tahun = now.Year();
  dow = now.DayOfWeek();
}
