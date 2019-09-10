/*
 * main.c
 *
 *  Created on: 2019. 7. 31.
 *      Author: JJang
 */


#include "../../_BASE_FILE/include/F28x_Project.h"

#define BLINKY_LED1_GPIO    46
#define BLINKY_LED2_GPIO    47

volatile Uint32 Count = 0;
volatile Uint32 Delay = 1e6;


void main(void)
{
	InitSysCtrl();


	InitGpio();
	GPIO_SetupPinMux(BLINKY_LED1_GPIO, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(BLINKY_LED1_GPIO, GPIO_OUTPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(BLINKY_LED2_GPIO, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(BLINKY_LED2_GPIO, GPIO_OUTPUT, GPIO_PUSHPULL);

    DINT;

    InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;


	InitPieVectTable();


	EINT;  // Enable Global interrupt INTM
	ERTM;

	for(;;)
	{
		if(Count++ >= Delay)
		{
			GPIO_WritePin(BLINKY_LED1_GPIO, 0);
			GPIO_WritePin(BLINKY_LED2_GPIO, 1);

			DELAY_US(1000*500);

			GPIO_WritePin(BLINKY_LED1_GPIO, 1);
			GPIO_WritePin(BLINKY_LED2_GPIO, 0);

			DELAY_US(1000*500);
		}
	}
}
