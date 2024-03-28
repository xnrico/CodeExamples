#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#define TIMER_ID 1

#define MY_TCPWM_CNT_NUM   (0UL)
#define MY_TCPWM_CNT_MASK  (1UL << MY_TCPWM_CNT_NUM)
#define MY_TCPWM_CNT_VAL   (0x8000UL) 					/* The "neutral" position for 16-bit decoder */

static void MotorTask(void *pvParameters);

static TaskHandle_t xMotorTask;

motor base, back;
motor* base_motor = &base;
motor* back_motor = &back;

int main(void)
{
    cy_rslt_t result;
    result = cybsp_init();			/* Initialize the device and board peripherals */
    if (result != CY_RSLT_SUCCESS) 	/* Board init failed. Stop program execution */
        CY_ASSERT(0);

    __enable_irq(); 				/* Enable global interrupts */

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) 	/* retarget-io init failed. Stop program execution */
        CY_ASSERT(0);

    if (result != CY_RSLT_SUCCESS)	/* GPIO init failed. Stop program execution */
        CY_ASSERT(0);

    if (CY_TCPWM_SUCCESS != Cy_TCPWM_QuadDec_Init(TCPWM1, MY_TCPWM_CNT_NUM, &tcpwm_1_cnt_0_config))
    	CY_ASSERT(0);

    printf("\x1b[2J\x1b[;H"); 		/* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */

    printf("************************ Motor Control ************************ \r\n\n");

    /* Enabled the initialized quadrature decoder */
	Cy_TCPWM_QuadDec_Enable(TCPWM1, MY_TCPWM_CNT_NUM);
	Cy_TCPWM_QuadDec_SetCounter(TCPWM1, MY_TCPWM_CNT_NUM, MY_TCPWM_CNT_VAL);
	Cy_TCPWM_TriggerReloadOrIndex(TCPWM1, MY_TCPWM_CNT_MASK);

	motorInit(base_motor, 1);
	setMotorPolarity(base_motor, +1);
	setMaxVelocity(base_motor, 100);

    // FreeRTOS Setup

    printf("Before FreeRTOS Scheduler \r\n");

    xTaskCreate(MotorTask, ( const char * ) "Motor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xMotorTask);
	vTaskStartScheduler();
}


static void MotorTask(void *pvParameters)
{
	for(;;)
	{
		goToPosition(base_motor, 75);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
/* [] END OF FILE */
