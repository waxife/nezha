/**
 *  @file   sys_cardsel.c
 *  @brief  system wide functions and resource management
 *  $Id: sys_libs.c,v 1.10 2015/08/05 10:32:22 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.10 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/04/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include <gpio.h>
#include <mipsregs.h>
#include <interrupt.h>
#include <string.h>

int card_select(int card)
{
    static int card_sel = -1;

    assert((card & ~0x07) == 0);

    if (card == card_sel)
        return 0;

    writel(card, GPIO_CARD_SEL);
    card_sel = card;

    return 0;
}


#define SYSMGM_BASE         0xb7c00000
#define SYSMGM_CLK_SEL      (SYSMGM_BASE + 0x100)
#define SYSMGM_CLK_DIV      (SYSMGM_BASE + 0x104)
#define SYSMGM_OP           (SYSMGM_BASE + 0x108)
#define SYSMGM_SWINFO       (SYSMGM_BASE + 0x10c)
#define SYSMGM_AHB_PLL      (SYSMGM_BASE + 0x110)
#define SYSMGM_AUD_PLL      (SYSMGM_BASE + 0x114)

#define SYSMGM_RTC_INTR		(SYSMGM_BASE + 0x02c)
#define SYSMGM_RTC_FLAG		(SYSMGM_BASE + 0x030)
#define SYSMGM_RTC_FREQ		(SYSMGM_BASE + 0x034)
#define SYSMGM_IR_CTRL		(SYSMGM_BASE + 0x040)
#define SYSMGM_IR_CODE		(SYSMGM_BASE + 0x044)

#define UART_BASE	        0xb2000000
#define UART_DLL	        (UART_BASE + 0x00)
#define UART_DLH            (UART_BASE + 0x04)
#define UART_LCR			(UART_BASE + 0x0C)

#define IR_CTRL_FLAG        (1 << 25)
#define IR_CTRL_INT_EN      (1 << 21)


#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	mips0					\n"	\
	"	.set	reorder"					\
	:								\
	: "i" (op), "m" (*(unsigned char *)(addr)))

#define Index_Load_Tag_I	    0x04
#define Index_Load_Tag_D	    0x05
#define Hit_Fetch_Lock_I        0x1c
#define Hit_Fetch_Lock_D        0x1d

/* system id is not always availabe in FPGA environment */
unsigned int sys_chip_ver(void)
{
    return (readl(0xb0400f30) << 24) | (readl(0xb0400f40) << 16) | (readl(0xb0400f50) << 8) | (readl(0xb0400f60)) ;
}
void __mips32__
cli32(void)
{
    __cli();
}
void __mips32__
sti32(void)
{
    __sti();
}


void __mips32__
sys_reset(void)
{
    __cli();

    printf("reset\n");

    writel(0x04, SYSMGM_OP);
    __asm__ __volatile__ (
        "1: b 1b;" 
    );
}
void __mips32__
sys_shutdown(void)
{
    __cli();

/* 2009-11-02 gary-lin modified. Most T373s are fixed for this bug */
    writel(0x01, SYSMGM_OP);
    
    __asm__ __volatile__ (
        "1: b 1b;" 
    );

}

char *sys_chip_ver_name(void)
{
    switch(sys_chip_ver()) {
    case CHIP_FPGA:
        return "FPGA";
    case CHIP_T571:
        return "T571";
    case CHIP_T373A:
        return "T373";
    default:
        return "Unkown";
    }
}

/* SDRAM MRS register [23:20]: clk delay */
unsigned long sys_chip_sdram_mrs_clk_delay(void)
{
    return 0x0;
}


__mips32__
unsigned long read_c0_count32(void)
{
    return read_c0_count();
}

__mips32__
unsigned long read_c0_datalo32(void)
{
    return read_c0_datalo();
}

__mips32__
unsigned long read_c0_taglo32(void)
{
    return read_c0_taglo();
}

__mips32__
unsigned long read_c0_taghi32(void)
{
        return read_c0_taghi();
}


__mips32__ void
change_c0_config32(int mask, int val)
{
    change_c0_config(mask, val);
}

__mips32__
unsigned long read_c0_config32(int sel)
{
    switch ( sel ) {
    case 0:
        return read_c0_config();
        break;
    case 1:
        return read_c0_config1();
        break;
    default:
        return -1;
    }
}

unsigned int 
__attribute__((__no_instrument_function__))
get_sp()
{ 
    uint32_t __sp;
    __asm__  __volatile__ ("move %0, $29" : "=r"(__sp));
    return __sp;
}

static unsigned int enter_sp_depth = -1;
static void* enter_func;
static void* enter_call;
static unsigned int exit_sp_depth = -1;
static void* exit_func;
static void* exit_call;

#if 0
void __attribute__((__no_instrument_function__))
sp_statistics_enter(void* func, void* call)
{
	unsigned int sp = 0;

	sp = get_sp();
	if(sp < enter_sp_depth) {
		enter_sp_depth = sp;
		enter_func = func;
		enter_call = call;
	}
}

void __attribute__((__no_instrument_function__))
sp_statistics_exit(void* func, void* call)
{
	unsigned int sp = 0;

	sp = get_sp();
	if(sp < exit_sp_depth) {
		exit_sp_depth = sp;
		exit_func = func;
		exit_call = call;
	}
}
#endif

void __attribute__((__no_instrument_function__))
sp_printf(void)
{
	printf("==== Stack Pointer of Statistics ====\n");
	printf("enter_sp_depth = %08X\n", enter_sp_depth);
	printf("enter_func = %p\n", enter_func);
	printf("enter_call = %p\n", enter_call);
	printf("exit_sp_depth = %08X\n", exit_sp_depth);
	printf("exit_func = %p\n", exit_func);
	printf("exit_call = %p\n", exit_call);
}

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_func, void *call_site)
{
//	sp_statistics_enter(this_func, call_site);
	unsigned int __sp;
    __asm__  __volatile__ ("move %0, $29" : "=r"(__sp));
	if(__sp < enter_sp_depth) {
		enter_sp_depth = __sp;
		enter_func = this_func;
		enter_call = call_site;
	}
}
void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_func, void *call_site)
{
//	sp_statistics_exit(this_func, call_site);
#if 0
	unsigned int sp;

	sp = get_sp();    
	if(sp < exit_sp_depth) {
		exit_sp_depth = sp;
		exit_func = this_func;
		exit_call = call_site;
	}
#endif    
}

#define LAYOUT_INFO_ADDR    (0x80000000 + BIN_LAYOUT_INFO_OFFSET)
int sys_firmware_ver(unsigned int *ver, unsigned int *build_date, unsigned int *build_num)
{
    int rc = 0;

    if (!ver || !build_date || !build_num) {
        printf("Null arguments");
        rc = -EINVAL;
        goto EXIT;
    }

    if (readl(LAYOUT_INFO_ADDR) !=  0x1A1A1010) {
        printf("error tag 0x%08X. (correct tag is 0x1A1A1010)\n", readl(BIN_LAYOUT_INFO_OFFSET));
        rc = -ENXIO;
        goto EXIT;
    }

    *ver = readl(LAYOUT_INFO_ADDR + 0x04);
    *build_date = readl(LAYOUT_INFO_ADDR + 0x08);
    *build_num = readl(LAYOUT_INFO_ADDR + 0x0C);

EXIT:
    return rc;
}
