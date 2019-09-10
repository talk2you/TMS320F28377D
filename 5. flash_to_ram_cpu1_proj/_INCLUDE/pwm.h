/*
 * pwm.h
 *
 *  Created on: 2019. 8. 20.
 *      Author: JJang
 */

#ifndef INCLUDE_PWM_H_
#define INCLUDE_PWM_H_

#include "def.h"

#define	SYSTEM_CLOCK		200E6	// 200MHz
#define	TBCLK				100E6	// 100MHz
#define	PWM_CARRIER			250E3	// 100kHz
#define	PWM_DUTY_RATIO_A	2.3E-1	// 0.23, 23%

void InitEPwm4Module(void);
void PwmOff(void);
void PwmOn(void);
interrupt void EPwm4Isr(void);

extern Uint16	BackTicker;
extern Uint16	EPwm4IsrTicker;

extern float32	PwmCarrierFrequency;
extern float32	PwmDutyRatioA;
extern float32	FallingEdgeDelay;
extern float32	RisingEdgeDelay;

#endif /* INCLUDE_PWM_H_ */
