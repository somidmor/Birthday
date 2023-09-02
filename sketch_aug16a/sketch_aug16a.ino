#include <Wire.h>
#include <U8g2lib.h>
#include <WaveGenerator.h>
#include <Button.h>
#include "global.h"
#include <BluefruitSerial.h>


// user input format:
// userFrequency: 1-13KHz, it is the userFrequency of the wave
// groundHeight: 0-1690, it is the ref height of the wave between blocks of waves (it is the height of the wave when it is not generating a wave)
// height1: -1690-1690, it is the height of the wave for the first portion
// portion1: 0-10, it is the portion of the wave for the first portion
// height2: -1690-1690, it is the height of the wave for the second portion
// portion2: 0-10, it is the portion of the wave for the second portion
// height3: -1690-1690, it is the height of the wave for the third portion
// portion3: 0-10, it is the portion of the wave for the third portion
// height4: -1690-1690, it is the height of the wave for the fourth portion
// portion4: 0-10, it is the portion of the wave for the fourth portion
// numWaves: 0-1000, it is the number of waves to generate, if it is 0, it will generate waves indefinitely
// delayBetweenBlocks: it is the delay between each block of waves in microseconds
// numBlock: 0-1000, it is the number of blocks of waves to generate, if it is 0, it will generate blocks indefinitely
/* isAM: 0-1690, it is the amplitude modulation of the wave, if it is 0, it will not generate amplitude modulation
    if greater than 0 it will generate amplitude modulation, the value is the amplitude of the modulation each steps */
/*AMSclae: it is the scale of the amplitude modulation, if it is 0, it will not generate amplitude modulation
    if not 0 it will generate amplitude modulation, the value is the scale of the modulation 
    it syncs the first height with the third height so always height1/height3 = AMScale even negative*/
/* isFM: 0-1690, it is the frequency modulation of the wave, if it is 0, it will not generate frequency modulation
    if greater than 0 it will generate frequency modulation, the value is the frequency of the modulation each steps */
/* maxFrequency: 0-13KHz, it is the maximum frequency of the wave during frequency modulation */
/* isBDM(Block Delay Modulation): 0-1690, it is the block delay modulation of the wave, if it is 0, it will not generate block delay modulation
    if greater than 0 it will generate block delay modulation, the value is the delay of the modulation each steps */
/* minBlockDelay: it is the minimum delay between each block of waves in microseconds */

enum States {
    INIT,
    GET_USER_INPUT,
    PARSE_USER_INPUT,
    HANDLE_USER_INPUT,
    GENERATE_WAVES,
    USER_INPUT_ERROR_STATE,
};

struct stateStatus {
    States next;
    States current;
    States previous;
};

stateStatus gState = {
    .next = States::INIT,
    .current = States::INIT,
    .previous = States::INIT
};

/*******************OLED Section***************************/
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

const int menuItems = 10;
int currentMenuItem = 0;
String menu[menuItems] = {"Option 1", "Option 2", "Option 3", "Option 4", "Option 5", "Option 6", "Option 7", "Option 8", "Option 9", "Option 10"};
String optionParams[menuItems] = {
  /*option1*/ "100,0,1690,5,-1690,5,0,0,0,0,0,0,0,0,0,0,0,0,0",
  /*option2*/ "1000,0,1690,5,-1690,5,0,0,0,0,0,0,0,0,0,1,5000,0,0",
  /*option3*/ "1000,0,1690,4,0,2,-1690,4,0,0,1,10000,0,0,0,0,0,100,1000",
  /*option4*/ "500,0,1690,4,0,2,0,4,0,0,0,0,0,1,-2,0,0,0,0",
  /*option5*/ "1000,0,1690,-1,-1690,-1,800,-1,800,-1,0,0,0,0,0,0,0,0,0",
  /*option6*/ "100,0,1690,5,-1690,5,0,0,0,0,0,0,0,0,0,0,0,0,0",
  /*option7*/ "100,0,1690,5,-1690,5,0,0,0,0,0,0,0,0,0,0,0,0,0",
  /*option8*/ "1000,0,1690,-1,-1690,-1,800,-1,800,-1,0,0,0,0,0,0,0,0,0",
  /*option9*/ "2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2",
  /*option10*/ "1000,0,1690,4,0,2,-1690,4,0,0,1,10000,0,0,0,0,0,100,1000"
};
/*******************OLED Section end***********************/


/******************Button Section**********************/
Button stopButton(36);
Button menuUpButton(37);
Button menuDownButton(38);
Button selectButton(39);
/******************Button Section end******************/


BluefruitSerial blueSerial; // Uses the default pin numbers for the Adafruit Feather 32u4 Bluefruit LE
WaveGenerator generator;
String input; // String to store the user input

void setup() {
  stopButton.setHandler(handleStopButtonClick);
  menuUpButton.setHandler(handleMenuUpButtonClick);
  menuDownButton.setHandler(handleMenuDownButtonClick);
  selectButton.setHandler (handleSelectButtonClick);
  initButtons();
  Serial.begin(115200); // Begin serial communication at 9600 baud
  blueSerial.begin(); // Begin bluetooth communication
  analogWriteResolution(12); // Set resolution of the voltage from 0 to 4095
  u8g2.begin();  // Initialize the display
  u8g2.setFont(u8g2_font_ncenB08_tr); // Set the font
  gState.current = States::INIT; // Initialize the first state
  analogWrite(DAC1, 2047); //base voltage on DAC1 for refrence
}

void loop() {
   // Check if any button was pressed
  switch(gState.current) {
    case INIT:
      Serial.println("Enter parameters: ");
      gState.next = States::GET_USER_INPUT;
      break;

    case GET_USER_INPUT:
      handleUserInput();
      checkButtons();
      break;

    case PARSE_USER_INPUT:
      
      generator.setInputs(input.c_str());
      gState.next = States::GENERATE_WAVES;
      break;

    case GENERATE_WAVES:
      generator.generateWaves(checkButtons);
      gState.next = States::GET_USER_INPUT;
      break;

    case USER_INPUT_ERROR_STATE:
      // for later
      break;
  }

  gState.previous = gState.current;
  gState.current = gState.next;
  delay(50);
}


/*************************Button Functions************************/
void handleStopButtonClick() {
  //Serial.println("stop");
  generator.stop();
}

void handleMenuUpButtonClick() {
  //Serial.println("up");
  if (currentMenuItem > 0) {
    currentMenuItem--;
  }
}

void handleMenuDownButtonClick() {
  //Serial.println("down");
  if (currentMenuItem < menuItems - 1) {
    currentMenuItem++;
  }
}

void handleSelectButtonClick() {
  //Serial.println("select");
  input = optionParams[currentMenuItem];
  gState.next = States::PARSE_USER_INPUT;
}

void checkButtons() {
  stopButton.wasPressed();
  menuUpButton.wasPressed();
  menuDownButton.wasPressed();
  selectButton.wasPressed();
}

void initButtons() {
  stopButton.init();
  menuUpButton.init();
  menuDownButton.init();
  selectButton.init();
}
/*************************Button Functions end************************/



/*************************OLED Section************************/
void handleUserInput() {
  if (blueSerial.validData()) {
    input = blueSerial.currentData;
    Serial.print(input);
    if (input == "r\n" || input == "R\n") {
      generator.stop();
    } else if (input == "w\n"){
      Serial.println("w");
      handleMenuUpButtonClick();
    } else if (input == "s\n"){
      Serial.println("s");
      handleMenuDownButtonClick();
    } else if (input == "f\n"){
      Serial.println("f");
      handleSelectButtonClick();
    } else if (input.charAt(0) == 'O') {
      Serial.println("Option selection");
      generator.stop();
      gState.next = States::PARSE_USER_INPUT;
      select_option_number(input);
    } else if (input.charAt(0) == 'p'){
      Serial.println("Option Select ");
      update_option_params(input);
    } else {
       generator.stop();
      gState.next = States::GET_USER_INPUT;
    }
  }
  drawMenu();  // Draw the menu on the display
}

void drawMenu() {
  u8g2.clearBuffer();
  int start = currentMenuItem - 1;
  if (start < 0) start = 0;

  for (int i = start; i < min(start + 3, menuItems); i++) {
    if (i == currentMenuItem) {
      u8g2.drawStr(0, (i - start + 1) * 10, (char*) menu[i].c_str());
    } else {
      u8g2.drawStr(10, (i - start + 1) * 10, (char*) menu[i].c_str());
    }
  }

  u8g2.sendBuffer();  // Update the display
}
/*************************OLED Section end************************/




/*************************GUI communications************************/
void update_option_params(String received) {
    // Find indices of commas
    Serial.println(received);
    int firstCommaIndex = received.indexOf(',');
    int secondCommaIndex = received.substring(firstCommaIndex + 1).indexOf(',')  + firstCommaIndex + 1;
    // Serial.println(received);
    // Serial.println(firstCommaIndex);
    // Serial.println(secondCommaIndex);    

    // Error checking
    if (firstCommaIndex == -1 || secondCommaIndex == -1 || firstCommaIndex == secondCommaIndex) {
        // //Serial.println("Invalid message format");
        return;
    }

    String py = received.substring(0, firstCommaIndex);
    //Serial.println(py);
    
    // More error checking
    if(py != "py") {
        // //Serial.println("Invalid prefix");
        return;
    }
    
    int index = received.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
    index--; // Decrement user input index to match array index
    String new_value = received.substring(secondCommaIndex + 1);
    //Serial.println(new_value);
    
    // Further error checking
    if(index >= 0 && index < menuItems) {
        optionParams[index] = new_value;
    } else {
        // //Serial.println("Invalid index");
    }
    //Serial.println(optionParams[index]);
    gState.next = States::GET_USER_INPUT;
    generator.stop();
    // blueSerial.println("Successfull updated option"); 
    //blueSerial.println(optionParams[index]);
}

// get an input with the format "option x" where x is the option number and set the input to the option
void select_option_number(String received) { // Fixed typo in function name
  int spaceIndex = received.indexOf(' ');
  if (spaceIndex == -1) {
    //Serial.println("Invalid message format");
    return;
  }
  int index = received.substring(spaceIndex + 1).toInt();
  if (index > 0 && index <= menuItems) {
    input = optionParams[index-1];
    currentMenuItem = index-1;
  } else {
    //Serial.println("Invalid index");
  }
}