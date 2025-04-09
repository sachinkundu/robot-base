#include "HMI.h"

HMI::HMI(int redPin, int greenPin)
    : redPin(redPin), greenPin(greenPin), redState(false), greenState(false),
      lastRedToggle(0), lastGreenToggle(0), redBlinking(false), greenBlinking(false) {}

void HMI::begin() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
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