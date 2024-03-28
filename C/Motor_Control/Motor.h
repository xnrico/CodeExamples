#include <stdint.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "PIDController.h"

void handle_error(cy_rslt_t status);				// Error Handler
void check_status(char *message, cy_rslt_t status); // Status Check
int8_t clamp(int d, int min, int max);			// Helper

typedef struct {
	char polarity; 							// Polarity of Motor (+1 or -1)
	char base;								// Base or Back

	uint8_t max_velocity;	 				// Max Motor Velocity (0 - 100)
	int32_t position; 						// Encoder Position
	int8_t output;							// Percent Output (0 - 100)

	uint8_t out;							// PWM Output Pin
	uint8_t A1;								// H-Bridge Sel 1
	uint8_t A2;								// H-Bridge Sel 2
	uint8_t port1;							// A1 / 10
	uint8_t pin1;							// A1 % 10
	uint8_t port2;							// A2 / 10
	uint8_t pin2;							// A2 % 10

	cyhal_pwm_t pwm;						// Cypress PWM Object
	cy_rslt_t result;						// Cypress API Return Code
	PIDController pid;						// PID Controller
} motor;

// Motor Initialization
void motorInit(motor* motor, char base);

void setMotorPolarity(motor* motor, char pol);	// Configures the polarity of the motor (1 or -1)
void setDebugMode(motor* motor, char debug);	// Enable or Disable Debug Mode (1 is On)
void setMaxVelocity(motor* motor, uint8_t vel); // Configures the maximum velocity (0 - 100)
void setMotorOutput(motor* motor, int8_t out);	// Set motor output with PWM
void goToPosition(motor* motor, int32_t pos);	// Go to a target position
