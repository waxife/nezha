/**
 *  @file   stdlib.c
 *  @brief  POSIX like standard symbolic constants and types
 *  $Id: stdlib.c,v 1.3 2014/07/24 06:54:31 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <mipsregs.h>
#include <sys.h>
#include <string.h>
#include <ctype.h>

unsigned int
atoh (const char *ptr)
{
	unsigned int val = 0;

	for (; *ptr; ptr++) {
		if (*ptr >= '0' && *ptr <= '9')
			val = (val << 4) + (*ptr - '0');
		else
		if (*ptr >= 'a' && *ptr <= 'f')
			val = (val << 4) + (*ptr + 10 - 'a');
		else
		if (*ptr >= 'A' && *ptr <= 'F')
			val = (val << 4) + (*ptr + 10 - 'A');
		else
			break;
	}

	return val;
}



int
atoi (const char *ptr)
{
	int val = 0;
    int sign = 1;
    int i = 0;

    if (ptr[0] == '-') {
        sign = -1;
        i++;
    }

    for (; ptr[i] >= '0' && ptr[i] <= '9'; ++i)
        val = val * 10 + ptr[i] - '0';

    return sign * val;
}

unsigned int
stoi (const char *ptr)
{
	if (ptr[0] == '0' && ptr[1] == 'x')
		return atoh (ptr + 2);
	else
		return atoi (ptr);
}

static int holdrand = 0;
void srand(unsigned int seed)
{
    holdrand = (int)seed;
}

int 
rand()
{   
    return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}



unsigned int
size (const char *ptr)
{
    unsigned int length;
    char *unit;

    length = atoi (ptr);
    unit = strpbrk (ptr, "GgMmKk");
    if (unit) {
        switch (toupper (*unit)) {
            case 'G': length <<= 30; break;
            case 'M': length <<= 20; break;
            case 'K': length <<= 10; break;
        }
    }
    //length = (length + (512 - 1)) & ~(512 - 1);

    return length;
}


/* reverse:  reverse string s in place */
void reverse(char s[]){
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];       
        s[i] = s[j];        
        s[j] = c; 
    }
}

void itoa(int n, char s[])
{
    int i, sign;    
    if ((sign = n) < 0)      /* record sign */       
        n = -n;              /* make n positive */    
    i = 0; 
    do {                         /* generate digits in reverse order */        
        s[i++] = n % 10 + '0';   /* get next digit */    
    } while ((n /= 10) > 0);     /* delete it */    
    if (sign < 0)        
        s[i++] = '-';    
    s[i] = '\0';
    reverse(s);
}


