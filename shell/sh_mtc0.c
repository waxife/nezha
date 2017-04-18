/**
 *  @file   sh_mtc0.c
 *  @brief  write value to MIPS coprocessor 0 register
 *
 *  $Id: sh_mtc0.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/18  gary    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <shell.h>
#include <sys.h>
#include <mipsregs.h>

typedef struct {
    char name[12];
    int  reg_idx;
} reg_info_t;
#define REG_NUM		5

command_init (sh_mtc0, "mtc0", "mtc0 <reg_name>.[reg_sel] <val>");

static int check_predefined_reg(char *name, reg_info_t *reg_info, int *reg_sel)
{
    int i, rc, len;
    
    rc = -1;
    for ( i=0; i<REG_NUM; i++ ) {
        len = strlen(reg_info[i].name);
        if ( (strncmp(name, reg_info[i].name, len) == 0) && 
             (name[len] == '.' || name[len] == '\0')        ) {
            rc = reg_info[i].reg_idx;
            break;
        }
    }
    if ( rc < 0 )
        return rc;
    
    if ( name[len] == '.' ) {
        *reg_sel = strtol(&name[len+1], NULL, 0);
    }
    
    return rc;
}

/* due to the nature of mtc0 assembly command, no simple way to implement 
   without unrolling all the cases */

__mips32__
static void mtc0_12(int reg_sel, unsigned int val)
{
    switch (reg_sel) {
    case 0:
         __write_32bit_c0_register($12, 0, val);
        break;
    case 1:
        __write_32bit_c0_register($12, 1, val);
        break;
    case 2:
         __write_32bit_c0_register($12, 2, val);
        break;
    case 3:
        __write_32bit_c0_register($12, 3, val);
        break;
    default:
        printf("cp0 status no select %d\n", reg_sel);
    }
}

__mips32__
static void mtc0_13(unsigned int val)
{
    __write_32bit_c0_register($13, 0, val);
}

__mips32__
static void mtc0_15(int reg_sel, unsigned int val)
{
    switch (reg_sel) {
    case 0:
        __write_32bit_c0_register($15, 0, val);
        break;
    case 1:
         __write_32bit_c0_register($15, 1, val);
        break;
    default:
        printf("cp0 prid/ebase no select %d\n", reg_sel);
    }
}

__mips32__
static void mtc0_16(int reg_sel, unsigned int val)
{
    switch (reg_sel) {
    case 0:
        __write_32bit_c0_register($16, 0, val);
        break;
    case 1:
        __write_32bit_c0_register($16, 1, val);
        break;
    case 2:
        __write_32bit_c0_register($16, 2, val);
        break;
    case 3:
        __write_32bit_c0_register($16, 3, val);
        break;
    default:
        printf("cp0 config no select %d\n", reg_sel);
    }
}

__mips32__
static void mtc0_31(unsigned int val)
{
    __write_32bit_c0_register($31, 0, val);
}


static int sh_mtc0 (int argc, char **argv, void **result, void *sys_ctx)
{
    reg_info_t reg_info[REG_NUM] = {
        { "status", 12 },
        { "cause", 13 },
        { "prid", 15 },
        { "config", 16 },
        { "desave", 31 },
    };
    int i, rc, reg_idx, reg_sel;
    unsigned int val;

	if (argc < 3) 
        goto EXIT;
        
    reg_idx = -1;
    reg_sel = 0;
    rc = check_predefined_reg(argv[1], reg_info, &reg_sel);
    if ( rc < 0 ) {
        goto EXIT;
    }
    reg_idx = rc;

    rc = get_val(argv[2], (int *)&val);
    if (rc < 0)
        goto EXIT;

    printf("mtc0 register $%d select %d value 0x%08x\n", reg_idx, reg_sel, val);
    switch ( reg_idx ) {
    case 12: /* status */
        mtc0_12(reg_sel, val);
        break;
    case 13: /* cause */
        mtc0_13(val);
        break;
    case 15: /* prid */
        mtc0_15(reg_sel, val);
        break;
    case 16: /* config */
        mtc0_16(reg_sel, val);
        break;
    case 31: /* desave */
        mtc0_31(val);
        break;
    default:
        printf("don't know how to mtc0 %d\n", reg_idx);
        goto EXIT;
    }

    return 0;

EXIT:
    print_usage (sh_mtc0);
    printf("Support registers:\n");
    for ( i=0; i<REG_NUM; i++ ) {
        printf("%s\t($%d)\n", reg_info[i].name, reg_info[i].reg_idx);
    }
    
    return -1;
}

