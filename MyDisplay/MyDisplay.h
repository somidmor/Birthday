#ifndef MyDisplay_h
#define MyDisplay_h

#include <U8g2lib.h>

class MyDisplay {
public:
  MyDisplay();
  void begin();
  void displayString(String message);
  void on();
  void off();
};

#endif
