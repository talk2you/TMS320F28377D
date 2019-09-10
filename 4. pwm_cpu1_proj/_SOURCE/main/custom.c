/*
 * custom.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "custom.h"
#include "string.h"

//stdlib.h의 atoi를 사용하는것보다 279byte 절약
int atoi(char const *c)
{
	int value = 0;
	int positive = 1;

	if(*c == '\0')
		return 0;

	if(*c == '-')
		positive = -1;

	while(*c) {
		if(*c >= '0' && *c <= '9')
			value = value * 10 + *c - '0';
		c++;
	}

	return value*positive;
}

//stdlib.h의 ltoa를 사용하는것보다 43byte 절약
char *itoa( char *a, int i)
{
	int sign=0;
	char buf[16] = {0,};
	char *ptr;

	ptr = buf;

	if( i )
	{
		if(i < 0)
		{
			i = ~i + 1;
			sign++;
		}
		while(i)
		{
			*ptr++ = (i % 10) + '0';
			i = i / 10;
		}
		if(sign)
			*ptr++ = '-';

		*ptr = '\0';

		for(i=0;i<strlen(buf);i++)
			*a++ = buf[strlen(buf)-i-1];
	}
	else
		*a++ = '0';

	return a;
}

//stdio.h의 vsnprintf() 를 사용하는 것보다 4375byte 절약
long cst_vsprintf(char* buf, char* format, char* arg)
{
	char *start=buf;
	char *p;

    while( *format )
	{
		if( *format != '%' )
		{
			*buf++ = *format++;
			continue;
		}

		format++;

		if( *format == '%' )
		{
			*buf++ = *format++;
			continue;
		}

		switch( *format )
		{
			case 'c' :
				*buf++ = va_arg(arg,char);
				break;

			case 'd' :
				buf = itoa(buf,va_arg(arg,int));
				break;

			case 's' :
				p=va_arg(arg,char*);
				while(*p)
						*buf++ = *p++;
				break;

			default :
				*buf++ = *format;
				break;
        }

		format++;
    }

    *buf = '\0';

    return (buf-start);
}


