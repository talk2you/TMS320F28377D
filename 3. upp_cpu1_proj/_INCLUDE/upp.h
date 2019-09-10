/*
 * upp.h
 *
 *  Created on: 2019. 8. 12.
 *      Author: JJang
 */

#ifndef INCLUDE_UPP_H_
#define INCLUDE_UPP_H_

#include "def.h"
#include "math.h"

// Define
#define LINE_CNT       	0			// DMA interrupt АЃАн

// External Various
extern volatile int upp_data_length;
extern volatile int real_upp_addr;
extern double dac_freq;

// Function
void InitUpp1Gpio(void);
void SettingUpp(void);
void SoftResetUpp(void);

void DacOff(void);
void DacOn(void);

void ConfigFreqUpp(double* freq);

interrupt void local_UPPA_ISR(void);


#endif /* INCLUDE_UPP_H_ */
