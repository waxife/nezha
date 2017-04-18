/**
 *  @file   shell.c
 *  @brief  shell environment for tboot
 *  $Id: shell.c,v 1.4 2014/08/15 06:42:05 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <shell.h>
#include <ctype.h>
#include <vt100.h>
#include <debug.h>
#include <gpio.h>
#include <sys.h>



#define MAX_SCRIPT_SIZE         256
#define MAX_COMMAND_SIZE        256
#define MAX_ARGUMENTS           32

#if 0 // no script command
static char script_buffer[MAX_SCRIPT_SIZE];  

static char *get_script_command (char **pscript, int *lineno)
{
    char *ptr, *end, ch;
    int col = 0;
    int silent = 0, comment = 0;
    char *fscript = *pscript;

    ptr = script_buffer;
    *ptr = '\0';
    end = script_buffer + MAX_SCRIPT_SIZE - 1;
    /* get command from script */
    while (1) {
        ch = *fscript++;
        col++;

        /* skip start space */
        if (ptr == script_buffer && (ch == ' ' || ch == '\t'))
            continue;

        /* skip carry return */
        if (ch == '\r')
            continue;

        /* end of command */
        if (ch == '\n' || ch == 0x00) {
            (*lineno)++;
            col = 0;
            if (*(ptr - 1) == '\\') {   /* check '\' */
                *(ptr - 1) = ' ';           
                continue;
            }

            *ptr = '\0';
            if (!silent && script_buffer[0])
                printf ("%s\n", script_buffer);

            *pscript = fscript;
            goto EXIT;
        }

        /* comment - skip characters after # */
        if (ch == '#') {
            comment = 1;
            continue;
        }

        /* silent - silent if @ at the beginning of the command */
        if (ch == '@' && ptr == script_buffer) {
            silent = 1;
            continue;
        }

        /* terminate if get NUL, EOT, ^Z */
        if (ch == 0x00 || ch == 0x04 || ch == 0x1a) {
            *pscript = NULL;
            goto EXIT;
        }

        /* terminate if get unrecognized character */
        if (!isspace(ch) && (ch < 0x20 || ch > 0x7e)) {
            printf("unknow character at line %d col %d\n", *lineno, col);
            *pscript = NULL;
            script_buffer[0] = '\0';
            goto EXIT;
        }

        if (!comment) {
            *ptr++ = ch;
            if (ptr > end) {
                printf("Command is too long in script file\n");
                *pscript = NULL;
                script_buffer[0] = '\0';
                goto EXIT;
            }
        }
    }

EXIT:
    return script_buffer;
}
#endif

//static char cmd_buffer[MAX_COMMAND_SIZE]; 
#if 0 // no script command
int shell_script = 0;
#endif

static int get_command (char **pscript, char **argv, int *pargc, char *cmd_buffer)
{
    char *ptr, *argp, *argq, *arge, ch;
    int argc = 0, quote = 0;
//    static int lineno = 1;

#if 0 // no script command
    if (*pscript) {
        shell_script = 1;
        ptr = get_script_command(pscript, &lineno);
        if (*pscript == NULL) {
            lineno = 1;        /* reset line number */
            shell_script = 0;
        }
    } else {
        ptr = get_str();
    }
#else
        ptr = get_str();
#endif

    if (ptr == NULL)
        return -1;
    
    argq = argp = cmd_buffer;
    arge = argp + MAX_COMMAND_SIZE - 1;

    /* calculate argc */
    for (;*ptr != '\0'; ptr++) {
        ch = *ptr;

        if (argp == argq && isspace(ch))
            continue;

        if ((quote & 1) == 0 && isspace(ch)) {
            /* end of argument */
            *argp++ = '\0';
            if (argc >= MAX_ARGUMENTS) {
                printf("Too many arguments\n");
                goto EXIT;
            }
            argv[argc++] = argq;
            argq = argp;
        } else if (ch == '"') {
            quote++;
        } else {
            *argp++ = ch;
        }

        if (argp > arge) {
            printf("Command is too long\n");
            goto EXIT;
        }
    }

    if (argp != argq) {
        *argp++ = '\0';
        if (argc >= MAX_ARGUMENTS) {
            printf("Too many arguments\n");
            goto EXIT;
        }
        argv[argc++] = argq;
    }

    *pargc = argc;

    return 0;

EXIT:
    return -1;

}

static void shell_init(void)
{
    extern unsigned int _ftboot, _etext, _fdata, _etboot, _stack, __STACK_BARRIER;
    extern unsigned int _fbss, _ebss;
    unsigned int addr, addr2;
    unsigned int len;

    addr = (unsigned int)&_ftboot;
    addr2 = (unsigned int)&_etext;
    printf("program %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_fdata;
    addr2 = (unsigned int)&_etboot;
    printf("data    %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_fbss;
    addr2 = (unsigned int)&_ebss;
    printf("bss     %08x-%08x\n", addr, addr2);

    addr = (unsigned int)&_stack;
    len = addr - (unsigned int) &__STACK_BARRIER;
    printf("stack   %08x-%p (size=%d)\n", addr, &__STACK_BARRIER, len);
}

int run_command(int argc, char **argv)
{
    int rc;
    cmd_t *cmd;
    void *result;
    void *sys_ctx = NULL;

    if (argc <= 0)
        return -1;

    if (strcmp(argv[0], "script") == 0 || strcmp(argv[0], "s") == 0) {
        printf("No allow script command in a script file\n");
        return -1;
    }

    /* lookup command */
    for (cmd = &__cmd_start; cmd < &__cmd_end; cmd++) {
        if (strcmp (argv[0], cmd->name) == 0)
            break;
    }

    if (cmd == &__cmd_end) {
        printf("Unknown command %s\n", argv[0]);
        return -1;
    }

    rc = (cmd->func)(argc, argv, (void **)&result, sys_ctx);
    if (rc < 0) {
        printf("Failed to execute function %s\n", argv[0]);
    }

    return rc;            
    
}

void shell(void *sys_ctx)
{
    char *argv[MAX_ARGUMENTS];
    int argc = 0;
    void *result;
    char *script = NULL;
    int rc;
    cmd_t *cmd;
    char cmd_buffer[MAX_COMMAND_SIZE]; 

    shell_init();

    while(1) {
        rc = get_command(&script, argv, &argc, cmd_buffer);
        if (rc < 0 || argc == 0)
            continue;

        /* lookup command */
        for (cmd = &__cmd_start; cmd < &__cmd_end; cmd++) {
            if (strcmp (argv[0], cmd->name) == 0)
                break;
        }

        if (cmd == &__cmd_end) {
            printf("Unknown command %s\n", argv[0]);
            continue;
        }

        optind = 0;
        rc = (cmd->func)(argc, argv, (void **)&result, sys_ctx);
        if (rc < 0) {
            printf("Failed to execute function %s\n", argv[0]);
            
            if (script != NULL) {
                printf("Abort script command\n");
                script = NULL;
            }

            continue;
        }

        if (strcmp(argv[0], "script") == 0 || strcmp(argv[0], "s") == 0) {
            if (script != NULL) {
                printf("No allow script command in a script file\n");
            } else {
                script = (char *)result;
            }
        }
    
    }
}

int idle_job(void)
{
    return 0;
}


#if 0
char script_text[] =
"\n"
"                skip start space\n"
"TAB\tis\t\twhite\t\t\tspace\n"
"filte carrier return\r\n"
"new-line after back-slash\\\n"
"should be ignored\n"
"hide the comment # XXXXX\n"
"@this is a silent command\n"
"    @this is still a silent command\n"
"this is a \"double quote\" string\n"
"there is only \"one quote string\n"
"exec 0x80400000 \\\n"
"\"osc_freq=24000\\\n"
"mtdparts=nand@4M@0M(nda),-(ndb)\\\n"
"root=/dev/mtdblock/1\"\n"
"unrecognized character\aterminates script\n"
"never see this\n";

cmd_t __cmd_start, __cmd_end;

int
main (void)
{
    char *script = script_text;
    char *argv[MAX_ARGUMENTS];
    int argc;
    int rc;
    int i;
    char cmd_buffer[MAX_COMMAND_SIZE]; 

printf("isspace('\\t') == %d\n", isspace('\t'));    
printf("isspace('a') == %d\n", isspace('a'));

    while(script) {
        rc = get_command(&script, argv, &argc, cmd_buffer);
        if (rc < 0) {
            printf("failed to get command %d\n", rc);
            goto EXIT;
        }
        
        printf("argc : %d\n", argc);
        for (i = 0; i < argc; i++)
            printf("argv[%d]: %s\n", i, argv[i]);

        printf("\n"); 
    }
        
EXIT:
    return 0;
}
#endif
