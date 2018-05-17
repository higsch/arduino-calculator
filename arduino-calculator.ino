/*
 * Calculator for Arduino
 * (initially started for the pay desk of my little daughter Madita)
 * @author Matthias Stahl
 * partly based on https://circuitdigest.com/microcontroller-projects/arduino-calculator-using-4x4-keypad
 * 
 */
#include <LedControl.h>
#include <Key.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 18
#define RST_PIN 19

// RFID reader
MFRC522 rfid(SDA_PIN, RST_PIN);

// Buzzer
const int buzzer = 14;

// Keypad definitions
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns

// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = { 9, 8, 2, 3 }; // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins. // TODO: change pin 8->1 and 9->0, when Serial.begin() is out
byte colPins[COLS] = { 4, 5, 6, 7 }; // Connect keypad COL0, COL1 and COL2 to these Arduino pins.

// 7-segment display setup
LedControl lc = LedControl(15, 16, 17, 1);

// Some other stuff
long num1, num2, number;
char key, action;
boolean result = false;

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); //  Create the Keypad

void setup() {
  //Serial.begin(9600);
  SPI.begin();

  rfid.PCD_Init();

  // Connect display
  lc.clearDisplay(0);
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);

  // Connect buzzer
  pinMode(buzzer, OUTPUT);
  beep();

  printMadita();
  delay(3000);
  lc.clearDisplay(0);
  lc.setDigit(0, 0, 0, false);
}

void loop() {
  
  key = kpd.getKey();
  
  if (key != NO_KEY) {
    DetectButtons();
  }
  
  if (result == true)
    CalculateResult();
    result = false;

  if (number > 0) {
    if (rfid.PICC_IsNewCardPresent()) {
      reset();
      beep();
    }
  }
}

void beep() {
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
}

void reset() {
  number = num1 = num2 = 0;
  result = false;
  printOnDisplay();
}

void printMadita() {
  lc.setRow(0, 6, 118);
  lc.setRow(0, 5, 119);
  lc.setDigit(0, 4, 0, false);
  lc.setDigit(0, 3, 1, false);
  lc.setDigit(0, 2, 7, false);
  lc.setRow(0, 1, 119);
}

void printOnDisplay() {
  String data = String(number);

  if (data.length() > 8) {
    if (result == true) {
      printErrOnDisplay();
      number = 0;
    } else {
      number = data.substring(0, 8).toInt();
    }
  } else {
    lc.clearDisplay(0);
    int i;
    for (i = 0; i <= data.length() - 1; i++) {
      lc.setChar(0, i, byte(data.charAt(data.length() - i - 1)), false);
    }
  }
}

void printErrOnDisplay() {
  action = NULL;
  lc.clearDisplay(0);
  lc.setRow(0, 4, 79);
  lc.setRow(0, 3, 5);
  lc.setRow(0, 2, 5);
  lc.setRow(0, 1, 29);
  lc.setRow(0, 0, 5);
}

void CalculateResult() {

  if (action == '+')
    number = num1 + num2;
  if (action == '-')
    number = num1 - num2;
  if (action == '*')
    number = num1 * num2;
  if (action == '/')
    if (num2 == 0) {
      printErrOnDisplay();
      number = num1 = num2 = 0;
      return;
    } else {
      number = num1 / num2;
    }

  if (number < 0) // no negative results
    number = 0;
    
  printOnDisplay();
  num1 = num2 = 0;
  
}

void DetectButtons() {

  if (key == '1') {
    if (number == 0)
      number = 1;
    else
      number = (number * 10) + 1;
  }

  if (key == '4') {
    if (number == 0)
      number = 4;
    else
      number = (number * 10) + 4;
  }

  if (key == '7') {
    if (number == 0)
      number = 7;
    else
      number = (number * 10) + 7;
  }

  if (key == '0') {
    if (number == 0)
      number = 0;
    else
      number = (number * 10) + 0;
  }

  if (key == '2') {
    if (number == 0)
      number = 2;
    else
      number = (number * 10) + 2;
  }

  if (key == '5') {
    if (number == 0)
      number = 5;
    else
      number = (number * 10) + 5;
  }

  if (key == '8') {
    if (number == 0)
      number = 8;
    else
      number = (number * 10) + 8;
  }

  if (key == '3') {
    if (number == 0)
      number = 3;
    else
      number = (number * 10) + 3;
  }

  if (key == '6') {
    if (number == 0)
      number = 6;
    else
      number = (number * 10) + 6;
  }

  if (key == '9') {
    if (number == 0)
      number = 9;
    else
      number = (number * 10) + 9;
  }

  if (key == '*') { // this is for cancelling
    reset();
    return;
  }

  if (key == '#') { // =
    num2 = number;
    result = true;
  }

  if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    num1 = number;
    number = 0;
    if (key == 'A') {
      action = '+';
    }
    if (key == 'B') {
      action = '-';
    }
    if (key == 'C') {
      action = '*';
    }
    if (key == 'D') {
      action = '/';
    }
  } else {
    printOnDisplay();
  }
  delay(100);
}
