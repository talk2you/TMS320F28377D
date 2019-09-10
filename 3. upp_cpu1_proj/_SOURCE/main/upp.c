/*
 * upp.c
 *
 *  Created on: 2019. 8. 12.
 *      Author: JJang
 */

#include "upp.h"

/* --------------------------------------
 *  UPP to DAC (AD9708) 통신
 *
 *  8bit Parallel 데이터 라인으로 원하는 주파수 출력
 --------------------------------------*/

/* ------------------------------------------------
 * # UPP 함수
 * ------------------------------------------------
 * @ SettingUpp 		: UPP 초기 설정
 * @ DacOff				: DAC 출력 OFF
 * @ DacOn				: DAC 출력 ON
 * @ ConfigFreqUpp		: DAC 주파수 계산
 * @ local_UPPA_ISR		: UPP 인터럽트 루틴 (실제 데이터가 변하는 곳)
------------------------------------------------ */

/*^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+

											Struct

^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^*/

// 16bit + 16bit 데이터 병합
// char형은 32bit 프로세서에서 16bit를 잡아먹는다..
// char형 두개를 쓸때 32bit를 잡아먹음 ㅎ
typedef struct diveded_8bit {
	char s[2];
} DIV_8bit;

union config_32bit{
	DIV_8bit bit_8;
	long int all;
};
union config_32bit var;

/*^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+

											Various

^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^*/

volatile long int ErrCount;
volatile long int WrValue = 0;
volatile int InitTxMsgRam = 0;
volatile int upp_data_idx = 0;
volatile int upp_data_length = 512;
volatile int upp_real_addr = 256;

double dac_freq = 50.0;
long  size_union;
char sine_var;

int off_flag = 0;
int end_int = 0;
int chg_freq = 0;

/*^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+

											Function

^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^`'*._,+^*/

void SettingUpp(void)
{
	int i;

	for (i = 0; i < uPP_TX_MSGRAM_SIZE / 2; i += 2)
		*(Uint32 *) (uPP_TX_MSGRAM_ADDR + i) = 0;

	// Configure uPP for TX
	UppRegs.THCFG.bit.TXSIZEA = uPP_TX_SIZE_256B;
	UppRegs.IFCFG.bit.CLKDIVA = 0x4;      		// Div8.
	UppRegs.IFCFG.bit.CLKINVA = 1;				// Clock Inverse
	UppRegs.CHCTL.bit.MODE 	  = uPP_TX_MODE; 	// Setup for TX.
	UppRegs.CHCTL.bit.DRA     = uPP_SDR;      	// SDR mode
	UppRegs.IFIVAL.all        = 0x0000;         // Idle Value

	// Enable EOL/EOW interrupt
	UppRegs.INTENSET.bit.EOLI = 1;
	UppRegs.INTENSET.bit.EOWI = 1;
	UppRegs.GINTEN.bit.GINTEN = 1;

	// Enable the uPP module
	UppRegs.PERCTL.bit.PEREN = 1;

	// Setup DMA channel
	UppRegs.CHIDESC0 = uPP_TX_MSGRAM_ADDR;
	UppRegs.CHIDESC1.bit.LCNT = LINE_CNT;
	UppRegs.CHIDESC1.bit.BCNT = upp_data_length;
	UppRegs.CHIDESC2.all = 1;
}

void DacOff(void)
{
	off_flag = 1;

	while(1)
	{
		if(end_int)
			break;
	}
	/*EALLOW;
	CpuSysRegs.PCLKCR12.bit.uPP_A = 0;
	EDIS;*/
}

void DacOn(void)
{
	off_flag = 0;
	end_int = 0;

	/*EALLOW;
	CpuSysRegs.PCLKCR12.bit.uPP_A = 1;
	EDIS;*/
}

void ConfigFreqUpp(double* freq)
{
	double div_freq;

	// 분주비 4일때
	// 40kHz ~ 1MHz 정도 출력이 좋음
	if(*freq < 40)
		*freq = 40;
	else if(*freq > 1000)
		*freq = 1000;

	div_freq = 200 / ((UppRegs.IFCFG.bit.CLKDIVA+1) * 2);
	upp_data_length = div_freq * 1000 / *freq;
	upp_real_addr = upp_data_length / 2;

	UppRegs.CHIDESC1.bit.BCNT = upp_data_length;
	chg_freq = 1;
}

// Interrupt routine
interrupt void local_UPPA_ISR(void)
{
	int i,j;

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

	if (UppRegs.GINTFLG.all != 0x0)
	{
		//Check for EOW Interrupt flag
		if (UppRegs.ENINTST.bit.EOWI == 0x1)
		{
			//if(chg_freq)
				InitTxMsgRam = 1;

			//Enable EOL interrupt
			UppRegs.INTENSET.bit.EOLI = 1;

			//Clear the Status for EOW Interrupt
			UppRegs.ENINTST.all = uPP_INT_EOWI;
		}
		// Check for EOL Interrupt Flag
		if (UppRegs.ENINTST.bit.EOLI == 0x1)
		{
			// Disable EOL Interrupt.
			UppRegs.ENINTST.all = uPP_INT_EOLI;
			if (UppRegs.ENINTST.bit.EOLI != 0)
			{
				ErrCount++;
				asm ("      ESTOP0");
				for (;;);
			}
			UppRegs.INTENCLR.all = uPP_INT_EOLI;
			if (UppRegs.INTENSET.bit.EOLI != 0)
			{
				ErrCount++;
				asm ("      ESTOP0");
				for (;;)
					;
			}
			if (InitTxMsgRam == 1)
			{
				for(i=0;i<upp_real_addr;i+= 2)
				{
					if(off_flag)
						WrValue = 0x80808080;
					else
					{
						for(j=0;j<4;j++)
						{
							sine_var = sin((float)upp_data_idx*6.2831853/upp_data_length)*50+128 + sin((float)upp_data_idx*6.2831853/upp_data_length*10)*50 + sin((float)upp_data_idx*6.2831853/upp_data_length*5)*20;
							if(sine_var >= 256)
								sine_var = 255;

							if(j%2 == 0)
								var.bit_8.s[j/2] = sine_var;
							else if(j%2 == 1)
								var.bit_8.s[j/2] |= sine_var << 8;

							upp_data_idx++;
							if(upp_data_idx >= upp_data_length)
								upp_data_idx = 0;
						}
						WrValue = var.all;

					}
					*(Uint32 *) (uPP_TX_MSGRAM_ADDR + i) = WrValue;
				}

				InitTxMsgRam = 0;
			}
			ConfigFreqUpp(&dac_freq);
			UppRegs.CHIDESC0 = uPP_TX_MSGRAM_ADDR;
			UppRegs.CHIDESC1.bit.LCNT = LINE_CNT;
			UppRegs.CHIDESC1.bit.BCNT = upp_data_length;
			UppRegs.CHIDESC2.all = 1;
		}

		if(off_flag)
			end_int = 1;

		chg_freq = 0;
		// Clear Global Interrupt.
		UppRegs.GINTCLR.bit.GINTCLR = 1;
	}
}
