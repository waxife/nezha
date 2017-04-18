/**
 *  @file   string.h
 *  @brief  POSIX like standard string header
 *  $Id: string.h,v 1.5 2014/08/14 06:11:00 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#ifndef __STRING_H__
#define __STRING_H__

#include <strings.h>

int memcmp (const void *_s1, const void *_s2, int n);
int memchk (void *mem, char c, int n);
void *memcpy (void *_dst, const void *_src, int n);
void *memset (void *s, int c, int n);
void memdump (const char *mem, int len);
void *memchr (const void *s, int c, unsigned int n);

void hexdump (unsigned int start, unsigned int len, const void *data);

char *strcat (char *dst, const char *src);
char *strncat(char *dst, const char *src, int length);
int strcmp (const char *s1, const char *s2);
int strcasecmp(const char *s1, const char *s2);
char *strcpy (char *to, const char *from);
char *strncpy (char *to, const char *from, int length);
int strlen (const char *s);
int strncmp (const char *s1, const char *s2, int n);

char *strchr (const char *s, int c);
char *strrchr (const char *s, int c);
char *strstr (const char *s, const char *find);
char *strpbrk(const char * cs,const char * ct);
char *strsep(char **s, const char *ct);

char *str2lower(char *s);
char *str2upper(char *s);

#endif /* __STRING_H__ */
