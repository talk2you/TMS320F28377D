/*
 * pin.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "def.h"

/* --------------------------------------
 *	Pin Setting
 *
 *	GPIO 입출력 설정
 --------------------------------------*/

/* ------------------------------------------------
 * # PIN 함수
 * ------------------------------------------------
 * @ Gpio_Custom_Setup 	: 핀 설정
------------------------------------------------ */

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

	// DAC-OPAMP Test
	GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(4, GPIO_INPUT, GPIO_PUSHPULL);

	GPIO_SetupPinMux(99, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(99, GPIO_OUTPUT, GPIO_PUSHPULL);

	// ePWM
	GPIO_SetupPinMux(6, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(6, GPIO_OUTPUT, GPIO_PUSHPULL);

	GPIO_SetupPinMux(7, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(7, GPIO_OUTPUT, GPIO_PUSHPULL);
}
