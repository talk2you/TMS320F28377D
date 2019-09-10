/*
 * main.c
 *
 *  Created on: 2019. 7. 31.
 *      Author: JJang
 */

#include "def.h"

void main(void)
{
	//Uint16 ReceivedChar;
	char *msg;

	InitSysCtrl();

	Gpio_Custom_Setup();

    DINT;

    InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EALLOW;
	PieVectTable.SCIA_RX_INT = &sciaRxFifoIsr;
	EDIS;

	scia_fifo_init();
	scia_init();

	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // PIE Group 9, INT1
	IER |= M_INT9;

	EINT;  // Enable Global interrupt INTM
	ERTM;

	msg = "\r\nHello World!\0";
	scia_msg(msg);

	for(;;)
	{
		msg = "\r\nrunning!\0";
		scia_msg(msg);

		GPIO_WritePin(BLINKY_LED1_GPIO, 0);
		GPIO_WritePin(BLINKY_LED2_GPIO, 1);

		DELAY_US(1000*500);

		GPIO_WritePin(BLINKY_LED1_GPIO, 1);
		GPIO_WritePin(BLINKY_LED2_GPIO, 0);

		DELAY_US(1000*500);
	}
}

