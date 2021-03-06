/**
 *  @file   doprnt.c
 *  @brief  format print function core of sprintf, printf, vsprintf, snprintf
 *  $Id: doprnt2.c,v 1.2 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char   uchar;
typedef unsigned long   ulong;
typedef unsigned long long ullong;

#define	LONGINT		0x01	/* long integer */
#define	FLOAT		0x02	/* float */
#define	SHORTINT	0x04	/* short integer */
#define	ALT		    0x08	/* alternate form */
#define	LADJUST		0x10	/* left adjustment */
#define	ZEROPAD		0x20	/* zero (as opposed to blank) pad */
#define	HEXPREFIX	0x40	/* add 0x or 0X prefix */
#define LONGLONG    0x100   /* long long mark */

#define ARG(signedtype) 	\
    ({_ulong = flags & LONGINT ? \
            (signedtype long) va_arg(argp, long) : \
            ((flags & SHORTINT) ? (signedtype short) va_arg(argp, int) : \
             (signedtype int) va_arg(argp, int)); })
#define	todigit(c) ((c) - '0')
#define	tochar(n) ((n) + '0')

#define BUFSIZE     32


// #define SUPPORT_LF

#ifdef SUPPORT_LF
static int ipow(int base, int exp)
{
    int result = 1;
    while (exp) {
        if (exp & 1)
            result *= base; 
        exp >>= 1; 
        base *= base; 
    } 
    
    return result;
}

static int fdec(double d)
{
    int i = 0;
    while(d > 1.0) {
        d /= 10;
        i++;
    }

    return i;
}
#endif


__mips16__
int  __doprnt(char *obuf, int bsize, const char *fmt0, va_list argp)
{
    uchar *fmt;		/* format string */
    int ch;			/* character from fmt */
    int cnt;		/* return value accumulator */
    int n;			/* random handy integer */
    char *t;		/* buffer pointer */
	ulong _ulong = 0;		/* integer arguments %[diouxX] */
    ullong _ullong = 0;     /* long long arguments %lld */
#ifdef SUPPORT_LF
    double _double = 0;     /* double %lf */
    ulong _fract = 0;
#endif    
	int base = 0;		/* base for [diouxX] conversion */
	int dprec;		/* decimal precision in [diouxX] */
	int fieldsz;	/* field size expanded by sign, etc */
	int flags;		/* flags as above */
	int fpprec;		/* `extra' floating precision in [eEfgG] */ 
	int prec;		/* precision from format (%.3d), or -1 */
	int realsz;		/* field size expanded by decimal precision */
	int size;		/* size of converted field or string */
	int width;		/* width from format (%8d), or 0 */
	char sign;		/* sign prefix (' ', '+', '-', or \0) */
	char *digs;		/* digits for [diouxX] conversion */
	char buf[BUFSIZE];	/* space for %c, %[diouxX], %[eEfgG] */

    fmt = (uchar *)fmt0;
	digs = "0123456789abcdef";
	for (cnt = 0; cnt < bsize ; ++fmt) {
		n = 0;
		for (; (ch = *fmt) && ch != '%' && cnt < bsize; ++cnt, ++fmt) {
			*obuf++ = ch;
		}

		if (!ch) {
			*obuf++ = '\0';
			return (cnt);
		} else if (cnt == bsize) {
            *obuf = '\0';
            return (cnt-1);
        }

		flags = 0;
		dprec = 0;
		fpprec = 0; 
		width = 0;
		prec = -1;
		sign = '\0';

rflag:	switch (*++fmt) {
		case ' ':
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign) {
				sign = ' ';
			}
			goto rflag;	
	
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			/*
			 * ``A negative field width argument is taken as a
			 * - flag followed by a  positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			if ((width = va_arg(argp, int)) >= 0) {
				goto rflag;
			}
			width = -width;
			/*
			 * FALLTHROUGH
			 */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if (*++fmt == '*') {
				n = va_arg(argp, int);
			} else {
				n = 0;
				while (isascii(*fmt) && isdigit(*fmt)) {
					n = 10 * n + todigit(*fmt++);
				}
				--fmt;
			}
			prec = n < 0 ? -1 : n;
			goto rflag;
		case '0':
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			while (isascii(*fmt) && isdigit(*fmt)) {
				n = 10 * n + todigit(*fmt++);
			}
			--fmt;
			width = n;
			goto rflag;
#if 0			
		case 'L':
			flags |= LONGDBL;
			goto rflag;
#endif
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
            if ((flags & LONGINT) == LONGINT)
                flags |= LONGLONG;
            else
			    flags |= LONGINT;
			goto rflag;
		case 'c':
			*(t = buf) = va_arg(argp, int);
			// egg responsc: when he printed '\0', the string was breaked
			// while other systems don't.
			if((*t) == '\0')
				*t = ' ';
			size = 1;
			sign = '\0';
			goto pforw;
        case 'F':
            flags |= LONGINT;

        case 'f':
#ifdef SUPPORT_LF
            flags |= FLOAT;
            _double = va_arg(argp, double);

            prec = ((prec < 0) ? 6 : prec);
            if (_double < 0) {
                _double = -_double;
                sign = '-';
            }
            _ullong = (unsigned long long)_double;
            _fract =  ((_double - _ullong) * ipow(10, prec)) + 0.5;
            goto number;
#endif
		case 'D':
			flags |= LONGINT;
			/*
			 * FALLTHROUGH
			 */
		case 'd':
		case 'i':
            if (flags & LONGLONG) {
                _ullong = va_arg(argp, long long);
                if ((long long) _ullong < 0) {
                    _ullong = -_ullong;
                    sign = '-';
                }
            } else {
			    ARG(signed);
			    if ((long)_ulong < 0) {
				    _ulong = -_ulong;
				    sign = '-';
			    }
            }
            base = 10;
			goto number;
		case 'n':
            if (flags & LONGINT) {
				*va_arg(argp, long *) = cnt;
			} else if (flags & SHORTINT) {
				*va_arg(argp, short *) = cnt;
			} else {
				*va_arg(argp, int *) = cnt;
			}
			break;
		case 'O':
			flags |= LONGINT;
			/*
			 * FALLTHROUGH
			 */
		case 'o':
		    if (flags & LONGLONG) {
		        _ullong = va_arg(argp, unsigned long long);
		    } else {
			    ARG(unsigned);
			}
			base = 8;
			goto nosign;
		case 'p':
			/*
			 * ``The argument shall be a pointer to void.  The
			 * value of the pointer is converted to a sequence
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_ulong = (ulong)va_arg(argp, void *);
            width = 8;
			base = 16;
            flags |= ZEROPAD;
			digs = "0123456789ABCDEF";
			goto nosign;
		case 's':
			if (!(t = va_arg(argp, char *))) {
				t = "(null)";
			}
			if (prec >= 0) {
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen() will go further.
				 */
				char *p;

				if ((p = memchr(t, 0, prec)) != NULL) {
					size = p - t;
					if (size > prec) {
						size = prec;
					}
				} else {
					size = prec;
				}
			} else {
				size = strlen(t);
			}
			sign = '\0';
			goto pforw;
		case 'U':
			flags |= LONGINT;
			/*
			 * FALLTHROUGH
			 */
		case 'u':
            if (flags & LONGLONG) {
                _ullong = va_arg(argp, unsigned long long);
            } else {
			    ARG(unsigned);
            }
			base = 10;
			goto nosign;
		case 'X':
			digs = "0123456789ABCDEF";
			/*
			 * FALLTHROUGH
			 */	
		case 'x':
            if (flags & LONGLONG) {
                _ullong = va_arg(argp, unsigned long long);
            } else {
			    ARG(unsigned);
			}
			base = 16;
			/*
			 * leading 0x/X only if non-zero
			 */
			if (flags & ALT && _ulong != 0) {
				flags |= HEXPREFIX;
			}
			

			/*
			 * unsigned conversions
			 */
nosign:		sign = '\0';
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:		if ((dprec = prec) >= 0) {
				flags &= ~ZEROPAD;
			}

			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			t = buf + BUFSIZE; /* let t point to end of buf */

            if (flags & FLOAT) {
#ifdef SUPPORT_LF           /* fractional part */
                if (fdec(_double) > 18) {
                    *--t = '#'; *--t='#'; *--t='#';
                    *--t = '.';
                    *--t = '#'; *--t ='#'; *--t='#';
                    prec = 0;
                    fpprec = 0;
                } else {
                    digs = "0123456789abcdef";
                    for (n = 0; n < prec; n++) {
                        *--t = digs[_fract % 10];
                        _fract /= 10;
                    } 
                    *--t = '.';
                    do {
                        *--t = digs[_ullong % base];
                        _ullong /=10;
                    } while(_ullong > 0);
                }
#endif          
          } else if (flags & LONGLONG) {
                if (_ullong != 0 || prec != 0) {                    
                    do {
                        *--t = digs[_ullong % base];
                        _ullong /= base;
                    } while(_ullong);
                    digs = "0123456789abcedf";
                    if (flags & ALT && base == 8 && *t != '0') {
                        *--t = '0'; /* octal leading 0 */
                    }
                }
           } else {   
    			if (_ulong != 0 || prec != 0) {
	    			do {
		    			*--t = digs[_ulong % base];
			    		_ulong /= base;
				    } while (_ulong);
				    digs = "0123456789abcdef";
				    if (flags & ALT && base == 8 && *t != '0') {
					    *--t = '0'; /* octal leading 0 */
				    }
			    }
            }
			size = buf + BUFSIZE - t;


pforw:
			/*
			 * All reasonable formats wind up here.  At this point,
			 * `t' points to a string which (if not flags&LADJUST)
			 * should be padded out to `width' places.  If
			 * flags&ZEROPAD, it should first be prefixed by any
			 * sign or other prefix; otherwise, it should be blank
			 * padded before the prefix is emitted.  After any
			 * left-hand padding and prefixing, emit zeroes
			 * required by a decimal [diouxX] precision, then print
			 * the string proper, then emit zeroes required by any
			 * leftover floating precision; finally, if LADJUST,
			 * pad with blanks.
			 */

			/*
			 * compute actual size, so we know how much to pad
			 * fieldsz excludes decimal prec; realsz includes it
			 */
			fieldsz = size +  fpprec  ;
			realsz = (dprec > fieldsz) ? dprec : fieldsz;
			if (sign) {
				realsz++;
			}

			if (flags & HEXPREFIX) {
				realsz += 2;
			}

            /* avoid overflow */
			if (cnt + (width > realsz ? width : realsz) > bsize) {
                *obuf = '\0';
                return cnt;
            }

			/*
			 * right-adjusting blank padding
			 */
			if ((flags & (LADJUST|ZEROPAD)) == 0 && width) {
				for (n = realsz; n < width; n++) {
					*obuf++ = ' ';
				}
			}

			/*
			 * prefix
			 */
			if (sign) {
				*obuf++ = sign;
			}
			if (flags & HEXPREFIX) {
				*obuf++ = '0';
				*obuf++ = (char)*fmt;
			}

			/*
			 * right-adjusting zero padding
			 */
			if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD) {
				for (n = realsz; n < width; n++) {
					*obuf++ = '0';
				}
			}
			

			/*
			 * leading zeroes from decimal precision
			 */
			for (n = fieldsz; n < dprec; n++) {
				*obuf++ = '0';
			}
			
			/*
			 * Check if we're doing fp - we may need to do some
			 * awkward padding for e/E and some g/G formats
			 */
			if (fpprec) {
				/*
				 * Do this the hard way :-(
				 */
				for (n = 0; n < size; n++) {
					/*
					 * Handle trailing zeros
					 */
					if ((t[n] == 'e')
					    || (t[n] == 'E')) {
						while (--fpprec >= 0) {
							*obuf++ = '0';
						}
					}
					*obuf++	= t[n];
				}

				/*
				 * trailing fp zeroes if not already
				 * handled for e/E case
				 */
				while (--fpprec >= 0) {
					*obuf++ = '0';
				}
			} else {
				/*
				 * copy the string or number proper
				 */
				memcpy(obuf, t, size);
				obuf += size;
			}


			
			/*
			 * left-adjusting padding (always blank)
			 */
			if (flags & LADJUST) {
				for (n = realsz; n < width; n++) {
					*obuf++ = ' ';
				}
			}
			
			/*
			 * finally, adjust cnt
			 */
			cnt += width > realsz ? width : realsz;
			break;
		case '\0':	/* "%?" prints ?, unless ? is NULL */
			*obuf++ = '\0';
			return (cnt);
		default:
			*obuf++ = (char)*fmt;
			cnt++;
		}
	}
	/*
	 * NOTREACHED
	 */
    return cnt;
}


#if 0
extern int myprintf(const char *fmt,...) __attribute__((format (printf, 1, 2)));
int myprintf(const char *fmt, ...) 
{
    va_list ap;
    char buf[2048];
    int size;

    va_start(ap, fmt);
    size = __doprnt(buf, 2048, fmt, ap);
    va_end(ap);
    printf("%s", buf);

    return size;
}

main()
{
    char *astr = "abcdefghijklmnop";
    int  aint = 1000;
    int  achar = 'Y';
    double adouble = 123456789.54321;
    double edouble = 1.23e17;
    double e3 = 9.99e19;
    double e2 = 1.23e63;
    double ldouble = 12345678901234567.123456789;
    float  afloat  =  123456.12345;
    unsigned long long ull = 12345678901234567ll;
    signed long long sll = -12345678901234567ll;
    int  size;
        
    size = myprintf("%%d=%d %d\n", 100, -100);
    size = myprintf("%%ld=%ld %ld\n", 200, -200);
    size = myprintf("%%u=%u %u\n", 300, -300);
    size = myprintf("%%hd=%hd %hd\n", 400, -400);
    size = myprintf("%02d-%02d-%04d %02d:%02d:%02d\n", 12, 25, 2006, 12, 10,24);
    size = myprintf("%s %d %c %ld %u\n", astr, aint, achar, aint, aint);
    printf("size = %d\n", size);
    size = myprintf("[%-10.5s] [%10.5s]\n", astr, astr);
    size = myprintf("double(123456789.54321)=%lf\n", adouble);
    size = myprintf("100=%d double(12345679.54321)=%lf\n", 100, adouble);
    size = myprintf("-double(-123456789.54321) =%lf\n", -adouble);
    size = myprintf("float(123456.12500) = %f\n", afloat);
    size = myprintf("ldouble(123456789012345678.000000)=%lf\n", ldouble);
    size = myprintf("edouble(1230000000000000000.000000)=[%lf]\n", edouble);
    size = myprintf("e2 (###.###)= [%lf]\n", e2);
    size = myprintf("e3 (###.###)=[%lf]\n", e3);
    size = myprintf("double %.2lf\n", adouble);
    size = myprintf("float %6.1lf\n", afloat);
    size = myprintf("(12345678901234567)=%lld\n", ull);
    size = myprintf("(-12345678901234567)=%lld\n", sll); 
    size = myprintf("(unsigned -12345678901234567)=%llu\n", sll);
}
#endif
