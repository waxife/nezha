/**
 *  @file   stdlib.h
 *  @brief  POSIX like standard library header
 *  $Id: stdlib.h,v 1.4 2014/07/17 08:03:37 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#ifndef __STDLIB_H__
#define __STDLIB_H__

#ifdef __mips

#define MIN(A, B)   ((A) < (B) ? (A) : (B))
#define MAX(A, B)   ((A) > (B) ? (A) : (B))

#define ABS(V)      ((V) > 0 ? (V) : -(V))

#define CDIV(X, Y)      (((X) + ((Y) - 1)) / (Y))   // compute ceil(X/Y)
#define FDIV(X, Y)      ((X) / (Y))                 // compute floor(X/Y)

#define ROUNDUP(X, Y)   (((X) + ((Y) - 1)) & ~((Y) - 1))
#define ROUNDDOWN(X, Y) ((X) & ~((Y) - 1))


typedef struct 
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

div_t div(int num, int denom);
int atoi (const char *ptr);
unsigned int atoh (const char *ptr);
unsigned int stoi (const char *ptr);
unsigned int size (const char *ptr);
void itoa(int n, char s[]);


void reverse(char s[]);

extern long int strtol(const char *nptr, char **endptr, int base);
extern int rand(void);

static inline void abort(void) {
   extern int puts(const char *);
   puts("System abort...\n"); for(;;);
}

#define malloc(xsize)	__malloc(xsize, __FILE__, __LINE__)
void *__malloc(unsigned long size, const char *fname, int lineno);
#define free(ptr)		__free(ptr, __FILE__, __LINE__)
void __free (void *p, const char *fname, int lineno);

typedef int (*__compare_fn_t) (const void *, const void *);
void qsort (void *base, int nmemb, int size, __compare_fn_t comp);


#else /* not __mips */
#include "/usr/include/stdlib.h"
#endif 

#define popcount(x)     __builtin_popcount(x)

#endif // __STDLIB_H__
