#ifndef HMI_H
#define HMI_H

#include <Arduino.h>

class HMI {
public:
    HMI(int redPin, int greenPin); // Constructor to initialize pins

    void begin();                  // Initialize the pins
    void setRed(bool on);          // Turn the red LED on or off
    void setGreen(bool on);        // Turn the green LED on or off
    void blinkRed();               // Blink the red LED
    void blinkGreen();             // Blink the green LED
    void update();                 // Update the blinking state (call in the main loop)

private:
    int redPin;                    // Pin for the red LED
    int greenPin;                  // Pin for the green LED
    bool redState;                 // Current state of the red LED
    bool greenState;               // Current state of the green LED
    unsigned long lastRedToggle;   // Last time the red LED toggled
    unsigned long lastGreenToggle; // Last time the green LED toggled
    bool redBlinking;              // Whether the red LED is blinking
    bool greenBlinking;            // Whether the green LED is blinking
    const unsigned long blinkInterval = 1000; // Blink interval in milliseconds
};

#endif