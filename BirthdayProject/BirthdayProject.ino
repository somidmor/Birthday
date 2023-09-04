#include "MyRTC.h"
#include "MyDisplay.h"
#include "BluefruitSerial.h"

MyRTC rtc;
MyDisplay display;
BluefruitSerial bluefruit;

#define BUTTON_PIN 53
#define DEBOUNCE_TIME 50
#define DISPLAY_TIME 5000

unsigned long lastDebounceTime = 0;
bool buttonState = HIGH;
unsigned long displayOnTime = 0;

enum States {
    WAIT_FOR_BUTTON,
    DISPLAY_DATETIME,
    DISPLAY_TIMEOUT
};

struct stateStatus {
    States next;
    States current;
    States previous;
};

stateStatus gState = {
    .next = States::WAIT_FOR_BUTTON,
    .current = States::WAIT_FOR_BUTTON,
    .previous = States::WAIT_FOR_BUTTON
};

void setup() {
  Serial.begin(57600);
  rtc.begin();
  display.begin();
  //bluefruit.begin();
  bluefruit.setDeepSleep();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  display.off();
}

void loop() {
  switch(gState.current) {
    case WAIT_FOR_BUTTON:
      waitForButton();
      break;
    case DISPLAY_DATETIME:
      displayDateTime();
      break;
    case DISPLAY_TIMEOUT:
      handleDisplayTimeout();
      break;
  }

  gState.previous = gState.current;
  gState.current = gState.next;
  delay(50);
}

void waitForButton() {
  bool rawState = digitalRead(BUTTON_PIN);
  if (rawState == LOW) {
    if (millis() - lastDebounceTime > DEBOUNCE_TIME) {
      buttonState = !buttonState;
      if (buttonState == LOW) {
        displayOnTime = millis();
        gState.next = States::DISPLAY_DATETIME;
      }
    }
    lastDebounceTime = millis();
  }
}

void displayDateTime() {
  //String dateTime = rtc.getCurrentDateTime();
  String dateTime = rtc.getFormattedDateTime();
  display.on();
  display.displayString(dateTime);
  Serial.println(dateTime);
  if (millis() - displayOnTime > DISPLAY_TIME) {
    gState.next = States::DISPLAY_TIMEOUT;
  }
}

void handleDisplayTimeout() {
  display.off();
  gState.next = States::WAIT_FOR_BUTTON;
}
