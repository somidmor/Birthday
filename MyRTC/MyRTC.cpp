#include "MyRTC.h"

RTC_PCF8523 _rtc;

MyRTC::MyRTC() {}

void MyRTC::begin() {
  _rtc.begin();
  //_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  _rtc.initialized(); // Added this line to initialize the RTC
}

String MyRTC::getFormattedDateTime() {
    DateTime now = _rtc.now();
    DateTime startDate(2022, 2, 14);
    
    int years, months, days, hours, minutes, seconds;

    seconds = now.second() - startDate.second();
    minutes = now.minute() - startDate.minute();
    hours = now.hour() - startDate.hour();
    days = now.day() - startDate.day();
    months = now.month() - startDate.month();
    years = now.year() - startDate.year();

    if (seconds < 0) {
        seconds += 60;
        minutes--;
    }

    if (minutes < 0) {
        minutes += 60;
        hours--;
    }

    if (hours < 0) {
        hours += 24;
        days--;
    }

    if (days < 0) {
        // Borrow from month, adjusting for variable month lengths
        months--;
        if (startDate.month() == 2) {
            days += 28 + (startDate.year() % 4 == 0); // Leap year check
        } else if (startDate.month() == 4 || startDate.month() == 6 || startDate.month() == 9 || startDate.month() == 11) {
            days += 30;
        } else {
            days += 31;
        }
    }

    if (months < 0) {
        months += 12;
        years--;
    }

    char buf[200];
    snprintf(buf, sizeof(buf), 
        "%d years, %d months\n"
        "%d days, %d hours\n"
        "%d minutes, %d seconds", 
        years, months, days, hours, minutes, seconds
    );

    return String(buf);
}

String MyRTC::getCurrentDateTime() {
    DateTime now = _rtc.now();
    
    char buf[40];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d\n%02d:%02d:%02d", 
        now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

    return String(buf);
}

