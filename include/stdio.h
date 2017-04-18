/**
 *  @file   stdio.h
 *  @brief  POSIX like standard io header
 *  $Id: stdio.h,v 1.4 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#ifndef __STDIO_H__
#define __STDIO_H__

#include <linker.h>

#ifdef __mips

#include <stdarg.h>

#define NULL		0

#define ESC		0x1b
#define BKT		0x5b
#define WAV		0x7e
#define BACKSPACE	0x08

#define CURSOR_UP	(256 + 0)	// (ESC, BKT, 'A')
#define CURSOR_DOWN	(256 + 1)	// (ESC, BKT, 'A')
#define CURSOR_RIGHT	(256 + 2)	// (ESC, BKT, 'C')
#define CURSOR_LEFT	(256 + 3)	// (ESC, BKT, 'D')
#define HOME		(256 + 4)	// (ESC, BKT, '1', '~')
#define INSERT		(256 + 5)	// (ESC, BKT, '2', '~')
#define DELETE		(256 + 6)	// (ESC, BKT, '3', '~')
#define END		(256 + 7)	// (ESC, BKT, '4', '~')
#define PAGE_DOWN	(256 + 8)	// (ESC, BKT, '5', '~')
#define PAGE_UP		(256 + 9)	// (ESC, BKT, '6', '~')

#define STR_CNT		8
#define STR_SIZE	1024

extern int putchar (int);
extern int getchar (void);

extern char *gets (char *buf);
extern int puts (const char *buf);

//extern int printf (const char *fmt, ...) __attribute__((format (printf, 1, 2)));
//extern int sprintf(char *str, const char *format, ...) __attribute__((format (printf, 2, 3)));
//extern int snprintf(char *str, unsigned int size, const char *format, ...) __attribute__((format (printf, 3, 4)));
//extern __mips16__ int _vprintf(const char *fmt, va_list va_ap);

extern int __printf(const char *fmt, ...) __attribute__((format (printf, 1, 2)));

extern __mips16__ int sprintf16(char *str, const char *format, ...) __attribute__((format (printf, 2, 3)));
extern __mips16__ int snprintf16(char *str, unsigned int size, const char *format, ...) __attribute__((format (printf, 3, 4)));

extern __mips16__ int printf16(const char *fmt, ...) __attribute__((format (printf, 1, 2)));

extern __mips16__ int __doprnt(char *obuf, int base, const char *fmt0, va_list argp);

extern void disable_printf(void);
extern void enable_printf(void);

#define printf(fmt, arg...)                 printf16(fmt, ##arg)
#define sprintf(str, fmt, arg...)           sprintf16(str, fmt, ##arg)
#define snprintf(str, size, fmt, arg...)    snprintf16(str, size, fmt, ##arg)

#else /* not __mips */

#include "/usr/include/stdio.h"

#endif

#endif /* __STDIO_H__ */
