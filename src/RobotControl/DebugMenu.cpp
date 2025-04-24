#include "DebugMenu.h"

bool serialPrintEnabled = false;    // Flag to control serial printing

void printMotorControlMenu() {
  Serial.println(F("==================================="));
  Serial.println(F("         Motor Control Menu        "));
  Serial.println(F("==================================="));
  Serial.println(F("Select motor to activate:"));
  Serial.println(F("  [1] Front Left"));
  Serial.println(F("  [2] Front Right"));
  Serial.println(F("  [3] Rear Left"));
  Serial.println(F("  [4] Rear Right"));
  Serial.println(F("  [a] All Motors"));
  Serial.println(F("  [ESC] Stop All Motors"));
  Serial.println(F("  [q] Exit Motor Control Submenu"));
  Serial.println(F("==================================="));
}

void printCardinalDirectionMenu() {
  Serial.println(F("==================================="));
  Serial.println(F("     Cardinal Direction Commands   "));
  Serial.println(F("==================================="));
  Serial.println(F("  [n] Move North"));
  Serial.println(F("  [e] Move East"));
  Serial.println(F("  [w] Move West"));
  Serial.println(F("  [s] Move South")); // Changed from 'x' to 's'
  Serial.println(F("  [f] Move Northeast"));
  Serial.println(F("  [g] Move Northwest"));
  Serial.println(F("  [h] Move Southeast"));
  Serial.println(F("  [j] Move Southwest"));
  Serial.println(F("  [q] Exit Cardinal Direction Control"));
  Serial.println(F("  [ESC] Stop all motors"));
  Serial.println(F("==================================="));
}

void stopAllMotors(MecanumDrive &drive) {
  Serial.println(F("Stopping all motors."));
  drive.move(0, 0, 0); // Reset motor power values to zero
  drive.disableMotors(); // Stop all motors
}

void motorControlSubmenu(MecanumDrive &drive) {
  Serial.println(F("Motor Control Submenu"));
  Serial.println(F("Enter desired motor RPM:"));
  while (!Serial.available());
  int rpm = Serial.parseInt(); // Read the RPM value
  Serial.print(F("RPM set to: "));
  Serial.println(rpm);

  // Normalize RPM to a value between 0 and 1
  float normalizedSpeed = static_cast<float>(rpm) / drive.getMaxRPM(); // Assuming getMaxRPM() returns the max RPM

  char activeMotor = '\0'; // Track the currently active motor

  while (true) {
    // Print motor control menu
    printMotorControlMenu();

    while (!Serial.available());
    char motorSelection = Serial.read(); // Read the input as a char

    switch (motorSelection) {
      case '1':
      case '2':
      case '3':
      case '4': {
        Serial.println(F("Selected motor: "));
        Serial.println(motorSelection);
        Serial.println(F("Press f for forward or r for reverse:"));
        while (!Serial.available());
        char direction = Serial.read();
        bool forward = (direction == 'f');

        // Stop the currently active motor if it's different from the new selection
        if (activeMotor != '\0' && activeMotor != motorSelection) {
          Serial.print(F("Stopping previously active motor: "));
          Serial.println(activeMotor);
          switch (activeMotor) {
            case '1': drive.disableFrontLeftMotor(); break;
            case '2': drive.disableFrontRightMotor(); break;
            case '3': drive.disableRearLeftMotor(); break;
            case '4': drive.disableRearRightMotor(); break;
          }
        }

        // Update the active motor
        activeMotor = motorSelection;

        // Enable and set RPM for the selected motor
        switch (motorSelection) {
          case '1': drive.enableFrontLeftMotor(); drive.setMotorRPM("frontLeft", rpm, !forward); break;
          case '2': drive.enableFrontRightMotor(); drive.setMotorRPM("frontRight", rpm, forward); break;
          case '3': drive.enableRearLeftMotor(); drive.setMotorRPM("rearLeft", rpm, !forward); break;
          case '4': drive.enableRearRightMotor(); drive.setMotorRPM("rearRight", rpm, forward); break;
        }
        break;
      }
      case 'a': {
        Serial.println(F("Activating all motors. Use cardinal commands to control direction."));
        drive.enableMotors(); // Enable all motors

        // Show cardinal direction commands
        printCardinalDirectionMenu();

        while (true) {
          while (!Serial.available());
          char directionCommand = Serial.read();

          switch (directionCommand) {
            case 'n': Serial.println(F("Moving North")); drive.move(0, normalizedSpeed, 0); break;
            case 'e': Serial.println(F("Moving East")); drive.move(normalizedSpeed, 0, 0); break;
            case 'w': Serial.println(F("Moving West")); drive.move(-normalizedSpeed, 0, 0); break;
            case 's': Serial.println(F("Moving South")); drive.move(0, -normalizedSpeed, 0); break;
            case 'f': Serial.println(F("Moving Northeast")); drive.move(normalizedSpeed, normalizedSpeed, 0); break;
            case 'g': Serial.println(F("Moving Northwest")); drive.move(-normalizedSpeed, normalizedSpeed, 0); break;
            case 'h': Serial.println(F("Moving Southeast")); drive.move(normalizedSpeed, -normalizedSpeed, 0); break;
            case 'j': Serial.println(F("Moving Southwest")); drive.move(-normalizedSpeed, -normalizedSpeed, 0); break;
            case 'r': Serial.println(F("Rotating Right")); drive.move(0, 0, normalizedSpeed); break;
            case 'l': Serial.println(F("Rotating Left")); drive.move(0, 0, -normalizedSpeed); break;
            case 27: // ESC key
              stopAllMotors(drive);
              break;
            case 'q': // Exit cardinal direction control
              Serial.println(F("Exiting Cardinal Direction Control..."));
              stopAllMotors(drive);
              break;
            default:
              Serial.println(F("Invalid direction command."));
              break;
          }

          if (directionCommand == 'q' || directionCommand == 27) break;
        }
        break;
      }
      case 27: // ESC key
        stopAllMotors(drive);
        activeMotor = '\0'; // Reset active motor
        break;
      case 'q': // Exit motor control submenu
        Serial.println(F("Exiting Motor Control Submenu..."));
        return;
      default:
        Serial.println(F("Invalid motor selection."));
        break;
    }
  }
}

void printDebugMenu() {
  Serial.println(F("==================================="));
  Serial.println(F("           Debug Mode Menu         "));
  Serial.println(F("==================================="));
  Serial.println(F("Commands:"));
  Serial.println(F("  [q] Exit Debug Mode"));
  Serial.println(F("  [s] System Status"));
  Serial.println(F("  [m] Motor Control Submenu"));
  Serial.println(F("==================================="));
}

void enterDebugMode(MecanumDrive &drive, XboxController &xbox) {
  // Print the debug menu initially
  printDebugMenu();

  while (true) {
    if (Serial.available()) {
      char debugCommand = Serial.read();
      if (debugCommand == 'q') {
        Serial.println(F("Exiting Debug Mode..."));
        break; // Exit debug mode
      }

      if (debugCommand == 's') {
        Serial.println(F("System Status:"));
        Serial.print(F("Front Left: "));
        Serial.print(drive.getFrontLeft(), 2);
        Serial.print(F("\tFront Right: "));
        Serial.print(drive.getFrontRight(), 2);
        Serial.print(F("\tRear Left: "));
        Serial.print(drive.getRearLeft(), 2);
        Serial.print(F("\tRear Right: "));
        Serial.println(drive.getRearRight(), 2);
      }

      if (debugCommand == 'm') {
        // Disable Xbox controller inputs
        xbox.setDisabled(true);
        motorControlSubmenu(drive); // Enter motor control submenu
        printDebugMenu(); // Print the debug menu again after returning from the submenu
        // Re-enable Xbox controller inputs
        xbox.setDisabled(false);
      }
      // Other debug commands...
    }
  }
}
