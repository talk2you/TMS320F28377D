/*
 * pin.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "def.h"

void Gpio_Custom_Setup(void)
{
	InitGpio();

	// LED 1
	GPIO_SetupPinMux(BLINKY_LED1_GPIO, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(BLINKY_LED1_GPIO, GPIO_OUTPUT, GPIO_PUSHPULL);
	// LED 2
	GPIO_SetupPinMux(BLINKY_LED2_GPIO, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(BLINKY_LED2_GPIO, GPIO_OUTPUT, GPIO_PUSHPULL);

	// SCI-A
	GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(28, GPIO_INPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_ASYNC);

	// test
	GPIO_SetupPinMux(20, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(20, GPIO_OUTPUT, GPIO_PUSHPULL);

	// UPP
	InitUpp1Gpio();
}
