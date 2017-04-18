/**
 *  @file   printf.c
 *  @brief  POSIX like printf function
 *  $Id: printf.c,v 1.4 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>
#include <debug.h>

static char do_printf = 1;

void disable_printf(void)
{
    do_printf--;
}

void enable_printf(void)
{
    do_printf++;
}

#define PRINTBUF_SIZE   128

static char _print_buf[PRINTBUF_SIZE];

#if 0
__mips16__
int _vprintf(const char *fmt, va_list args)
{
    int size;
    char *p;

    ASSERT_UNDER_STACK();
    size = __doprnt(_print_buf, PRINTBUF_SIZE, fmt, args);
    _print_buf[PRINTBUF_SIZE-1] = '\0';
    p = _print_buf;
    while(*p)
        putchar(*p);

    return size;
}

__mips16__
int printf16(const char *fmt, ...)
{
    va_list ap;
    int size;

    va_start(ap, fmt);
    size = _vprintf(fmt, ap);
    va_end(ap);

    return size;
}

#else

int __printf(const char *fmt, ...)
{
    char *p;
    va_list ap;
    int size;

    ASSERT_UNDER_STACK();
    va_start(ap, fmt);
    size = __doprnt(_print_buf, PRINTBUF_SIZE, fmt, ap);
    va_end(ap);

    _print_buf[PRINTBUF_SIZE-1] = '\0';
    p = _print_buf;
	while (*p)
		putchar (*p++);

    return size;
}

__mips16__
int printf16(const char *fmt, ...)
{
    char *p;
    va_list ap;
    int size;

    if (do_printf <= 0)
        return 0;

    ASSERT_UNDER_STACK();
    va_start(ap, fmt);
    size = __doprnt(_print_buf, PRINTBUF_SIZE, fmt, ap);
    va_end(ap);

    _print_buf[PRINTBUF_SIZE-1] = '\0';
    p = _print_buf;
	while (*p)
		putchar (*p++);

    return size;
}
#endif

__mips16__
int sprintf16(char *str, const char *fmt, ...)
{
    va_list ap;
    int size;
    va_start(ap, fmt);
    size = __doprnt(str, 65536, fmt, ap);
    va_end(ap);

    return size;
}

__mips16__
int snprintf16(char *str, unsigned int ssize, const char *fmt, ...)
{
    va_list ap;
    int size;
    va_start(ap, fmt);
    size = __doprnt(str, ssize, fmt, ap);
    va_end(ap);

    return size;
}



#if 0
int main () {
	printf ("[c] %c\n", 'c');
	printf ("[s] %s\n", "this is a string");
	printf ("[d] %d, %d\n", 123456, -123456);
	printf ("[x] %x, %8x, %4x, %3x\n", 0x12, 0x1234, 0x12345678, 0x12);
	
	printf ("[d] %d,%2d,%d,%d,%5d,%5d,%5d,%5d\n", 
			0, 0, 123, -123, 123, -123, 123456, -123456);
	printf("[d] %9d\n", 1234567890);
	printf("[x] %8x\n", 15);
}
#endif

