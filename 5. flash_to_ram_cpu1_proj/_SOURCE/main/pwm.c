/*
 * pwm.c
 *
 *  Created on: 2019. 8. 20.
 *      Author: JJang
 */

#include "pwm.h"

/* --------------------------------------
 *  Buck Regulator를 사용하기 위해
 *
 *  ePWM을 출력하여 0~48Vdc 사이의 전압을 출력을 위함
 *  현재는 11vdc 출력
 --------------------------------------*/

/* ------------------------------------------------
 * # PWM 함수
 * ------------------------------------------------
 * @ InitEPwm4Module 	: PWM 초기 설정
 * @ PwmOff				: PWM 출력 OFF
 * @ PwmOn				: PWM 출력 ON
 * @ EPwm4Isr			: PWM 인터럽트 루틴
------------------------------------------------ */

Uint16	BackTicker;
Uint16	EPwm4IsrTicker;

float32	PwmCarrierFrequency;
float32	PwmDutyRatioA;
float32	FallingEdgeDelay;
float32	RisingEdgeDelay;

void InitEPwm4Module(void)
{
	// Setup Counter Mode and Clock
	EPwm4Regs.TBCTL.bit.CTRMODE = 0;		// Count Up (Asymmetric)
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0;		// TBCLK = SYSCLKOUT / EPWMCLKDIV / (HSPCLKDIV * CLKDIV) = 100MHz
	EPwm4Regs.TBCTL.bit.CLKDIV = 0;

	// Setup Phase
	EPwm4Regs.TBPHS.bit.TBPHS = 0;			// Phase is 0
	EPwm4Regs.TBCTL.bit.PHSEN = 0;			// Disable phase loading

	// Setup Period (Carrier Frequency)
	EPwm4Regs.TBPRD = (TBCLK/PWM_CARRIER)-1;	// Set Timer Period, (100MHz/100KHz)-1 = 999 (0x03E7)
	EPwm4Regs.TBCTR = 0;						// Clear Counter

	// Set Compare Value
	EPwm4Regs.CMPA.bit.CMPA = (Uint16)((EPwm4Regs.TBPRD + 1) * PWM_DUTY_RATIO_A);	// Set Compare A Value to 50%

	// Setup shadowing
	EPwm4Regs.TBCTL.bit.PRDLD = 0;			// Period Register is loaded from its shadow when CNTR=Zero
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = 0;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm4Regs.CMPCTL.bit.LOADAMODE = 0;

	// Set actions
	EPwm4Regs.AQCTLA.bit.ZRO = 2;			// Set EPWM4A on CNTR=Zero
	EPwm4Regs.AQCTLA.bit.CAU = 1;			// Clear EPWM4A on CNTR=CMPA, Up-Count

	// Set Dead-time
	EPwm4Regs.DBCTL.bit.IN_MODE = 0;		// EPWMxA is the source for both falling-edge & rising-edge delay
	EPwm4Regs.DBCTL.bit.OUT_MODE = 3;		// Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
	EPwm4Regs.DBCTL.bit.POLSEL = 2;			// Active High Complementary (AHC). EPWMxB is inverted
	EPwm4Regs.DBFED.bit.DBFED = 4;		// 2usec, Falling Edge Delay
	EPwm4Regs.DBRED.bit.DBRED = 4;		// 2usec, Rising Edge Delay

	// Set Interrupts
	EPwm4Regs.ETSEL.bit.INTSEL = 1;			// Select INT on CNTR=Zero
	EPwm4Regs.ETPS.bit.INTPRD = 1;			// Generate INT on 1st event
	EPwm4Regs.ETSEL.bit.INTEN = 1;			// Enable INT

	// ePWM Forced Low Level
	EPwm4Regs.AQCSFRC.bit.CSFA = 1;			// Forces low output A
	EPwm4Regs.AQCSFRC.bit.CSFB = 1;			// Forces low output B
}

void PwmOff(void)
{
	EPwm4Regs.AQCSFRC.bit.CSFA = 1;			// Forces low output A
	EPwm4Regs.AQCSFRC.bit.CSFB = 1;			// Forces low output B
}

void PwmOn(void)
{
	EPwm4Regs.AQCSFRC.bit.CSFA = 0;			// output mode A
	EPwm4Regs.AQCSFRC.bit.CSFB = 0;			// output mode B
}

interrupt void EPwm4Isr(void)
{
	EPwm4IsrTicker++;

	EPwm4Regs.TBPRD = (TBCLK / PwmCarrierFrequency) - 1;
	EPwm4Regs.CMPA.bit.CMPA = (EPwm4Regs.TBPRD + 1) * PwmDutyRatioA;

	// Clear INT flag for this timer
	EPwm4Regs.ETCLR.bit.INT = 1;

	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.bit.ACK3 = 1;
}
