/*
 * main.c
 *
 *  Created on: 2019. 7. 31.
 *      Author: JJang
 */

#include "def.h"
#include "math.h"

Uint16 RamfuncsLoadSize;
Uint16 RamfuncsLoadStart;
Uint16 RamfuncsRunStart;

int cnt = 0;

void init_word(void);

/*^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+

											Main

^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^*/

void main(void)

{
	int sw_stat = 0;

	InitSysCtrl();

	Gpio_Custom_Setup();

    DINT;

    InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EALLOW;
	PieVectTable.TIMER0_INT = &cpu_timer0_isr;
	PieVectTable.SCIA_RX_INT = &sciaRxFifoIsr;
	PieVectTable.EPWM4_INT = &EPwm4Isr;
	PieVectTable.UPPA_INT = &local_UPPA_ISR;
	EDIS;

	InitCpuTimers();
	scia_fifo_init();
	scia_init();

	ConfigCpuTimer(&CpuTimer0, 200, 1000);
	CpuTimer0Regs.TCR.all = 0x4000;

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// Timer 0 Interrupt routine
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   	// SCI-A RX Interrupt routine
	PieCtrlRegs.PIEIER3.bit.INTx4 = 1;		// ePWM Interrupt routine
	PieCtrlRegs.PIEIER8.bit.INTx15 = 1;		// UPP Interrupt routine

	IER |= M_INT1;
	IER |= M_INT3;
	IER |= M_INT8;
	IER |= M_INT9;

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 1;	// 0: EPWMCLK = SYSCLKOUT = 200MHz
												// 1: EPWMCLK = SYSCLKOUT/2 = 100MHz (Default)
	EDIS;

	InitEPwm4Module();

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

	PwmCarrierFrequency = PWM_CARRIER;
	PwmDutyRatioA = PWM_DUTY_RATIO_A;
	FallingEdgeDelay = (1.0 / TBCLK) * EPwm4Regs.DBFED.bit.DBFED;
	RisingEdgeDelay = (1.0 / TBCLK) * EPwm4Regs.DBRED.bit.DBRED;

	EINT;  // Enable Global interrupt INTM
	ERTM;

	SoftResetUpp();
	init_word();
	SettingUpp();

	dac_freq = 400;

	DacOff();
	DELAY_US(500);
	DacOn();

	// 같은 GPxDAT을 연속으로 쓸때 적절한 delay가 필요 >> 첫번째에 수정된 값이 업데이트 되기전에 두번째로 수정된 값이 파이프라인에 들어와 처리되어 GPxDAT 레지스터의 값을 변경시킴
	// GPxSET, GPxCLEAR, GPxTOGGLE 경우 GPIO MUX를 직접 제어하는 Atomic 명령어이므로 피할수 있음.

	GpioDataRegs.GPBSET.bit.GPIO46 = 1;
	GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;

	for(;;)
	{
		if(CpuTimer0.InterruptCount % 100 == 0)
		{
			if(GpioDataRegs.GPADAT.bit.GPIO4 == 0)
			{
				if(sw_stat != 1)
				{
					EPwm4Regs.AQCSFRC.bit.CSFA = 0;
					EPwm4Regs.AQCSFRC.bit.CSFB = 0;

					DacOn();
					GpioDataRegs.GPDCLEAR.bit.GPIO99 = 1;
					sw_stat = 1;
				}
			}
			else
			{
				if(sw_stat != 0)
				{
					EPwm4Regs.AQCSFRC.bit.CSFA = 1;
					EPwm4Regs.AQCSFRC.bit.CSFB = 1;

					DacOff();
					GpioDataRegs.GPDSET.bit.GPIO99 = 1;
					sw_stat = 0;
				}
			}
		}

 		if(CpuTimer0.InterruptCount % 1000 == 0)
		{
			cnt++;

			SendToPc("test : %d\n",cnt);

			GpioDataRegs.GPBTOGGLE.bit.GPIO46 ^= 1;
			GpioDataRegs.GPBTOGGLE.bit.GPIO47 ^= 1;

			if(cnt == 100)
				cnt = 0;
		}
	}
}

void init_word(void)
{
	SendToPc("\n+^`'*._,+^`'*._, Program Start +^`'*._,+^`'*._,\n");
	SendToPc("                Since 2019.08.06\n");
	SendToPc("+^`'*._,    Author : Young Hyun Jang   +^`'*._,\n");
	SendToPc("\n Version 1.0.0\n");
	SendToPc(">> GPIO, Timer, SCI system build\n");

	// salnt
	SendToPc("    ____                                           _____ __             __ \n");
	SendToPc("   / __ \\_________  ____ __________ _____ ___     / ___// /_____ ______/ /_\n");
	SendToPc("  / /_/ / ___/ __ \\/ __ `/ ___/ __ `/ __ `__ \\    \\__ \\/ __/ __ `/ ___/ __/\n");
	SendToPc(" / ____/ /  / /_/ / /_/ / /  / /_/ / / / / / /   ___/ / /_/ /_/ / /  / /_  \n");
	SendToPc("/_/   /_/   \\____/\\__, /_/   \\__,_/_/ /_/ /_/   /____/\\__/\\__,_/_/   \\__/  \n");
	SendToPc("                 /____/                                                    \n");
}
