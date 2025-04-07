#include "DebugMenu.h"

bool serialPrintEnabled = false;    // Flag to control serial printing

void motorControlSubmenu(MecanumDrive &drive) {
  Serial.println(F("Motor Control Submenu"));
  Serial.println(F("Enter desired motor RPM:"));
  while (!Serial.available());
  int rpm = Serial.parseInt(); // Read the RPM value
  Serial.print(F("RPM set to: "));
  Serial.println(rpm);

  // Normalize RPM to a value between 0 and 1
  float normalizedSpeed = static_cast<float>(rpm) / drive.getMaxRPM(); // Assuming getMaxRPM() returns the max RPM

  String activeMotor = ""; // Track the currently active motor

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
      Serial.println(F("Selected motor: "));
      Serial.println(motorSelection);
      Serial.println(F("Press f for forward or r for reverse:"));
      while (!Serial.available());
      char direction = Serial.read();
      bool forward = (direction == 'f');

      // Stop the currently active motor if it's different from the new selection
      if (activeMotor != "" && activeMotor != motorSelection) {
        Serial.print(F("Stopping previously active motor: "));
        Serial.println(activeMotor);
        if (activeMotor == "1") {
          drive.disableLeftFrontMotor();
        } else if (activeMotor == "2") {
          drive.disableRightFrontMotor();
        } else if (activeMotor == "3") {
          drive.disableLeftRearMotor();
        } else if (activeMotor == "4") {
          drive.disableRightRearMotor();
        }
      }

      // Update the active motor
      activeMotor = motorSelection;

      // Enable and set RPM for the selected motor
      if (motorSelection == "1") {
        drive.enableLeftFrontMotor();
        drive.setMotorRPM("leftFront", rpm, !forward);
      } else if (motorSelection == "2") {
        drive.enableRightFrontMotor();
        drive.setMotorRPM("rightFront", rpm, forward);
      } else if (motorSelection == "3") {
        Serial.println(F("Enabling left rear motor."));
        drive.enableLeftRearMotor();
        drive.setMotorRPM("leftRear", rpm, !forward);
      } else if (motorSelection == "4") {
        drive.enableRightRearMotor();
        drive.setMotorRPM("rightRear", rpm, forward);
      }
    } else if (motorSelection == "a") {
      Serial.println(F("Activating all motors. Use cardinal commands to control direction."));
      drive.enableMotors(); // Enable all motors

      // Show cardinal direction commands
      Serial.println(F("  n - Move North"));
      Serial.println(F("  e - Move East"));
      Serial.println(F("  w - Move West"));
      Serial.println(F("  x - Move South"));
      Serial.println(F("  f - Move Northeast"));
      Serial.println(F("  g - Move Northwest"));
      Serial.println(F("  h - Move Southeast"));
      Serial.println(F("  j - Move Southwest"));
      Serial.println(F("  q - Exit Cardinal Direction Control"));
      Serial.println(F("  s - Stop all motors"));

      while (true) {
        while (!Serial.available());
        char directionCommand = Serial.read();

        if (directionCommand == 'n') {
          Serial.println(F("Moving North"));
          drive.move(0, normalizedSpeed, 0); // Forward
        } else if (directionCommand == 'e') {
          Serial.println(F("Moving East"));
          drive.move(normalizedSpeed, 0, 0); // Right
        } else if (directionCommand == 'w') {
          Serial.println(F("Moving West"));
          drive.move(-normalizedSpeed, 0, 0); // Left
        } else if (directionCommand == 'x') {
          Serial.println(F("Moving South"));
          drive.move(0, -normalizedSpeed, 0); // Backward
        } else if (directionCommand == 'f') {
          Serial.println(F("Moving Northeast"));
          drive.move(normalizedSpeed, normalizedSpeed, 0); // Diagonal forward-right
        } else if (directionCommand == 'g') {
          Serial.println(F("Moving Northwest"));
          drive.move(-normalizedSpeed, normalizedSpeed, 0); // Diagonal forward-left
        } else if (directionCommand == 'h') {
          Serial.println(F("Moving Southeast"));
          drive.move(normalizedSpeed, -normalizedSpeed, 0); // Diagonal backward-right
        } else if (directionCommand == 'j') {
          Serial.println(F("Moving Southwest"));
          drive.move(-normalizedSpeed, -normalizedSpeed, 0); // Diagonal backward-left
        } else if (directionCommand == 's') {
          Serial.println(F("Stopping all motors."));
          drive.move(0, 0, 0); // Reset motor power values to zero
          drive.disableMotors(); // Stop all motors
        } else if (directionCommand == 'q') {
          Serial.println(F("Exiting Cardinal Direction Control..."));
          drive.move(0, 0, 0); // Reset motor power values to zero
          drive.disableMotors(); // Stop all motors
          break; // Exit the cardinal direction control loop
        } else {
          Serial.println(F("Invalid direction command."));
        }
      }
    } else if (motorSelection == "s") {
      Serial.println(F("Stopping all motors."));
      drive.disableMotors(); // Stop all motors
      activeMotor = ""; // Reset active motor
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
