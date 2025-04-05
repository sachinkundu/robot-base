#ifndef XBOXCONTROLLER_H
#define XBOXCONTROLLER_H

#include <XBOXUSB.h>

// Constants
const float DEADZONE = 0.1; // Deadzone for joystick inputs (-1 to 1)

class XboxController {
public:
  // Constructor
  XboxController(USB* usb) : Xbox(usb) {}

  // Update the controller state
  void update() {
    if (disabled) return; // Ignore updates if disabled

    // Read joystick inputs (values from -32768 to 32767)
    int16_t xRaw = Xbox.getAnalogHat(LeftHatX);
    int16_t yRaw = Xbox.getAnalogHat(LeftHatY);
    int16_t turnRaw = Xbox.getAnalogHat(RightHatX);

    // Normalize inputs to range -1 to 1
    xNorm = xRaw / 32767.0;
    yNorm = yRaw / 32767.0;
    turnNorm = turnRaw / 32767.0;

    // Apply deadzone to eliminate drift
    if (abs(xNorm) < DEADZONE) xNorm = 0;
    if (abs(yNorm) < DEADZONE) yNorm = 0;
    if (abs(turnNorm) < DEADZONE) turnNorm = 0;
  }

  // Check if the Xbox controller is connected
  bool isConnected() {
    return !disabled && Xbox.Xbox360Connected;
  }

  // Get normalized X-axis value
  float getX() {
    return disabled ? 0 : xNorm;
  }

  // Get normalized Y-axis value
  float getY() {
    return disabled ? 0 : yNorm;
  }

  // Get normalized turn value
  float getTurn() {
    return disabled ? 0 : turnNorm;
  }

  // Get left trigger value
  uint8_t getLT() {
    return disabled ? 0 : Xbox.getButtonPress(LT);
  }

  // Enable or disable the controller
  void setDisabled(bool state) {
    disabled = state;
  }

private:
  XBOXUSB Xbox;
  float xNorm = 0;
  float yNorm = 0;
  float turnNorm = 0;
  bool disabled = false; // Flag to disable the controller
};

#endif // XBOXCONTROLLER_H