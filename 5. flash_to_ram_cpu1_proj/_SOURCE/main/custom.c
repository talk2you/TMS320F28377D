/*
 * custom.c
 *
 *  Created on: 2019. 8. 6.
 *      Author: JJang
 */

#include "custom.h"
#include "string.h"

/* --------------------------------------
 *  Customized function
 *
 *  ������ ���̺귯���� �ִ� �Լ��� ����ȭ ��Ŵ
 --------------------------------------*/

/* ------------------------------------------------
 * # CUSTOM �Լ�
 * ------------------------------------------------
 * @ atoi 			: ASCII to Int �� ��ȯ
 * @ itoa			: Int to ASCII �� ��ȭ
 * @ cst_vsprintf	: customized vsprintf
------------------------------------------------ */

//stdlib.h�� atoi�� ����ϴ°ͺ��� 279byte ����
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

//stdlib.h�� ltoa�� ����ϴ°ͺ��� 43byte ����
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

//stdio.h�� vsnprintf() �� ����ϴ� �ͺ��� 4375byte ����
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

