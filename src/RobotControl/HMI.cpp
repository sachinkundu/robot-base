#include "HMI.h"

HMI::HMI(int redPin, int greenPin, int motorFaultPins[4]) 
    : redPin(redPin), greenPin(greenPin), redState(false), greenState(false),
      lastRedToggle(0), lastGreenToggle(0), redBlinking(false), greenBlinking(false) {
    for (int i = 0; i < 4; i++) {
        this->motorFaultPins[i] = motorFaultPins[i];
    }
}

void HMI::begin() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    for (int i = 0; i < 4; i++) {
        pinMode(motorFaultPins[i], INPUT_PULLUP); // Fault pins are normally high
    }
}

void HMI::setRed(bool on) {
    redBlinking = false; // Stop blinking if manually controlled
    redState = on;
    digitalWrite(redPin, on ? HIGH : LOW);
}

void HMI::setGreen(bool on) {
    greenBlinking = false; // Stop blinking if manually controlled
    greenState = on;
    digitalWrite(greenPin, on ? HIGH : LOW);
}

void HMI::blinkRed() {
    redBlinking = true;
}

void HMI::blinkGreen() {
    greenBlinking = true;
}

void HMI::update() {
    unsigned long currentMillis = millis();

    // Handle red LED blinking
    if (redBlinking && currentMillis - lastRedToggle >= blinkInterval) {
        redState = !redState;
        digitalWrite(redPin, redState ? HIGH : LOW);
        lastRedToggle = currentMillis;
    }

    // Handle green LED blinking
    if (greenBlinking && currentMillis - lastGreenToggle >= blinkInterval) {
        greenState = !greenState;
        digitalWrite(greenPin, greenState ? HIGH : LOW);
        lastGreenToggle = currentMillis;
    }
}

bool HMI::motorInFault() {
    for (int i = 0; i < 4; i++) {
        if (digitalRead(motorFaultPins[i]) == LOW) {
            this->setGreen(false); 
            this->blinkRed();
            return true;
        }
    }
    this->setRed(true); // Stop blinking red LED if no fault
    return false; // No faults
}