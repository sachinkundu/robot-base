#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include <Arduino.h>
#include "MecanumDrive.h"
#include "XboxController.h"

// Function to initialize the debug menu
void enterDebugMode(MecanumDrive &drive, XboxController &xbox);

#endif // DEBUG_MENU_H