#ifndef XBOXCONTROLLER_H
#define XBOXCONTROLLER_H

#include <XBOXUSB.h>

// Constants
const float LEFT_DEADZONE = 0.1;  // Deadzone for the left joystick (-1 to 1)
const float RIGHT_DEADZONE = 0.15; // Deadzone for the right joystick (-1 to 1)

class XboxController {
public:
  // Constructor
  XboxController(USB* usb) : Xbox(usb) {}

  // Update the controller state
  void update() {
    if (disabled) return;

    // Read joystick inputs (values from -32768 to 32767)
    int16_t xRaw = Xbox.getAnalogHat(LeftHatX);
    int16_t yRaw = Xbox.getAnalogHat(LeftHatY);
    int16_t turnRaw = Xbox.getAnalogHat(RightHatX);

    // Normalize inputs to range -1 to 1
    xNorm = xRaw / 32767.0;
    yNorm = yRaw / 32767.0;
    turnNorm = turnRaw / 32767.0;

    // Apply deadzone for the left joystick
    if (abs(xNorm) < LEFT_DEADZONE) xNorm = 0;
    if (abs(yNorm) < LEFT_DEADZONE) yNorm = 0;

    // Apply deadzone for the right joystick
    if (abs(turnNorm) < RIGHT_DEADZONE) turnNorm = 0;
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

  // Get Xbox button press value
  uint8_t getXBoxButtonPressed() {
    return disabled ? 0 : Xbox.getButtonPress(XBOX);
  }

  // Enable or disable the controller
  void setDisabled(bool state) {
    disabled = state;
  }

  // Shim to set rumble on
  void setRumble(uint8_t leftMotor, uint8_t rightMotor) {
    if (!disabled) {
      Xbox.setRumbleOn(leftMotor, rightMotor);
    }
  }

  // Shim to turn rumble off
  void stopRumble() {
    if (!disabled) {
      Xbox.setRumbleOn(0, 0);
    }
  }

  // Shim to set the LED to rotating mode
  void setLedRotating() {
    if (!disabled) {
      Xbox.setLedMode(ROTATING);
    }
  }

  // Shim to set the LED to LED1
  void setLedToLED1() {
    if (!disabled) {
      Xbox.setLedOn(LED1);
    }
  }

  // Shim to set the LED to LED4
  void setLedToLED4() {
    if (!disabled) {
      Xbox.setLedOn(LED4);
    }
  }

  // Shim to blink all LEDs
  void setLedBlinkAll() {
    if (!disabled) {
      Xbox.setLedBlink(ALL);
    }
  }

private:
  XBOXUSB Xbox;
  float xNorm = 0;
  float yNorm = 0;
  float turnNorm = 0;
  bool disabled = false;
};

#endif // XBOXCONTROLLER_H