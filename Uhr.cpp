#include "Uhr.h"


RTC_DATA_ATTR int Welche = 3;
RTC_DATA_ATTR uint16_t s7 = 7000;
RTC_DATA_ATTR uint16_t s6 = 6000;
RTC_DATA_ATTR uint16_t s5 = 5000;
RTC_DATA_ATTR uint16_t s4 = 4000;
RTC_DATA_ATTR uint16_t s3 = 3000;
RTC_DATA_ATTR uint16_t s2 = 2000;
RTC_DATA_ATTR uint16_t s1 = 1000;

void Uhr::drawWatchFace() {

  display.fillScreen(GxEPD_BLACK);
  display.setTextColor(GxEPD_WHITE);

  // reset step counter at midnight
  if (currentTime.Hour == 00 && currentTime.Minute == 00) {

    s7 = s6;
    s6 = s5;
    s5 = s4;
    s4 = s3;
    s3 = s2;
    s2 = s1;
    sensor.resetStepCounter();

    //turn off radios
    WiFi.mode(WIFI_OFF);
    btStop();
  }

  //time adjustment start
  //if you don't need a time adjustment, just delete all lines between time adjustment start and end
  //my watchy runs too fast (15 seconds and 500 ms a day), this code set back time each day by 15 seconds and 500 ms

  if (currentTime.Hour == 00 && currentTime.Minute == 30) {

    RTC.read(currentTime);
    int8_t sekunde = currentTime.Second;
    int8_t minute = currentTime.Minute;
    int8_t hour = currentTime.Hour;
    int8_t day = currentTime.Day;
    int8_t month = currentTime.Month;
    int8_t year = tmYearToY2k(currentTime.Year);

    delay(15500);

    tmElements_t tm;
    tm.Month = month;
    tm.Day = day;
    tm.Year = y2kYearToTm(year);
    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = sekunde;

    RTC.set(tm);
  }
  // time adjustment end

  s1 = sensor.getCounter();
  float VBAT = getBatteryVoltage();
  uint32_t Durch;
  Durch = (s7 + s6 + s5 + s4 + s3 + s2 + s1) / 7;
  uint8_t Protz = min<int>(100 - ((4.16 - VBAT) * 100), 100);
  Protz = max<int>(Protz, 0);
  int8_t stundeA = currentTime.Hour;
  int minuteA = currentTime.Minute;
  int tagA = currentTime.Day;
  int monatA = currentTime.Month;


  if (Welche == 1) {
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(5, 58);
    if (stundeA < 10) {
      display.print("0");
    }
    display.print(stundeA);
    display.print(":");
    if (minuteA < 10) {
      display.print("0");
    }
    display.println(minuteA);

    display.setFont(&FreeSansBold15pt7b);

    String TagA = dayShortStr(currentTime.Wday);
    int b = currentTime.Day;
    String TagC = monthShortStr(currentTime.Month);
    display.setCursor(30, 90);
    display.println(TagA + " " + b + " " + TagC);

    display.drawBitmap(40, 100, akku, 30, 20, GxEPD_WHITE);
    display.setCursor(80, 120);
    display.print(Protz);
    display.print(" %");
    display.drawBitmap(40, 130, steps, 19, 23, GxEPD_WHITE);
    display.setCursor(80, 150);
    display.println(s1);
    display.drawBitmap(40, 160, xdurch, 20, 21, GxEPD_WHITE);
    display.setCursor(80, 185);
    display.println(Durch);
  } else if (Welche == 2) {
    {
      display.setFont(&FreeSansBold15pt7b);

      display.setCursor(0, 25);
      display.print("today ");
      display.println(s1);
      display.setCursor(20, 49);
      display.print("t - 1  ");
      display.println(s2);
      display.setCursor(20, 73);
      display.print("t - 2  ");
      display.println(s3);
      display.setCursor(20, 97);
      display.print("t - 3  ");
      display.println(s4);
      display.setCursor(20, 121);
      display.print("t - 4  ");
      display.println(s5);
      display.setCursor(20, 145);
      display.print("t - 5  ");
      display.println(s6);
      display.setCursor(20, 169);
      display.print("t - 6  ");
      display.println(s7);
      display.writeFastHLine(0, 175, 200, GxEPD_WHITE);

      display.drawBitmap(30, 177, xdurch, 20, 21, GxEPD_WHITE);
      display.setCursor(90, 199);
      display.println(Durch);
    }
  } else if (Welche == 3) {

    display.setFont(&bubble30pt7b);
    int16_t xc, yc;
    uint16_t wc, hc;

    int a1 = stundeA / 10;
    int a2 = stundeA - (a1 * 10);
    int a3 = minuteA / 10;
    int a4 = minuteA - (a3 * 10);


    String TagA = dayShortStr(currentTime.Wday);
    String TagC = monthShortStr(currentTime.Month);
    display.getTextBounds(TagA, 0, 0, &xc, &yc, &wc, &hc);
    display.setCursor(100 - (wc / 2), 94);
    display.println(TagA);

    display.getTextBounds(String(tagA), 0, 0, &xc, &yc, &wc, &hc);
    display.setCursor(100 - (wc / 2), 143);
    display.println(tagA);

    display.getTextBounds(TagC, 0, 0, &xc, &yc, &wc, &hc);
    display.setCursor(100 - (wc / 2), 192);
    display.println(TagC);

    if (a4 + a3 + a2 == 0) {
      bubble(0, 0);
    }
    display.setCursor(0, 45);
    display.println(a1);

    if (a4 + a3 == 0) {
      bubble(50, 0);
    }
    display.setCursor(50, 45);
    display.println(a2);

    if (a4 == 0) {
      bubble(100, 0);
    }
    display.setCursor(100, 45);
    display.println(a3);

    bubble(150, 0);
    display.setCursor(150, 45);
    display.println(a4);
  }
}

  void Uhr::bubble(uint8_t x1, uint8_t y1) {

    display.drawBitmap(x1, y1, xpeng1, 50, 50, GxEPD_WHITE);
    display.display(true);

    display.drawBitmap(x1, y1, xpeng2, 50, 50, GxEPD_WHITE);
    display.display(true);

    display.drawBitmap(x1, y1, xpeng3, 50, 50, GxEPD_BLACK);
    display.display(true);
    display.drawBitmap(x1, y1, xpeng4, 50, 50, GxEPD_BLACK);
  }


  void Uhr::handleButtonPress() {
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if (IS_DOUBLE_TAP) {
      while (!sensor.getINT()) {
      }
    }

    if (IS_BTN_RIGHT_UP) {
      vibrate();
      Welche = Welche + 1;
      if (Welche > 3) {
        Welche = 1;
      }
      RTC.read(currentTime);
      drawWatchFace();
      showWatchFace(true);
    } else if (IS_BTN_RIGHT_DOWN) {
      vibrate();
      Welche = Welche - 1;
      if (Welche < 1) {
        Welche = 3;
      }
      RTC.read(currentTime);
      drawWatchFace();
      showWatchFace(true);
    } else {
      Watchy::handleButtonPress();
    }
  }

  void Uhr::vibrate() {

    sensor.enableFeature(BMA423_WAKEUP, false);
    pinMode(VIB_MOTOR_PIN, OUTPUT);

    delay(50);
    digitalWrite(VIB_MOTOR_PIN, true);
    delay(50);
    digitalWrite(VIB_MOTOR_PIN, false);

    sensor.enableFeature(BMA423_WAKEUP, true);
  }
