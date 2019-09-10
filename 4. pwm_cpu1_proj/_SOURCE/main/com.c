/*
 * com.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "com.h"
#include "sci.h"
#include "custom.h"

va_list ap;

void SendToPc(char* format, ...)
{
	char buf[100];

	va_start(ap, format);
	cst_vsprintf(buf,format,ap);
	va_end(ap);

	scia_msg(buf);
}


