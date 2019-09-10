/*
 * sci.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "sci.h"

/* --------------------------------------
 *  SCI to PC 통신을 하기위한 파일
 *
 *  SCI 통신을 하기위한 설정
 --------------------------------------*/

/* ------------------------------------------------
 * # SCI 함수
 * ------------------------------------------------
 * @ scia_fifo_init 	: SCI FIFO 설정
 * @ scia_init			: SCI 기능 설정 초기화
 * @ scia_tx			: SCI TX 함수
 * @ scia_msg			: SCI 문자열 보내는 함수
 * @ sciaRxFifoIsr		: SCI RX 인터럽트 루틴
------------------------------------------------ */

char rdataA;

void scia_fifo_init()
{
    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x0021;
    SciaRegs.SCIFFCT.all = 0x0;
}

void scia_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                    // No parity,8 char bits,
                                    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                    // Disable RX ERR, SLEEP, TXWAKE

    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
    // 50000000 / (250000 * 8) - 1 = 24
    SciaRegs.SCIHBAUD.all = 0x0000;
    SciaRegs.SCILBAUD.all = 0x0018;

    SciaRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset

    SciaRegs.SCIFFTX.bit.TXFIFORESET = 1;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
}

void scia_tx(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all = a;
}

void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_tx(msg[i]);
        i++;
    }
}

interrupt void sciaRxFifoIsr(void)
{
    rdataA=SciaRegs.SCIRXBUF.all;  // Read data

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag

    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}


