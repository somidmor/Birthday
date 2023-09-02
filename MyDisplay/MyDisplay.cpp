#include "MyDisplay.h"

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C _u8g2(U8G2_R0, /* clock=*/ SCL1, /* data=*/ SDA1, /* reset=*/ U8X8_PIN_NONE);

MyDisplay::MyDisplay() {}

void MyDisplay::begin() {
  _u8g2.begin();  // Initialize the display
  _u8g2.setFont(u8g2_font_ncenB08_tr); // Set the font
}

void MyDisplay::displayString(String message) {
  _u8g2.clearBuffer();
  _u8g2.drawStr(0, 10, message.c_str());
  _u8g2.sendBuffer();
}


void MyDisplay::on() {
  // In this setup, turning "on" doesn't require special action 
  // because drawing to the display automatically turns it on.
  // However, if you had a specific command to turn on the display, you'd put it here.
}

void MyDisplay::off() {
  // Clear the display buffer
  _u8g2.clearBuffer();

  // Send the blank buffer to the display
  _u8g2.sendBuffer();
}