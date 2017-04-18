/**
 *  @file   story.c
 *  @brief  story framework for integritation test
 *  $Id: story.c,v 1.4 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <story.h>
#include <stdarg.h>
#include <shell.h>
#include <ctype.h>

story_board_t _sb;

void disable_printf(void);
void enable_printf(void);

//extern int _vprintf(const char *fmt, va_list args);
#if 0
int story_printf(const char *fmt, ...)
{
    va_list ap;
    int size;

    va_start(ap, fmt);
    // size = _vprintf(fmt,ap);
    va_end(ap);
    
    return size;
}
#endif

int story_board(void)
{
    printf("S_R0  = %d (0x%x)\n", _sb.regs[0], _sb.regs[0]);
    printf("S_R1  = %d (0x%x)\n", _sb.regs[1], _sb.regs[1]);
    printf("S_R2  = %d (0x%x)\n", _sb.regs[2], _sb.regs[2]);
    printf("S_R3  = %d (0x%x)\n", _sb.regs[3], _sb.regs[3]);
    printf("STR   = \"%s\"\n", _sb.str);
    printf("Counter  = %d\n", _sb.counter);
    printf("Success  = %d  Fail = %d\n", _sb.success, _sb.fail);

    return 0;
}

int story_reset(void)
{
    bzero(&_sb, sizeof(_sb));
    return 0;
}

int item_get(int xreg)
{
    if (xreg < 0 || xreg > 3) {
        story_printf("\n  >>item_get [invaid reg %d]", xreg);
        return -1;
    }

    return _sb.regs[xreg];
}

int item_get_counter(void)
{
    return _sb.counter;
}

char *item_get_str(void)
{
    return _sb.str;
}

int __item_check(int xreg, int val, const char *message, 
    const char *item, const char *fname, int lno) 
{
    if (xreg < 0 || xreg > 3) {
        story_printf("\n  >>item(%s) %s [invaid reg] at %s:%d", item, message, fname, lno);
        return -1;
    }

    if (_sb.regs[xreg] != val) {
        story_printf("\n  >>item(%s) %s [expect %d but %d] at %s:%d", item, message, val, _sb.regs[xreg], fname, lno);
        return -1;
    }

    return 0;
}

int __item_check_hex(int xreg, int val, const char *message, 
    const char *item, const char *fname, int lno)
{
    if (xreg < 0 || xreg > 3) {
        story_printf("\n  >>item(%s) %s [invaid reg] at %s:%d", item, message, fname, lno);
        return -1;
    }

    if (_sb.regs[xreg] != val) {
        story_printf("\n  >>item(%s) %s [expect %x but %x] at %s:%d", item, message, val, _sb.regs[xreg], fname, lno);
        return -1;
    }

    return 0;
}

int __item_check_str(const char *xstr, const char *message, 
    const char *item, const char *fname, int lno)
{   
    if (xstr == NULL) 
        xstr = "(null)";

    if (strncmp(xstr, _sb.str, 32) != 0) {
        story_printf("\n  >>item(%s) %s [expect \"%s\" but \"%s\"] at %s:%d", 
                item, message, xstr, _sb.str, fname, lno);
        return -1;
    }

    return 0;
}

int __item_expect(int success, int fail, const char *message, 
        const char *item, const char *fname, int lno)
{
    if (_sb.success != success) {
        story_printf("\n  >> item(%s) %s [expect success %d but %d] at %s:%d", item, message, success, _sb.success, fname, lno);
        return -1;
    }

    if (_sb.fail != fail) {
        story_printf("\n  >> item(%s) %s [expect fail %d but %d] at %s:%d", item, message, fail, _sb.fail, fname, lno);
        return -1;
    }

    return 0;
}

int __item_check_counter(int counter, const char *message, 
        const char *item, const char *fname, int lno) 
{
    if (_sb.counter != counter) {
        story_printf("\n  >>item(%s) %s [expect counter %d but %d] at %s:%d", 
            item, message, counter, _sb.counter, fname, lno);
        return -1;
    }

    return 0;
}


int __item(const char *shell, const char *args, const char *fname, int lno)
{
    cmd_t *cmd;
    void *result;
    int rc;
    const char *ptr;
    char *argp, *argq, *arge;
    char ch;
    int argc = 1, quote = 0;
    char argbuf[64];
    const char *argv[6];
    void *sys_ctx;
    extern void get_sys_ctx(void **sys_ctx);

    get_sys_ctx(&sys_ctx);


    for (cmd = &__cmd_start; cmd < &__cmd_end; cmd++) {
        if (strcmp(shell, cmd->name) == 0)
            break;
    }

    if (cmd == &__cmd_end) {
        story_printf(">> item(%s %s) command %s not found at %s:%d\n", 
                shell, args, shell, fname, lno);
        return -1;
    }

    argv[0] = shell;
    argc = 1;
    if (args == NULL) {
        argv[1] = "";
    } else {
        strncpy(argbuf, args, 64);
        argq = argp = argbuf;
        arge = argp + sizeof(argbuf) - 1; 
        ptr = args;
        for (; ptr && *ptr != '\0' ; ptr++) {
            ch = *ptr;
            if (argp == argq && isspace(ch))
                continue;
            if ((quote & 1) == 0 && isspace(ch)) {
                *argp++ = '\0';
                argv[argc++] = argq;
                if (argc >= 6)
                    break;
                argq = argp;
            } else if (ch == '\'') {
                quote++;
            } else {
                *argp++ = ch;
            }
            if (argp > arge) {
                break;
            }
        }
        if (argp != argq) {
            *argp++ = '\0';
            if (argc < 6) {
                argv[argc++] = argq;
            }
        }
    }

    rc = (cmd->func)(argc, (char **)argv, (void **)&result, sys_ctx);
    if (rc < 0) {
        story_printf(">> failed to execute %s at %s:%d\n", shell, fname, lno);  
        return -1;
    }

    return 0;
}

int list_story(void)
{
    story_t *st;
    const char *group = "";
    
    printf("%p - %p\n", &__story_start, &__story_end);

    for (st = &__story_start; st < &__story_end; st++) {
        if (strcmp(group, st->group) == 0) {
            printf("%s ", st->name);
        } else {
            printf("\n%s : %s ", st->group, st->name);
            group = st->group;
        }
    }

    printf("\n");

    return 0;
}

int run_all_story(int opt)
{
    story_t *st;
    int rc = 0;
    int err = 0;

    disable_printf();
    for (st = &__story_start; st < &__story_end; st++) {
        story_printf("\nSTORY(%s,%s) ", st->group, st->name);

        if (st->story == NULL) {
            story_printf(" Ignore");
            continue;
        }
            
        rc = st->story(opt);

        if (rc != 0) {
            story_printf("\n  >>STORY(%s,%s) %d Fails", st->group, st->name, rc);   
            err++;
        } else {
            story_printf(" Pass");
        }

        if (rc != 0 && (opt & STOP_STORY) == STOP_STORY)    
            break;

        rc = 0;
    }

    if (rc != 0)
        story_printf("\n\n--- fails and stop story\n");
    else {
       if (err == 0) {
            story_printf("\n\n--- end story and all success\n");
       } else {
            story_printf("\n\n--- end story but %d fails\n", err);
       }
    }

    enable_printf();

    return 0;
}

int run_story(const char *astory, const char *chapter, int opt)
{
    
    story_t *st;
    int rc = 0;
    int err = 0;

    disable_printf();
    for (st = &__story_start; st < &__story_end; st++) {
        if (strcmp(astory, st->group) != 0) 
            continue;
        if (chapter && strcmp(chapter, st->name) != 0)
            continue;
        if (st->story == NULL) {
            story_printf("\nIGNORE_STORY(%s,%s)", st->group, st->name);
            continue;
        }

        story_printf("\nSTORY (%s,%s) ", st->group, st->name);

        if (st->story == NULL) {
            story_printf(" Ignore");
            continue;
        }

        rc = st->story(opt);
                
        if (rc != 0) {
            story_printf("\n  >>STORY(%s,%s) %d Fails", st->group, st->name, rc);
            err++;
        } else { 
            story_printf(" Pass");
        }

        if (rc != 0 && (opt & STOP_STORY) == STOP_STORY)
            break;

        rc = 0;
    }

    if (rc != 0) {
        story_printf("\n\n--- fails and stop story\n");
    } else {
       if (err == 0) {
            story_printf("\n\n--- end story and all success\n");
       } else {
            story_printf("\n\n--- end story but %d fails\n", err);
       }
    }

    enable_printf();

    return 0;
}
