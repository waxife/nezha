/**
 *  @file   shell.h
 *  @brief  shell environment for tboot
 *  $Id: shell.h,v 1.2 2014/01/27 12:26:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdint.h>

typedef struct cmd {
	int	    (*func) (int argc, char **argv, void **result, void *sys_ctx);
	char	*name;
	char	*synopsis;
} cmd_t;

extern cmd_t __cmd_start, __cmd_end;


#define command_init(xfn, xname, xargs) \
    static int xfn(int argc, char **argv, void **result, void *sys_ctx) __attribute__((used)); \
    cmd_t __cmd_##xfn \
        __attribute__ ((section (".cmd"))) \
        = {xfn, xname, xargs}

#define command_alias(xfn, id, xname, xargs) \
    cmd_t __cmd_##xfn##_##id \
        __attribute__ ((section (".cmd"))) \
        = {xfn, xname, xargs}


#define print_usage(fn) \
    printf ("[%s] usage: %s\n", __cmd_##fn.name, (*__cmd_##fn.synopsis == '-') ? __cmd_##fn.synopsis + 1 : __cmd_##fn.synopsis)

extern void shell (void *sys_ctx);

/* shell environment variables */
extern char *getenv(const char *var);

/* system memory tag */
extern int minfo_tag(const char *tag, void *addr, int len, int temp);
extern int minfo_check(const char *tag, char *addr, int len);

/* argument parsing functions */
extern int get_object(const char *arg, int *obj, void *addr, int *len, char **name);
extern int get_addr(const char *arg, void *addr);
extern int get_addr_len(const char *arg, void *addr, int *len);
extern int get_val(const char *arg, int *val);
extern int get_val_width(const char *arg, int *val, int *width);
extern int check_addr_aligned(void *addr, int len, int align);

extern int shell_script;

#define OBJ_ADDR    0
#define OBJ_FILE    1
#define OBJ_IMAGE   2

/* terminal type */
#define T_ANSI      0
#define T_VT100     1
#define T_VT102     2


#endif /* __SHELL_H__ */
