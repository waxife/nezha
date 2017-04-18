/**
 *  @file   unistd.h
 *  @brief  POSIX like standard symbolic constants and types
 *  $Id: unistd.h,v 1.6 2014/08/15 06:42:05 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#ifndef __UNISTD_H__
#define __UNISTD_H__

int sleep (unsigned int time);
int msleep (unsigned int mtime);
int usleep (unsigned int utime);

unsigned long long read_cpu_count(void);

extern unsigned long    sys_cpu_clk;

#define SEC_INTERVAL    (sys_cpu_clk >> 1)
#define MSEC_INTERVAL   (sys_cpu_clk / (2 * 1000))
#define USEC_INTERVAL   (sys_cpu_clk / (2 * 1000 * 1000))

#define hz2sec(xhz)    (unsigned long)((xhz)/SEC_INTERVAL)
#define hz2msec(xhz)   (unsigned long)(((xhz)%SEC_INTERVAL)/MSEC_INTERVAL)
#define hz2usec(xhz)   (unsigned long)(((xhz)%MSEC_INTERVAL)/USEC_INTERVAL)

#define sec2hz(xsec)   ((unsigned long long)(xsec) * SEC_INTERVAL)
#define msec2hz(xmsec) ((unsigned long long)(xmsec) * MSEC_INTERVAL)
#define usec2hz(xusec) ((unsigned long long)(xusec) * USEC_INTERVAL)


#if 0 
/* use #inclue <sys/types> to instead of define following types */ 
/* file function prototyping */
typedef unsigned long ssize_t;
typedef unsigned long size_t;
typedef unsigned long off_t;
#endif

#ifdef _TEST
#define open    u_open
#define close   u_close
#define chdir   u_chdir
#define read    u_read
#define write   u_write
#define lseek   u_lseek
#define getcwd  u_getcwd
#define mkdir   u_mkdir
#define rmdir   u_rmdir
#define unlink  u_unlink
#define rename  u_rename
#endif


#define O_ACCMODE          0003
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_CREAT            0100 /* not fcntl */
#define O_EXCL             0200 /* not fcntl */
#define O_NOCTTY           0400 /* not fcntl */
#define O_TRUNC           01000 /* not fcntl */
#define O_APPEND          02000
#define O_NONBLOCK        04000
#define O_NDELAY        O_NONBLOCK
#define O_SYNC           010000
#define O_FSYNC          O_SYNC
#define O_ASYNC          020000

extern int close(int);
extern int chdir(const char *);
extern int open(const char *, int, ...);
extern unsigned long read(int, void *, unsigned long);
extern unsigned long write(int, const void *, unsigned long);
extern unsigned long lseek(int, long, int);
extern char *getcwd(char *, int);
extern int mkdir(const char *, int);
extern int rmdir(const char *);
extern int fsize(int fd);
extern int unlink(const char *);
extern int rename(const char *, const char *);
extern int wtvblank(int timeout);

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

int getopt (int argc, char *argv[], char *optstring);
extern char *optarg;
extern int optind, optopt;

#endif /* __UNISTD_H__ */
