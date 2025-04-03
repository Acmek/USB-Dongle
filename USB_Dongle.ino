#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

/* 
// You'll need to find the folder where your ch55xduino
// library files are located
*/
#include "HIDKeyboardMouse/USBHIDKeyboardMouse.h"

// Pins match with included PCB
#define LED 14
#define BUTTON 11

#define flashLED(ms) digitalWrite(LED, HIGH); delay(ms); digitalWrite(LED, LOW)

// Used to read button presses
bool lastButtonState;
bool pressed;

// Used to flash the LED ON and OFF
uint32_t flashOnTime = 200;
uint32_t flashOffTime = 200;

/*
// Used to save the previous ON and OFF state
// of the board
*/
bool isOn;
uint8_t writePtr = 0;
uint8_t writeData = 0;

void flashOn();
void readButton();
void onTurnON();
void onTurnOFF();
void doAction();


// ----- WRITE DESIRED ACTIONS HERE -----
#define setPauseTimer() pauseTimer = millis() + pauseTime
int moveAmount = 40;
unsigned long pauseTime = 1000;
unsigned long pauseTimer;

// Executes every loop if ON
void doAction() {
  if (millis() >= pauseTimer) {
    Mouse_move(0, moveAmount);
    moveAmount *= -1;

    setPauseTimer();
  }
}

// Executes after LEDs indicate ON
void onTurnON() {
  setPauseTimer();
}

// Executes before LEDs indicate OFF
void onTurnOFF() {}


//----- OTHER CODE LOGIC -----
void setup() {
  // put your setup code here, to run once:
  USBInit();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUTTON, INPUT_PULLUP);

  // Read previous ON state from EEPROM (address 0)
  isOn = (eeprom_read_byte(0) != 0) ? true : false;

  // Flash LED to indicate initial state
  flashOn();
}

void loop() {
  // put your main code here, to run repeatedly:
  readButton();

  // Turn ON and OFF dongle
  if (pressed) {
    isOn = !isOn;

    // Write state to EEPROM
    eeprom_write_byte(0, isOn ? 1 : 0);

    flashOn();

    pressed = false;
  }

  if (isOn) {
    doAction();
  }
}

void flashOn() {
  /*
  // Flash TWICE if turning ON,
  // Flash ONCE if turning OFF
  */
  if (isOn) {
    flashLED(flashOnTime);
    delay(flashOffTime);
    flashLED(flashOnTime);

    onTurnON();
  }
  else {
    onTurnOFF();

    flashLED(flashOnTime);
  }
}

void readButton() {
  // PULLUP (1 when not pressed) so flip
  bool input = !digitalRead(BUTTON);

  // Button state as changed so it was either pressed or released
  if (input != lastButtonState) {
    // Debounce
    delay(10);

    // Check if button is still different
    if (input != lastButtonState) {
      // Button was pressed
      if (input) {
        pressed = true;
      }

      lastButtonState = input;
    }
  }
}