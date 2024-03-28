#include <Math.h>
#include "Motor.h"

#define PWM_FREQUENCY (500u)
#define PWM_ZERO_DUTY (0.0f)

#define BASE_1A_PORT	P9_1_PORT
#define BASE_1A_PIN  	P9_1_NUM
#define BASE_1A_GPIO  	P9_1_GPIO
#define BASE_2A_PORT 	P9_2_PORT
#define BASE_2A_PIN  	P9_2_NUM
#define BASE_2A_GPIO  	P9_2_GPIO
#define BASE_OUT_PORT	P8_0_PORT
#define BASE_OUT_PIN	P8_0_NUM
#define BASE_OUT_GPIO	P8_0_GPIO
#define BASE_OUT_PWM	P8_0

#define BACK_1A_PORT	P5_4_PORT
#define BACK_1A_PIN  	P5_4_NUM
#define BACK_1A_GPIO  	P5_4_GPIO
#define BACK_2A_PORT 	P5_5_PORT
#define BACK_2A_PIN  	P5_5_NUM
#define BACK_2A_GPIO  	P5_5_GPIO
#define BACK_OUT_PORT	P0_5_PORT
#define BACK_OUT_PIN	P0_5_NUM
#define BACK_OUT_GPIO	P0_5_GPIO
#define BACK_OUT_PWM	P0_5

#define MY_TCPWM_CNT_NUM   (0UL)
#define MY_TCPWM_CNT_MASK  (1UL << MY_TCPWM_CNT_NUM)
#define MY_TCPWM_CNT_VAL   (0x8000UL) 					/* The "neutral" position for 16-bit decoder */

#define kP_BASE 0.2
#define kI_BASE 0.02
#define kD_BASE 0.0

#define kP_BACK 1.0
#define kI_BACK 0.1
#define kD_BACK 0.01

#define MIN_VELOCITY 30

void handle_error(cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        /* Halt the CPU while debugging */
        CY_ASSERT(0);
    }
}

void check_status(char *message, cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        printf("\r\n=====================================================\r\n");
        printf("\nFAIL: %s\r\n", message);
        printf("\r\n=====================================================\r\n");
        while(true);
    }
}

void motorInit(motor* motor, char base) {
	motor->polarity = 1;
	motor->max_velocity = 100;
	motor->position = 0;
	motor->output = 0;
	motor->base = base;

	// Initialize the GPIO pin
	if (motor->base == 1) { 																		// BASE
		motor->A1 = 91;
		motor->A2 = 92;
		motor->out = 80;
		motor->port1 = 9;
		motor->port2 = 9;
		motor->pin1 = 1;
		motor->pin2 = 2;

		Cy_GPIO_Pin_FastInit(BASE_1A_PORT, BASE_1A_PIN, CY_GPIO_DM_STRONG, 1UL, BASE_1A_GPIO); 		// 1A - HIGH
		Cy_GPIO_Pin_FastInit(BASE_2A_PORT, BASE_2A_PIN, CY_GPIO_DM_STRONG, 0UL, BASE_2A_GPIO); 		// 2A - LOW
		Cy_GPIO_Pin_FastInit(BASE_OUT_PORT, BASE_OUT_PIN, CY_GPIO_DM_STRONG, 0UL, BASE_OUT_GPIO); 	// PWM OUTPUT - LOW

		/* Initialize the PWM */
		motor->result = cyhal_pwm_init(&(motor->pwm), BASE_OUT_PWM, NULL);
		check_status("API cyhal_pwm_init failed with error code", motor->result);

		// Initialize the PID controller
		PIDControllerInit(&(motor->pid), kP_BASE, kI_BASE, kD_BASE);
	}
	else { 																							// BACK
		motor->A1 = 54;
		motor->A2 = 55;
		motor->out = 5;
		motor->port1 = 5;
		motor->port2 = 5;
		motor->pin1 = 6;
		motor->pin2 = 7;

		Cy_GPIO_Pin_FastInit(BACK_1A_PORT, BACK_1A_PIN, CY_GPIO_DM_STRONG, 1UL, BACK_1A_GPIO); 		// 1A - HIGH
		Cy_GPIO_Pin_FastInit(BACK_2A_PORT, BACK_2A_PIN, CY_GPIO_DM_STRONG, 0UL, BACK_2A_GPIO); 		// 2A - LOW
		Cy_GPIO_Pin_FastInit(BACK_OUT_PORT, BACK_OUT_PIN, CY_GPIO_DM_STRONG, 0UL, BACK_OUT_GPIO); 	// PWM OUTPUT - LOW

		/* Initialize the PWM */
		motor->result = cyhal_pwm_init(&(motor->pwm), BACK_OUT_PWM, NULL);
		check_status("API cyhal_pwm_init failed with error code", motor->result);

		// Initialize the PID controller
		PIDControllerInit(&(motor->pid), kP_BACK, kI_BACK, kD_BACK);
	}

    /* Set the PWM output frequency and duty cycle */
    motor->result = cyhal_pwm_set_duty_cycle(&(motor->pwm), PWM_ZERO_DUTY, PWM_FREQUENCY);
    check_status("API cyhal_pwm_set_duty_cycle failed with error code", motor->result);

    /* Start the PWM */
    motor->result = cyhal_pwm_start(&(motor->pwm));
    check_status("API cyhal_pwm_start failed with error code", motor->result);

    printf("Motor PWM started successfully...\r\n");
}

void setMotorPolarity(motor* motor, char pol) {
	motor->polarity = pol;
}

void setMaxVelocity(motor* motor, uint8_t vel) {
		motor->max_velocity = vel;
}

void setMotorOutput(motor* motor, int8_t speed) {
	speed = speed * motor->polarity;

	// Set H-Bridge Polarity
	if (speed >= 0) {
		Cy_GPIO_Write(Cy_GPIO_PortToAddr(motor->port1), motor->pin1, 1UL);
		Cy_GPIO_Write(Cy_GPIO_PortToAddr(motor->port2), motor->pin2, 0UL);
	}
	else {
		Cy_GPIO_Write(Cy_GPIO_PortToAddr(motor->port1), motor->pin1, 0UL);
		Cy_GPIO_Write(Cy_GPIO_PortToAddr(motor->port2), motor->pin2, 1UL);
	}

	/* Set the PWM output frequency and duty cycle */
	motor->result = cyhal_pwm_set_duty_cycle(&(motor->pwm), fabs(speed), PWM_FREQUENCY);
	check_status("API cyhal_pwm_set_duty_cycle failed with error code", motor->result);

	/* Start the PWM */
	motor->result = cyhal_pwm_start(&(motor->pwm));
	check_status("API cyhal_pwm_start failed with error code", motor->result);
}

void goToPosition(motor* motor, int32_t pos) {
	int32_t currentPosition = (int32_t)(Cy_TCPWM_QuadDec_GetCounter(TCPWM1, MY_TCPWM_CNT_NUM) - 0x8000);

	// Calculate control output using PID
	int motorOutput = PIDControllerUpdate(&(motor->pid), pos, currentPosition);

	// Ensure that motor output is within a valid range (-max < speed < max)
	motorOutput = clamp(motorOutput, -(motor->max_velocity), motor->max_velocity);

	if (fabs(motorOutput) < MIN_VELOCITY && motorOutput != 0) {
		if (motorOutput > 0)
			motorOutput = MIN_VELOCITY;
		else
			motorOutput = -MIN_VELOCITY;
	}

	// Set motor output
	setMotorOutput(motor, motorOutput);

	printf("CurrentPos: %ld; SetPoint: %ld; Output: %d \r\n", currentPosition, pos, motorOutput);
}

int8_t clamp(int d, int min, int max) {
  const int8_t t = d < min ? min : d;
  return t > max ? max : t;
}
