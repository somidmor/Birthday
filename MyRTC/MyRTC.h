#ifndef MyRTC_h
#define MyRTC_h

#include "RTClib.h"

class MyRTC {
public:
  MyRTC();
  void begin();
  String getFormattedDateTime();
};

#endif
