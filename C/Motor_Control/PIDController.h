#include <stdint.h>
#include <math.h>

#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

float clamp_float(float d, float min, float max); // Helper

typedef struct {
    float kP;     // Proportional gain
    float kI;     // Integral gain
    float kD;     // Derivative gain
    float prevError;
    float integral;
} PIDController;

void PIDControllerInit(PIDController* controller, float kp, float ki, float kd);
int PIDControllerUpdate(PIDController* controller, float setpoint, float input);

#endif
