/******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for the <CE Title> Example
 *              for ModusToolbox.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * Copyright 2021-2023, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

extern "C" {
#include "cy_pdl.h" /* Peripheral Driver Library */
#include "cyhal.h"
#include "cybsp.h" /* Board specific pin and peripheral definitions */
#include "cy_retarget_io.h"

#include "FreeRTOS.h"
#include "task.h"

#include "capsense.h"
#include "cycfg.h" /* Auto-generated system configuration headers */

#include "cycfg_qspi_memslot.h" /*QSPI external memory configuration structures */
#include "cy_serial_flash_qspi.h" /* QSPI library functions */

#include "cy_sysclk.h"

#include "ili9488.h" //display library
#include "bitmap.h"
}
#include "CircFifo.h"

TaskHandle_t test_task_handle;

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 * This is the main function for CM4 CPU. It does
 *    1. Initialization of board resources.
 *    2. Retains minimum amount of SRAM needed by the example.
 *    3. Creation of CapSense task.
 *    4. Start FreeRTOS scheduler.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/
/* Main configuration parameters*/
#define NUMBER_OF_EXTERNAL_MEM      (1U)
#define SMIF_PRIORITY               (1U)
#define TIMEOUT_1_S                 (1000U)
#define DESELECT_DELAY              (7U)
/* SMIF interrupt initialization */
#if defined (CY_IP_M4CPUSS)
#define SMIF_INTERRUPT smif_interrupt_IRQn
#elif defined (CY_IP_M33SYSCPUSS)
#define SMIF_INTERRUPT smif_interrupt_normal_IRQn
#else /* M7 CPUSS */
#define SMIF_INTERRUPT smif_0_interrupt_IRQn
#endif

#define MEM_SIZE (320U * 480U + 1U)
CircFifo rxFifo;
cy_stc_scb_spi_context_t sContext;
cy_stc_scb_spi_context_t mContext;
cy_stc_scb_spi_context_t mContext2;
cy_stc_smif_context_t smifText;


void SMIF_Interrupt_User(void)
{
	Cy_SMIF_Interrupt(SMIF0, &smifText);
}

void test_task(void *arg) {
	uint16_t loops = 0;
	cy_rslt_t result;
	rxFifo = CircFifo();
	uint8 txBuffer[64];
	txBuffer[0] = 0xAA;
	const uint8_t mem[100] = {0};


	ili9488_init();

	bool is_init = false;

	/*CY_SET_REG32(&GPIO_PRT8->OUT_CLR, GPIO_PRT_OUT_CLR_OUT0_Msk);
	CY_SET_REG32(&GPIO_PRT9->OUT_CLR, GPIO_PRT_OUT_CLR_OUT6_Msk);
	CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT7_Msk);*/


	for(;;) {
		is_init = ili9488_is_init();
		ili9488_status_t error = eILI9488_OK;
		if (!is_init) {
			error = eILI9488_ERROR;
		}


		ili9488_rect_attr_t rect = {
				// Position of rectangle
				.position =
				{
					.start_page = 100,
					.start_col = 100,
					.page_size = 1,
					.col_size = 1
				},

				// Rounded corners
				.rounded =
				{
					.enable = false,
					.radius = 0
				},

				// Filled rectangle
				.fill =
				{
					.enable = true,
					.color = eILI9488_COLOR_GREEN
				},

				// Rectangle with border
				.border =
				{
					.enable = false,
					.color = eILI9488_COLOR_PURPLE,
					.width = 0
				}

			};


		error = ili9488_set_background( eILI9488_COLOR_GREEN );
		//ili9488_draw_rectangle(&rect);
		ili9488_read_memory	((uint8_t *) &mem, 100);
		if (mem[1] != eILI9488_COLOR_GREEN) {
			error = eILI9488_ERROR;
		}
		vTaskDelay(1000);

		error = ili9488_set_background( eILI9488_COLOR_BLUE );
		ili9488_read_memory	((uint8_t *) &mem, 100);
		if (mem[2] != eILI9488_COLOR_BLUE) {
			error = eILI9488_ERROR;
		}
		vTaskDelay(1000);
		ili9488_set_background( eILI9488_COLOR_RED );
		ili9488_set_cursor(0,0);
		ili9488_read_memory	((uint8_t *) &mem, 100);
		if (mem[0] != eILI9488_COLOR_RED) {
			error = eILI9488_ERROR;
		}

		ili9488_draw_bitmap((uint8_t *)&img);

		vTaskDelay(1000);
	}
}

int main(void)
{
	cy_rslt_t result;

#if defined(CY_DEVICE_SECURE)
	cyhal_wdt_t wdt_obj;
	/* Clear watchdog timer so that it doesn't trigger a reset */
	result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
	CY_ASSERT(CY_RSLT_SUCCESS == result);
	cyhal_wdt_free(&wdt_obj);
#endif

	/* Initialize the device and board peripherals */
	result = cybsp_init() ;
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	cy_stc_sysint_t smifIntConfig =
	{
#if (CY_CPU_CORTEX_M0P)
			/* .intrSrc */ NvicMux7_IRQn,
			/* .cm0pSrc */ SMIF_INTERRUPT,
#else
			/* .intrSrc */ SMIF_INTERRUPT, /* SMIF interrupt number (non M0 core)*/
#endif
			/* .intrPriority */ SMIF_PRIORITY
	};
	(void) Cy_SysInt_Init(&smifIntConfig, SMIF_Interrupt_User);


	//SPI crap

	//Cy_SCB_SPI_Init(SCB1, &scb_1_config, &mContext);
	Cy_SCB_SPI_Init(SCB2, &scb_2_config, &mContext);
	cy_en_scb_spi_status_t status = Cy_SCB_SPI_Init(SCB6, &scb_6_config, &mContext2);
	if (status != CY_SCB_SPI_SUCCESS) {
		while(1);
	}

	Cy_SysClk_PeriphAssignDivider(PCLK_SCB6_CLOCK, CY_SYSCLK_DIV_8_BIT, 0);
	Cy_SysClk_PeriphSetDivider   (CY_SYSCLK_DIV_8_BIT, 0, 4UL);
	Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 0);


	Cy_GPIO_SetHSIOM(P13_1_PORT, P13_1_NUM, P13_1_SCB6_SPI_MISO); //MISO
	Cy_GPIO_SetHSIOM(P13_0_PORT, P13_0_NUM, P13_0_SCB6_SPI_MOSI); //MOSI
	Cy_GPIO_SetHSIOM(P13_2_PORT, P13_2_NUM, P13_2_SCB6_SPI_CLK); //SCLK

	Cy_GPIO_SetDrivemode(P13_1_PORT, P13_1_NUM, CY_GPIO_DM_HIGHZ); //MISO
	Cy_GPIO_SetDrivemode(P13_0_PORT, P13_0_NUM, CY_GPIO_DM_STRONG_IN_OFF); //MOSI
	Cy_GPIO_SetDrivemode(P13_2_PORT, P13_2_NUM, CY_GPIO_DM_STRONG_IN_OFF); //SCLK


	Cy_SCB_SPI_Enable(SCB2);
	Cy_SCB_SPI_Enable(SCB6);

	//set GPIO initial vals
	CY_SET_REG32(&GPIO_PRT10->OUT, GPIO_PRT_OUT_CLR_OUT3_Msk); //display ss
	CY_SET_REG32(&GPIO_PRT8->OUT, GPIO_PRT_OUT_CLR_OUT0_Msk); //display dc
	CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT6_Msk); //display rst
	CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT7_Msk); //display blk

	/* Enable global interrupts */
	__enable_irq();

	/* Enable SMIF interrupt */
#if (__CORTEX_M == 0)
	NVIC_EnableIRQ(NvicMux7_IRQn);
#else
	NVIC_EnableIRQ(SMIF_INTERRUPT);
#endif

	/* Create the CapSense task */
	xTaskCreate(test_task, "Test Task", 512,
			NULL, 3, &test_task_handle);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never get here! */
	/* Halt the CPU if scheduler exits */
	CY_ASSERT(0);

	for(;;)
	{
	}

}

/* [] END OF FILE */
