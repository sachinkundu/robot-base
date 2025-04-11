#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "XboxController.h"
#include "MecanumDrive.h"
#include "DebugMenu.h"
#include "HMI.h"

bool printingEnabled = false;

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000;

USB Usb;

XboxController xbox(&Usb);
MecanumDrive mecanumDrive;
HMI hmi(26, 24);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  hmi.begin();
  hmi.blinkRed();

  if (Usb.Init() == -1) {
    Serial.println(F("USB initialization failed!"));
    while (1) {
      hmi.update();
    }
  }
  Serial.println(F("XBOX USB Library Started"));

  hmi.setRed(true);
  xbox.setLedRotating();
}

bool waitForXboxButton() {
  unsigned long buttonPressStart = 0;
  bool buttonHeld = false;

  while (true) {
    Usb.Task();

    if (xbox.isConnected()) {
      if (xbox.getXBoxButtonPressed()) {
        if (!buttonHeld) {
          buttonPressStart = millis();
          buttonHeld = true;
        } else if (millis() - buttonPressStart >= 1000) {
          Serial.println(F("Xbox center button held for 1 seconds. Starting program..."));
          return true;
        }
      } else {
        buttonHeld = false;
      }
    }
  }
}

bool deadManActivated() {
  return xbox.getLT() == 255;
}

void printMainMenu() {
  Serial.println(F("==================================="));
  Serial.println(F("           Main Menu v 0.4         "));
  Serial.println(F("==================================="));
  Serial.println(F("Available Commands:"));
  Serial.println(F("  [d] Enter Debug Mode"));
  Serial.println(F("  [p] Toggle Serial Printing"));
  Serial.println(F("  [h] Display this Help Menu"));
  Serial.println(F("==================================="));
}

void loop() {
  Usb.Task();

  static bool setupComplete = false;
  static bool menuDisplayed = false;

  if (!setupComplete) {
    Serial.println(F("Hold the Xbox center button for 1 seconds to start the program."));

    if (waitForXboxButton()) {
      if (!mecanumDrive.initialize()) {
        Serial.println(F("Failed to initialize MecanumControl!"));
        while (1);
      }

      Serial.println(F("Setup complete"));
      setupComplete = true;

      xbox.setLedToLED1();

      hmi.blinkGreen();

      xbox.setRumble(255, 255);
      delay(100);
      xbox.stopRumble();
    }
    return;
  }

  if (!menuDisplayed) {
    printMainMenu();
    menuDisplayed = true;
  }

  if (Serial.available()) {
    char command = Serial.read();
    menuDisplayed = false;

    if (command == 'd') {
      enterDebugMode(mecanumDrive, xbox);
    } else if (command == 'p') {
      printingEnabled = !printingEnabled;
      if (printingEnabled) {
        Serial.println(F("Normal mode serial printing enabled"));
      } else {
        Serial.println(F("Normal mode serial printing disabled"));
      }
    } else if (command == 'h') {
      printMainMenu();
      menuDisplayed = true;
    } else {
      Serial.println(F("Invalid command. Type 'h' for help."));
    }
  }
  
  
  float x, y, turn;

  if (xbox.isConnected() && deadManActivated()) {
    
    xbox.update();

    if (printingEnabled && millis() - lastPrintTime >= printInterval) {
      lastPrintTime = millis();
      Serial.print("DM pressed: ");
      Serial.print(xbox.getLT());
      Serial.println();
    }

    mecanumDrive.enableMotors();
    x = xbox.getX();
    y = xbox.getY();
    turn = xbox.getTurn();
    mecanumDrive.move(x, y, turn);

    hmi.setGreen(true);

  } else {
    mecanumDrive.disableMotors();

    hmi.blinkGreen();
  }

  if (printingEnabled && millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();
    Serial.println(F("Motor powers updated"));
  }

  hmi.update();
}