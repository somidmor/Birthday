#include "MyRTC.h"

RTC_PCF8523 _rtc;

MyRTC::MyRTC() {}

void MyRTC::begin() {
  _rtc.begin();
  _rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  _rtc.initialize(); // Added this line to initialize the RTC
}

String MyRTC::getFormattedDateTime() {
    DateTime now = _rtc.now();
    DateTime startDate(2022, 2, 14);

    long secondsElapsed = now.unixtime() - startDate.unixtime();
    DateTime difference(2000, 1, 1, 0, 0, secondsElapsed); // Temporarily use a placeholder epoch date

    int years = difference.year() - 2000;
    int months = difference.month() - 1;
    int days = difference.day() - 1;
    int hours = difference.hour();
    int minutes = difference.minute();
    int seconds = difference.second();

    char buf[200]; // Increased buffer size to accommodate the new format
    snprintf(buf, sizeof(buf), "%d year%s, %d month%s, %d day%s, %d hour%s, %d minute%s and %d second%s", 
        years, years == 1 ? "" : "s",
        months, months == 1 ? "" : "s",
        days, days == 1 ? "" : "s",
        hours, hours == 1 ? "" : "s",
        minutes, minutes == 1 ? "" : "s",
        seconds, seconds == 1 ? "" : "s"
    );

    return String(buf);
}

