/**
 *  @file   ctype.h
 *  @brief  POSIX like standard string header
 *  $Id: ctype.h,v 1.2 2014/07/15 09:25:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#ifndef __CTYPE_H
#define __CTYPE_H

#ifdef __mips

#define	_U	  1	/* UPPER char       */
#define	_L	  2	/* LOWER char       */
#define	_N	  4	/* NUMBER char      */
#define	_S	  8 /* WHITESPACE char  */
#define _P	 16	/* PUNCTUATION char */
#define _C	 32	/* CONTROL char     */
#define _X	 64	/* HEX DIGIT char   */
#define	_B	128	/* BLANK char	    */


extern const unsigned char _ant_ctype[];
                                                            
/* these are non-ansi but useful macros */

#define toascii(c)	((c)&0177)

static inline int isspace(int c) {
    return ((_ant_ctype)[c] & (_S|_B));
}

static inline int isblank(int c) {
    return ((_ant_ctype)[c] & (_B));
}


static inline int isalpha(int c) {
	return((_ant_ctype)[c] & (_U|_L));
}

static inline int isalnum(int c) {
	return((_ant_ctype)[c] & (_U|_L|_N));
}

static inline int iscntrl(int c) {
	return((_ant_ctype)[c] & _C);
}

static inline  int isdigit(int c) {
	return((_ant_ctype)[c] & _N);
}
static inline int isgraph(int c) {
	/* Printable, Upper, Lower, Numeric */
	return((_ant_ctype)[c] & (_P|_U|_L|_N));
}
static inline int islower(int c) {
	/* Lower only */
	return((_ant_ctype)[c] & (_L));
}
static inline int isprint(int c) {
	/* Printable, Upper, Lower, Numeric */
	return((_ant_ctype)[c] & (_P|_U|_L|_N|_B));
}
static inline int ispunct(int c) {
	return((_ant_ctype)[c] & (_P));
}
static inline int isupper(int c) {
    /* upper only */
	return((_ant_ctype)[c] & (_U));
}
static inline int isxdigit(int c) {
	return((_ant_ctype)[c] & (_X|_N));
}

static inline int toupper(int c) {
	return islower(c) ? (c) - 'a' + 'A' : c;
}

static inline int tolower(int c) {
    return isupper(c) ? (c) - 'A' + 'a' : c;
}

static inline int isascii(int c) {
    return ((c & 0x80) == 0);
}

#else /* not __mips */

#include "/usr/include/ctype.h"
#endif

#endif /* __CTYPE_H */

