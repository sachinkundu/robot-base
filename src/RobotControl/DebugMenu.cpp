#include "DebugMenu.h"

bool serialPrintEnabled = false;    // Flag to control serial printing

void motorControlSubmenu(MecanumDrive &drive) {
  Serial.println(F("Motor Control Submenu"));
  Serial.println(F("Enter desired motor RPM:"));
  while (!Serial.available());
  int rpm = Serial.parseInt(); // Read the RPM value
  Serial.print(F("RPM set to: "));
  Serial.println(rpm);

  while (true) { // Repeat the motor selection menu until 'q' is pressed
    Serial.println(F("Select motor to activate:"));
    Serial.println(F("  1 - Front Left"));
    Serial.println(F("  2 - Front Right"));
    Serial.println(F("  3 - Rear Left"));
    Serial.println(F("  4 - Rear Right"));
    Serial.println(F("  a  - All Motors"));
    Serial.println(F("  s  - Stop All Motors"));
    Serial.println(F("  q  - Exit Motor Control Submenu"));

    while (!Serial.available());
    String motorSelection = Serial.readStringUntil('\n'); // Read motor selection

    if (motorSelection == "1" || motorSelection == "2" || motorSelection == "3" || motorSelection == "4") {
      Serial.println(F("Press f for forward or r for reverse:"));
      while (!Serial.available());
      char direction = Serial.read();
      bool forward = (direction == 'f');
      drive.enableMotors(); // Enable motors before setting RPM
      if (motorSelection == "1") {
        drive.setMotorRPM("leftFront", rpm, forward);
      } else if (motorSelection == "2") {
        drive.setMotorRPM("rightFront", rpm, forward);
      } else if (motorSelection == "3") {
        drive.setMotorRPM("leftRear", rpm, forward);
      } else if (motorSelection == "4") {
        drive.setMotorRPM("rightRear", rpm, forward);
      }
    } else if (motorSelection == "a") {
      Serial.println(F("Activating all motors. Use cardinal commands to control direction."));
      // Show cardinal direction commands
      Serial.println(F("  n - Move North"));
      Serial.println(F("  e - Move East"));
      Serial.println(F("  w - Move West"));
      Serial.println(F("  x - Move South"));
      Serial.println(F("  f - Move Northeast"));
      Serial.println(F("  g - Move Northwest"));
      Serial.println(F("  h - Move Southeast"));
      Serial.println(F("  j - Move Southwest"));
    } else if (motorSelection == "s") {
      Serial.println(F("Stopping all motors."));
      drive.disableMotors(); // Stop all motors
    } else if (motorSelection == "q") {
      Serial.println(F("Exiting Motor Control Submenu..."));
      break; // Exit the submenu
    } else {
      Serial.println(F("Invalid motor selection."));
    }
  }
}

void enterDebugMode(MecanumDrive &drive, XboxController &xbox) {
  Serial.println(F("Entering Debug Mode..."));
  Serial.println(F("Commands:"));
  Serial.println(F("  q - Exit Debug Mode"));
  Serial.println(F("  s - System Status"));
  Serial.println(F("  m - Motor Control Submenu"));

  // Disable Xbox controller inputs
  xbox.setDisabled(true);

  while (true) {
    if (Serial.available()) {
      char debugCommand = Serial.read();
      if (debugCommand == 'q') {
        Serial.println(F("Exiting Debug Mode..."));
        break;
      }

      if (debugCommand == 's') {
        Serial.println(F("System Status:"));
        Serial.print(F("LF: "));
        Serial.print(drive.getLeftFront(), 4);
        Serial.print(F("\tRF: "));
        Serial.print(drive.getRightFront(), 4);
        Serial.print(F("\tLR: "));
        Serial.print(drive.getLeftRear(), 4);
        Serial.print(F("\tRR: "));
        Serial.println(drive.getRightRear(), 4);
      }

      if (debugCommand == 'm') {
        motorControlSubmenu(drive);
      }

      // Other debug commands...
    }
  }

  // Re-enable Xbox controller inputs
  xbox.setDisabled(false);
}
