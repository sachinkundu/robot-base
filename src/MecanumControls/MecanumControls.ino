// Arduino program to read Xbox 360 controller inputs and control mecanum wheels.

//Include the necessary libraries
#include <XBOXUSB.h>
#include <SPI.h>

// Initialize USB and Xbox controller objects
USB Usb;
XBOXUSB Xbox(&Usb);


const float DEADZONE = 0.1; // Deadzone for joystick inputs (-1 to 1)

// Variables to store motor powers
float leftFront = 0;
float rightFront = 0;
float leftRear = 0;
float rightRear = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect

  if (Usb.Init() == -1) {
    Serial.print(F("\nDid not start"));
    while (1); // stop if USB initialization fails
  }
  Serial.println(F("\nXBOX USB Library Started"));
}

void loop() {
  Usb.Task();

  if (Xbox.Xbox360Connected) {
    // Read joystick inputs (values from -32768 to 32767)
    int16_t x = Xbox.getAnalogHat(LeftHatX);
    int16_t y = Xbox.getAnalogHat(LeftHatY);
    int16_t turn = Xbox.getAnalogHat(RightHatX);

    // Normalize inputs to range -1 to 1
    float xNorm = x / 32767.0;
    float yNorm = y / 32767.0;
    float turnNorm = turn / 32767.0;

    // Apply deadzone to eliminate drift
    if (abs(xNorm) < DEADZONE) xNorm = 0;
    if (abs(yNorm) < DEADZONE) yNorm = 0;
    if (abs(turnNorm) < DEADZONE) turnNorm = 0;

    // calculate the angle of joystick vector
    float theta = atan2(yNorm, xNorm);
    // calculate the magnitude of joystick vector
    float power = hypot(xNorm, yNorm);

    // adjust theta by 45deg (PI/4 radians) to align with wheel axes.
    float sinComponent = sin(theta - PI / 4);
    float cosComponent = cos(theta - PI / 4);

    // Find the maximum of the absolute values
    float maxComponent = max(abs(sinComponent), abs(cosComponent));

    // Normalize sin and cos components
    if (maxComponent != 0) { // Avoid division by zero
      sinComponent /= maxComponent;
      cosComponent /= maxComponent;
    }

    // Compute motor powers
    leftFront = power * cosComponent + turnNorm;
    rightFront = power * sinComponent - turnNorm;
    leftRear = power * sinComponent + turnNorm;
    rightRear = power * cosComponent - turnNorm;

    // Compute the total power including turn
    float totalPower = power + abs(turnNorm);

    // If total power > 1, normalize motor powers
    if (totalPower > 1.0) {
      leftFront /= totalPower;
      rightFront /= totalPower;
      leftRear /= totalPower;
      rightRear /= totalPower;
    }

     //Output motor powers through serial for testing
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
