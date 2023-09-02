#include "MyRTC.h"
#include "MyDisplay.h"

MyRTC rtc;
MyDisplay display;

#define BUTTON_PIN 53
#define DEBOUNCE_TIME 50    // Time in milliseconds
#define DISPLAY_TIME 5000   // Time display is on in milliseconds

unsigned long lastDebounceTime = 0;  // Last time the button pin was toggled
bool buttonState = HIGH;             // Current debounced button state
unsigned long displayOnTime = 0;     // Time when display was turned on
bool displayActive = false;          // To check if display is currently on

void setup() {
  Serial.begin(57600);
  rtc.begin();
  display.begin();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set the button pin as input with pull-up resistor
  
  display.off();  // Turn off the display at the start
}

void loop() {
  // Check for button press with debounce
  bool rawState = digitalRead(BUTTON_PIN);
  if (rawState == LOW) {  // If the button is pressed (LOW because of pull-up resistor)
    if (millis() - lastDebounceTime > DEBOUNCE_TIME) {
      buttonState = !buttonState;  // Toggle the button state
      if (buttonState == LOW) {    // If the button was pressed
        display.on();             // Turn the display on
        displayActive = true;
        displayOnTime = millis();
      }
    }
    lastDebounceTime = millis();
  }

  // If display is active, show the datetime
  if (displayActive) {
    String dateTime = rtc.getFormattedDateTime();
    display.displayString(dateTime);
    Serial.println(dateTime);
  }

  // Turn off the display after DISPLAY_TIME
  if (displayActive && (millis() - displayOnTime > DISPLAY_TIME)) {
    display.off();
    displayActive = false;
  }
}
