/**
 *  @file   sh_mfc0.c
 *  @brief  read value from MIPS coprocessor 0 register
 *
 *  $Id: sh_mfc0.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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

command_init (sh_mfc0, "mfc0", "mfc0 <reg_name>.[reg_sel]");

static int check_predefined_reg(char *name, reg_info_t *reg_info, int *reg_sel)
{
    int i, rc, len;
    
    rc = -1;
    for ( i=0; i<REG_NUM; i++ ) {
        len = strlen(reg_info[i].name);
        if ( (strncmp(name, reg_info[i].name, len) == 0) && 
             (name[len] == '\0' || name[len] == '.')        ) {
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

/* due to the nature of mfc0 assembly command, no simple way to implement 
   without unrolling all the cases */

__mips32__
static unsigned int mfc0_12(int reg_sel)
{
    unsigned int val;

    switch (reg_sel) {
    case 0:
        val = __read_32bit_c0_register($12, 0);
        break;
    case 1:
        val = __read_32bit_c0_register($12, 1);
        break;
    case 2:
        val = __read_32bit_c0_register($12, 2);
        break;
    case 3:
        val = __read_32bit_c0_register($12, 3);
        break;
    default:
        printf("cp0 status no select %d\n", reg_sel);
        return -1;
    }
    
    return val;
}

__mips32__
static unsigned int mfc0_13(void)
{
    return __read_32bit_c0_register($13, 0);
}

__mips32__
static unsigned int mfc0_15(int reg_sel)
{
    unsigned int val;

    switch (reg_sel) {
    case 0:
        val = __read_32bit_c0_register($15, 0);
        break;
    case 1:
        val = __read_32bit_c0_register($15, 1);
        break;
    default:
        printf("cp0 prid/ebase no select %d\n", reg_sel);
        return -1;
    }
    
    return val;
}

__mips32__
static unsigned int mfc0_16(int reg_sel)
{
    unsigned int val;

    switch (reg_sel) {
    case 0:
        val = __read_32bit_c0_register($16, 0);
        break;
    case 1:
        val = __read_32bit_c0_register($16, 1);
        break;
    case 2:
        val = __read_32bit_c0_register($16, 2);
        break;
    case 3:
        val = __read_32bit_c0_register($16, 3);
        break;
    default:
        printf("cp0 config no select %d\n", reg_sel);
        return -1;
    }
    
    return val;
}

__mips32__
static unsigned int mfc0_31(void)
{
    return __read_32bit_c0_register($31, 0);
}

static int sh_mfc0 (int argc, char **argv, void **result, void *sys_ctx)
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

	if (argc < 2) 
        goto EXIT;
        
    reg_idx = -1;
    reg_sel = 0;
    rc = check_predefined_reg(argv[1], reg_info, &reg_sel);
    if ( rc < 0 ) {
        goto EXIT;
    }
    reg_idx = rc;

    printf("mfc0 register $%d select %d\n", reg_idx, reg_sel);
    switch ( reg_idx ) {
    case 12: /* status */
        val = mfc0_12(reg_sel);
        break;
    case 13: /* cause */
        val = mfc0_13();
        break;
    case 15: /* prid */
        val = mfc0_15(reg_sel);
        break;
    case 16: /* config */
        val = mfc0_16(reg_sel);
        break;
    case 31: /* desave */
        val = mfc0_31();
        break;
    default:
        printf("don't know how to mfc0 %d\n", reg_idx);
        goto EXIT;
    }
    printf("value = 0x%08x(", val);
    
    for ( i=31; i>=0; i-- ) {
        int m = (1 << i);
        if ( val & m )
            printf("1");
        else
            printf("0");
        if ( (i%4 == 0) && (i != 0) )
            printf("-");
    }
    printf(")\n");

    return 0;

EXIT:
    print_usage (sh_mfc0);
    printf("Support registers:\n");
    for ( i=0; i<REG_NUM; i++ ) {
        printf("%s\t($%d)\n", reg_info[i].name, reg_info[i].reg_idx);
    }

    return -1;
}

