// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      ili9488_if.c
*@brief     Application level interafce with ILI9488 chip
*@author    Ziga Miklosic
*@date      31.04.2021
*@version	V1.0.1
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup ILI9488_IF
* @{ <!-- BEGIN GROUP -->
*
* 	Application level interface driver for ILI9488 chip
*
*
* 	Put code that is platform depended inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "ili9488_if.h"

// USER INCLUDES BEGIN...
#include "cy_pdl.h"

//#include "drivers/peripheral/gpio/gpio.h"
//#include "drivers/peripheral/timer/timer.h"
//#include "drivers/peripheral/spi/spi.h"

//TODO Add includes for plc spi, and gpio

// USER INCLUDES END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialise interface (peirphery)
*
* @note	User shall put here all hardware dependent initilisation. Those are
* 		GPIO, SPI & TIMER.
*
* 		Not needed to be initilized here but must be initiliezd before calling
* 		"ili9488_init()" function!
*
* @return		status 	- Status of initilization
*/
////////////////////////////////////////////////////////////////////////////////
ili9488_status_t ili9488_if_init(void)
{
	ili9488_status_t status = eILI9488_OK;

	// USER CODE BEGIN...

	// Left empty as periphery is initilize elswhere...
	//TODO

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Control SPI chip select line
*
* @note	User shall provide definition of that function based on used platform!
*
* @param[in]	state	- Logic state of CS line
* @return		void
*/
////////////////////////////////////////////////////////////////////////////////
void ili9488_if_set_cs(const bool state)
{
	// USER CODE BEGIN...

	Cy_SysLib_DelayUs(50);
	//TODO
	if (state)
	{
		Cy_GPIO_Set(P10_3_PORT, P10_3_NUM);
		//CY_SET_REG32(&GPIO_PRT10->OUT, GPIO_PRT_OUT_CLR_OUT3_Msk);
	}
	else
	{
		Cy_GPIO_Clr(P10_3_PORT, P10_3_NUM);
		//CY_SET_REG32(&GPIO_PRT10->OUT_CLR, GPIO_PRT_OUT_CLR_OUT3_Msk);
	}
	Cy_SysLib_DelayUs(2);
	Cy_SysLib_DelayUs(50);

	// USER CODE END...
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Control display DC line
*
* @note	User shall provide definition of that function based on used platform!
*
* @param[in]	state	- Logic state of DC line
* @return		void
*/
////////////////////////////////////////////////////////////////////////////////
void ili9488_if_set_dc(const bool state)
{
	// USER CODE BEGIN...
	//TODO
	if (state)
	{
		Cy_GPIO_Set(P8_0_PORT, P8_0_NUM);
		//CY_SET_REG32(&GPIO_PRT8->OUT, GPIO_PRT_OUT_CLR_OUT0_Msk);
	}
	else
	{
		Cy_GPIO_Clr(P8_0_PORT, P8_0_NUM);
		//CY_SET_REG32(&GPIO_PRT8->OUT_CLR, GPIO_PRT_OUT_CLR_OUT0_Msk);
	}
	Cy_SysLib_DelayUs(1);
	// USER CODE END...
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Control display reset line
*
* @note	User shall provide definition of that function based on used platform!
*
* @param[in]	state	- Logic state of reset line
* @return		void
*/
////////////////////////////////////////////////////////////////////////////////
void ili9488_if_set_reset(const bool state)
{
	// USER CODE BEGIN...

	//todo set value of reset pin
	if (state)
	{
		Cy_GPIO_Set(P9_6_PORT, P9_6_NUM);
		//CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT6_Msk);
	}
	else
	{
		Cy_GPIO_Clr(P9_6_PORT, P9_6_NUM);
		//CY_SET_REG32(&GPIO_PRT9->OUT_CLR, GPIO_PRT_OUT_CLR_OUT6_Msk);
	}

	// USER CODE END...
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Control of LED dimming timer
*
* @note	User shall provide definition of that function based on used platform!
*
* @param[in]	brightness	- Brightness of backlight display LED
* @return		void
*/
////////////////////////////////////////////////////////////////////////////////
void ili9488_if_set_led(const float brigthness)
{
	// USER CODE BEGIN...

	//TODO we might not want to have this. I'm thinking set brightness to vcc and call it a day. Or maybe just a digital pin.
	//timer_set_pwm( eTIMER_TIM1_CH4_D_LED, brigthness );
	if (brigthness > 0.0f)
	{
		Cy_GPIO_Set(P9_7_PORT, P9_7_NUM);
		//CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT7_Msk);
	}
	else
	{
		Cy_GPIO_Clr(P9_7_PORT, P9_7_NUM);
		//CY_SET_REG32(&GPIO_PRT9->OUT_CLR, GPIO_PRT_OUT_CLR_OUT7_Msk);
	}

	// USER CODE END...
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Transmit bytes via SPI
*
* @param[in]	p_data	- Pointer to transmit data
* @param[in]	size 	- Number of data to transmit
* @return		status 	- Status of transmittion
*/
////////////////////////////////////////////////////////////////////////////////
ili9488_status_t ili9488_if_spi_transmit(const uint8_t * p_data, const uint32_t size)
{
	ili9488_status_t status = eILI9488_OK;

	// USER CODE BEGIN...

	//TODO code for spi output
	//Cy_SCB_SPI_WriteArrayBlocking(SCB6, p_data, size);
	//Cy_SCB_SPI_IsTxComplete(SCB6);
	Cy_SCB_SPI_WriteArrayBlocking(SCB6, p_data, size);
	while(!Cy_SCB_SPI_IsTxComplete(SCB6));
	//Cy_SysLib_DelayUs(25);

	/*if ( eSPI_OK != spi_display_transmit((uint8_t*) p_data, size ))
	{
		status = eILI9488_ERROR_SPI;
	}*/

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive bytes via SPI
*
* @param[in]	p_data	- Pointer to received data
* @param[in]	size 	- Number of data to receive
* @return		status 	- Status of reception
*/
////////////////////////////////////////////////////////////////////////////////
uint8_t emptyBuf[255] = {};
ili9488_status_t ili9488_if_spi_receive(uint8_t * p_data, const uint32_t size)
{
	ili9488_status_t status = eILI9488_OK;

	// USER CODE BEGIN...
	//Cy_GPIO_Clr(P9_7_PORT, P9_7_NUM);
	//CY_SET_REG32(&GPIO_PRT9->OUT_CLR, GPIO_PRT_OUT_CLR_OUT7_Msk);

	//Empty RX FIFO
	Cy_SCB_SPI_ClearRxFifo(SCB6);


	Cy_SCB_SPI_WriteArrayBlocking(SCB6, emptyBuf, size);
	while(!Cy_SCB_SPI_IsTxComplete(SCB6));

	Cy_SCB_SPI_ReadArray(SCB6, p_data, size); //returns number of elements read

	//Cy_GPIO_Set(P9_7_PORT, P9_7_NUM);
	//CY_SET_REG32(&GPIO_PRT9->OUT, GPIO_PRT_OUT_CLR_OUT7_Msk);

	/*if ( eSPI_OK != spi_display_receive( p_data, size ))
	{
		status = eILI9488_ERROR_SPI;
	}*/

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
