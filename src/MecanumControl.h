#ifndef MECANUMCONTROL_H
#define MECANUMCONTROL_H

#include <Arduino.h>

class MecanumControl {
public:

  // Calculate motor powers based on joystick inputs
  void calculateMotorPowers(float x, float y, float turn) {

    // Calculate the angle of joystick vector
    float theta = atan2(y, x);

    // Calculate the magnitude of joystick vector
    float power = hypot(x, y);

    // Adjust theta by 45 degrees (PI/4 radians) to align with wheel axes
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
    leftFront = power * cosComponent + turn;
    rightFront = power * sinComponent - turn;
    leftRear = power * sinComponent + turn;
    rightRear = power * cosComponent - turn;

    // Compute the total power including turn
    float totalPower = power + abs(turn);

    // If total power > 1, normalize motor powers
    if (totalPower > 1.0) {
      leftFront /= totalPower;
      rightFront /= totalPower;
      leftRear /= totalPower;
      rightRear /= totalPower;
    }
  }

  // Get motor power for the left front wheel
  float getLeftFront() {
    return leftFront;
  }

  // Get motor power for the right front wheel
  float getRightFront() {
    return rightFront;
  }

  // Get motor power for the left rear wheel
  float getLeftRear() {
    return leftRear;
  }

  // Get motor power for the right rear wheel
  float getRightRear() {
    return rightRear;
  }

private:
  float leftFront = 0;
  float rightFront = 0;
  float leftRear = 0;
  float rightRear = 0;
};

#endif // MECANUMCONTROL_H