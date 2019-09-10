/*
 * def.h
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#ifndef INCLUDE_DEF_H_
#define INCLUDE_DEF_H_

#include "../../_BASE_FILE/include/F28x_Project.h"
#include "com.h"
#include "sci.h"
#include "timer.h"
#include "upp.h"

#define BLINKY_LED1_GPIO    46
#define BLINKY_LED2_GPIO    47

void Gpio_Custom_Setup(void);

#endif /* INCLUDE_DEF_H_ */
