// Include the necessary libraries
#include <SPI.h>
#include "XboxController.h"
#include "MecanumControl.h"

// Initialize USB object
USB Usb;

// Create instances of the XboxController and MecanumControl classes
XboxController xbox(&Usb);
MecanumControl drive;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect

  if (Usb.Init() == -1) {
    Serial.print(F("\nDid not start"));
    while (1); // Stop if USB initialization fails
  }
  Serial.println(F("\nXBOX USB Library Started"));
}

void loop() {
  Usb.Task();

  if (xbox.isConnected()) {
    xbox.update();

    // Get joystick inputs
    float x = xbox.getX();
    float y = xbox.getY();
    float turn = xbox.getTurn();

    // Calculate motor powers
    drive.calculateMotorPowers(x, y, turn);

    // Get motor powers
    float leftFront = drive.getLeftFront();
    float rightFront = drive.getRightFront();
    float leftRear = drive.getLeftRear();
    float rightRear = drive.getRightRear();

    // Output motor powers through serial for testing
    Serial.print("Left Front: ");
    Serial.print(leftFront, 4); // Prints 4 decimals
    Serial.print("\tRight Front: ");
    Serial.print(rightFront, 4);
    Serial.print("\tLeft Rear: ");
    Serial.print(leftRear, 4);
    Serial.print("\tRight Rear: ");
    Serial.println(rightRear, 4);

    delay(50); // Makes the output readable

    /* To-do:
      - Implement motor power scaling for physical hardware.
        The calculated motor powers need to be converted to a suitable range (e.g., 0-10V)
        for motor controller inputs, possibly using a DAC (Digital-to-Analog Converter).
    */
  }
}
