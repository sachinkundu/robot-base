#include "DebugMenu.h"

bool serialPrintEnabled = false;    // Flag to control serial printing

void enterDebugMode(MecanumControl &drive, XboxController &xbox) {
  Serial.println(F("Entering Debug Mode..."));
  Serial.println(F("Commands:"));
  Serial.println(F("  q - Exit Debug Mode"));
  Serial.println(F("  s - System Status"));
  Serial.println(F("  t - Toggle Serial Printing in Normal Mode"));
  Serial.println(F("  n - Move North"));
  Serial.println(F("  e - Move East"));
  Serial.println(F("  w - Move West"));
  Serial.println(F("  x - Move South"));
  Serial.println(F("  f - Move Northeast"));
  Serial.println(F("  g - Move Northwest"));
  Serial.println(F("  h - Move Southeast"));
  Serial.println(F("  j - Move Southwest"));

  // Disable Xbox controller inputs
  xbox.setDisabled(true);

  while (true) {
    if (Serial.available()) {
      char debugCommand = Serial.read();
      if (debugCommand == 'q') {
        Serial.println(F("Exiting Debug Mode..."));
        break;
      }

      if (debugCommand == 't') {
        Serial.print(F("Normal mode serial printing "));
      }

      // System Status
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

      // Cardinal Directions
      float x = 0, y = 0, turn = 0;

      if (debugCommand == 'n') { // North
        x = 0; y = 1;
      } else if (debugCommand == 'e') { // East
        x = 1; y = 0;
      } else if (debugCommand == 'w') { // West
        x = -1; y = 0;
      } else if (debugCommand == 'x') { // South
        x = 0; y = -1;
      } else if (debugCommand == 'f') { // Northeast
        x = 1; y = 1;
      } else if (debugCommand == 'g') { // Northwest
        x = -1; y = 1;
      } else if (debugCommand == 'h') { // Southeast
        x = 1; y = -1;
      } else if (debugCommand == 'j') { // Southwest
        x = -1; y = -1;
      }

      if (x != 0 || y != 0 || turn != 0) {
        drive.calculateMotorPowers(x, y, turn);

        if (serialPrintEnabled) {
          Serial.println(F("Simulating Direction:"));
          Serial.print(F("X: "));
          Serial.print(x, 4);
          Serial.print(F("\tY: "));
          Serial.print(y, 4);
          Serial.print(F("\tTurn: "));
          Serial.println(turn, 4);

          Serial.print(F("LF: "));
          Serial.print(drive.getLeftFront(), 4);
          Serial.print(F("\tRF: "));
          Serial.print(drive.getRightFront(), 4);
          Serial.print(F("\tLR: "));
          Serial.print(drive.getLeftRear(), 4);
          Serial.print(F("\tRR: "));
          Serial.println(drive.getRightRear(), 4);
        }
      }
    }
  }

  // Re-enable Xbox controller inputs
  xbox.setDisabled(false);
}