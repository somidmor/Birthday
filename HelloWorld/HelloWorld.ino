#include <Wire.h>
#include <U8g2lib.h>
#include "RTClib.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL1, /* data=*/ SDA1, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(57600);
  
  rtc.begin();
  rtc.initialized();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  u8g2.begin();  // Initialize the display
  u8g2.setFont(u8g2_font_ncenB08_tr); // Set the font
}

void loop() {
  DateTime now = rtc.now();
  
  u8g2.clearBuffer();                   // clear the internal memory
  
  // Create time and date string
  char buf[50];
  snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d", 
    now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  
  u8g2.drawStr(0, 10, buf);             // draw date and time string
  u8g2.sendBuffer();                    // transfer internal memory to the display
  
  // Print to Serial (if you still need this)
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
  
  delay(1000);  // delay for 3 seconds
}
