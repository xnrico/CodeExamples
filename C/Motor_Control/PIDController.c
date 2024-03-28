#include "PIDController.h"

#define kF 0.5

void PIDControllerInit(PIDController* controller, float kp, float ki, float kd) {
    controller->kP = kp;
    controller->kI = ki;
    controller->kD = kd;
    controller->prevError = 0.0;
    controller->integral = 0.0;
}

int PIDControllerUpdate(PIDController* controller, float setpoint, float input) {
    float error = setpoint - input;

    if (fabs(error) < 2)
    	return 0;

    // Calculate the proportional term
    float proportional = controller->kP * error;

    // Calculate the integral term
    controller->integral += controller->kI * error;
    controller->integral = clamp_float(controller->integral, -100, +100);

    // Calculate the derivative term
    float derivative = controller->kD * (error - controller->prevError);
    controller->prevError = error;

    // Calculate the control output
    int output = proportional + controller->integral + derivative;

    output = clamp_float(output, -200.0, 200.0);

    return output;
}

float clamp_float(float d, float min, float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}
