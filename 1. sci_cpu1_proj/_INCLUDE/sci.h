/*
 * sci.h
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "def.h"

#ifndef INCLUDE_SCI_H_
#define INCLUDE_SCI_H_

void scia_fifo_init();
void scia_init();
void scia_msg(char * msg);
void scia_tx(int a);
interrupt void sciaRxFifoIsr(void);

extern char rdataA;

#endif /* INCLUDE_SCI_H_ */
