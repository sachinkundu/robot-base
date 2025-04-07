#include "DebugMenu.h"

bool serialPrintEnabled = false;    // Flag to control serial printing

void motorControlSubmenu(MecanumDrive &drive) {
  Serial.println(F("Motor Control Submenu"));
  Serial.println(F("Enter desired motor RPM:"));
  while (!Serial.available());
  int rpm = Serial.parseInt(); // Read the RPM value
  Serial.print(F("RPM set to: "));
  Serial.println(rpm);

  Serial.println(F("Select motor to activate:"));
  Serial.println(F("  fl - Front Left"));
  Serial.println(F("  fr - Front Right"));
  Serial.println(F("  rl - Rear Left"));
  Serial.println(F("  rr - Rear Right"));
  Serial.println(F("  a  - All Motors"));

  while (!Serial.available());
  String motorSelection = Serial.readStringUntil('\n'); // Read motor selection

  if (motorSelection == "fl" || motorSelection == "fr" || motorSelection == "rl" || motorSelection == "rr") {
    Serial.println(F("Press f for forward or r for reverse:"));
    while (!Serial.available());
    char direction = Serial.read();
    bool forward = (direction == 'f');

    if (motorSelection == "fl") {
      drive.setMotorRPM("leftFront", rpm, forward);
    } else if (motorSelection == "fr") {
      drive.setMotorRPM("rightFront", rpm, forward);
    } else if (motorSelection == "rl") {
      drive.setMotorRPM("leftRear", rpm, forward);
    } else if (motorSelection == "rr") {
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
  } else {
    Serial.println(F("Invalid motor selection."));
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
