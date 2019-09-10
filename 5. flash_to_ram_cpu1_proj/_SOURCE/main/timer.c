/*
 * timer.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "timer.h"

/* --------------------------------------
 *	타이머 설정
 --------------------------------------*/

/* ------------------------------------------------
 * # TIMER 함수
 * ------------------------------------------------
 * @ cpu_timer0_isr 	: cpu_timer0 인터럽트 루틴
------------------------------------------------ */

__interrupt void cpu_timer0_isr(void)
{
   CpuTimer0.InterruptCount++;

   //
   // Acknowledge this interrupt to receive more interrupts from group 1
   //
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


