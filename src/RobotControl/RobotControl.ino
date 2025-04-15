#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "XboxController.h"
#include "MecanumDrive.h"
#include "DebugMenu.h"
#include "HMI.h"

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000;

float joystickXSum = 0;
float joystickYSum = 0;
float joystickTurnSum = 0;
unsigned long joystickSampleCount = 0;
unsigned long joystickLastResetTime = 0;

float motorPowerSum[4] = {0, 0, 0, 0}; // Accumulate motor powers for 4 motors
unsigned long motorSampleCount = 0;   // Count the number of motor power samples

USB Usb;

XboxController xbox(&Usb);
MecanumDrive mecanumDrive;
int motorFaultPins[4] = {43, 45, 47, 49}; // Define motor fault pins
int redPin = 26; // Pin for red LED
int greenPin = 24; // Pin for green LED

HMI hmi(redPin, greenPin, motorFaultPins); // Pass motor fault pins to the HMI constructor

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
    Serial.println(F("  [s] Print System Status"));
    Serial.println(F("==================================="));
}

void loop() {
    static bool setupComplete = false;
    static bool menuDisplayed = false;
    static bool motorFaultRecovered = false;
    static bool statusPrintingActive = false; // Flag to track if status printing is active

    // Check for motor faults
    if (hmi.motorInFault()) {
        if (!motorFaultRecovered) {
            Serial.println(F("Motor fault detected!"));
            mecanumDrive.disableMotors();
            motorFaultRecovered = true;
        }
        hmi.update();
        return;
    }

    // Recover from motor fault
    if (motorFaultRecovered) {
        Serial.println(F("Motor fault cleared. Press the Xbox button to continue."));
        motorFaultRecovered = false;
        setupComplete = false;
    }

    // Wait for Xbox button press if setup is not complete
    if (!setupComplete) {
        Serial.println(F("Waiting for Xbox button press..."));
        xbox.setLedRotating();
        if (waitForXboxButton()) {
            if (!mecanumDrive.initialize()) {
                Serial.println(F("Failed to initialize MecanumControl!"));
                while (1);
            }

            Serial.println(F("Setup complete"));
            setupComplete = true;

            hmi.blinkGreen();
            xbox.setRumble(255, 255);
            delay(100);
            xbox.stopRumble();
        }
        return;
    } 

    // Display the main menu if not already displayed
    if (!menuDisplayed) {
        printMainMenu();
        menuDisplayed = true;
    }

    // Handle serial input commands
    if (Serial.available()) {
        char command = Serial.read();
        menuDisplayed = false;

        if (command == 'd') {
            enterDebugMode(mecanumDrive, xbox);
        } else if (command == 'h') {
            printMainMenu();
            menuDisplayed = true;
        } else {
            Serial.println(F("Invalid command. Type 'h' for help."));
        }
    }

    // Handle Xbox controller input and motor control
    Usb.Task();
    float x, y, turn;

    if (xbox.isConnected() && deadManActivated()) {
        xbox.update();

        x = xbox.getX();
        y = xbox.getY();
        turn = xbox.getTurn();

        mecanumDrive.enableMotors();
        mecanumDrive.move(x, y, turn);

        // statusPrinter.accumulateData(x, y, turn);
        hmi.setGreen(true);

    } else {
        mecanumDrive.disableMotors();
        hmi.blinkGreen();
    }
    hmi.update();
}