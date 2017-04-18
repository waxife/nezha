/**
 *  @file   sys.h
 *  @brief  this header file put system interface initial function prototypeing
 *  $Id: sys.h,v 1.22 2015/08/05 10:33:30 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.22 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <io.h>
#include <sys/types.h>
#include <sys_stack.h>

#define CHIP_FPGA       0x3000
#define CHIP_T307       0x3070
#define CHIP_T302A      0x3021
#define CHIP_T302B      0x3022
#define CHIP_T376A      0x3761
#define CHIP_T373A      0x3731
#define CHIP_T571       0x5711
#define CHIP_T582       0x582
#define CHIP_T582B      0xBAD9
#define CHIP_T582DB     0xBBD9


#define DFU_ENTRY_T373	0xbfc00c48
#define DFU_ENTRY_T376	0xbfc00c1c
//#define DFU_ENTRY_T302	0xbfc00c0c
#define DFU_ENTRY_T302	0xbfc00b1c

/*****************************************************
 * sysclock paramenter. define on disp/sysclock.c
 *****************************************************/

extern const unsigned long    VAL_SYSMGM_ALTFUNC_SEL;
extern const unsigned long    VAL_SYSMGM_CLK_SEL;
extern const unsigned long    VAL_SYSMGM_CLK_DIV;
extern const unsigned long    VAL_SYSMGM_AHB_PLL;
extern const unsigned long    VAL_SYS_CLK_SRC;


/*****************************************************
 * system variable caclute by sysclk_info()
 *****************************************************/

extern unsigned long    sys_cpu_clk;
extern unsigned long    sys_plld_clk;   


#define sys_ahb_clk     (sys_cpu_clk)
#define sys_apb_clk     (VAL_SYS_CLK_SRC)
#define sys_clk_src     (VAL_SYS_CLK_SRC)


extern unsigned int sys_chip_ver(void);
extern char *sys_chip_ver_name(void);
extern unsigned long sys_chip_sdram_mrs_clk_delay(void);

struct fat_t;
struct sdc;
struct norc_t;


enum SYS_STATUS_ {
    ST_NONE = 0,
    ST_PLAYFILE,
    ST_RECFILE,
    ST_DECPIC,
    ST_ENCPIC,
    ST_DECAU,
    ST_ENCAU,
    ST_SLIDE,
    ST_MDVID,
    ST_LSF
};

enum SYS_STATE_ {
    NONE = 0,
    PLAY,
    REC,
    PAUSE,
    STOP_SOF,
    STOP_EOF,
    STOP_USER,
    STOP_MD,
    STOP_ERR
};

struct sysc_status_t {
    int status; /* enum */
    int fno;
    int time;
    int offset;
    int quality;
    int size;
    int state;
};

#define ERR_MSG_LEN 32
struct sysc_t {
    int err_no;
    char err_msg[ERR_MSG_LEN];
    const char *fname;      /* error report file */
    int lno;                /* error report number */
    struct fat_t *pfat;
    struct norc_t *pnor;
    void *panel;
    void *src;
    void *au;
    struct sysc_status_t status;
};
extern struct sysc_t gSYSC;

void set_err(int no, const char *fmt, ...);
int get_err(char *errmsg);

#ifdef __mips

#if 0
static __inline__ void
__sti(void) 
{
    __asm__ __volatile__ (
        ".set push;"
        ".set reorder;"
        ".set noat;"
        ".set nomips16;"
        "mfc0 $1, $12;"
        "ori  $1, 0x1f;"
        "xori $1, 0x1e;"
        "mtc0 $1, $12;"
        ".set pop;"
    );

}

static __inline__ void
__cli(void) 
{
    __asm__ __volatile__ (
        ".set push;"
        ".set noat;"
        ".set nomips16;"
        "mfc0 $1, $12;"
        "ori  $1, 1;"
        "xori $1, 1;"
        ".set noreorder;"
        "mtc0 $1, $12;"
        "nop;nop;nop;"
        ".set pop;"
    );
}
#endif

#define __cli()     __asm__ __volatile__ (".set mips32r2; di")
#define __sti()     __asm__ __volatile__ (".set mips32r2; ei")

extern void __mips32__ cli32(void);
extern void __mips32__ sti32(void);
extern void __mips32__ cpu_wait(void);

#define cli     __cli
#define sti     __sti
#endif /* __mips */

extern int      serial_init(unsigned long baud);              /* drivers/serial/serialxxx.c */
extern int      afc_serial_init(unsigned long uart_baud);
extern void     fs_init(void);                  /* fat/fs.c */
extern void     trap_init(void);                /* sys/trap.c */
extern void     cache_init(void);               /* sys/c-r4k.c */
extern void     ah1_bus_init(void);             /* sys/sys.c */
extern int      sysclk(void);              /* sys/sys.c */
extern int      sysclk_info(void);
extern int      card_select(int card);          /* sys/sys.c */
extern int      enable_carddect_irq(int card);  /* sys/sys.c */

#define         card_unselect(xcard)     /* do nothing */


extern void     shell (void *sys_ctx);                   /* shell/shell.c */

extern __mips32__ void sys_reset(void);
extern __mips32__ void sys_shutdown(void);
extern __mips32__ unsigned long read_c0_count32(void);
extern __mips32__ void change_c0_config32(int mask, int val);
extern __mips32__ unsigned long read_c0_config32(int sel);
extern __mips32__ unsigned long read_c0_datalo32(void);
extern __mips32__ unsigned long read_c0_datahi32(void);
extern __mips32__ unsigned long read_c0_taglo32(void);
extern __mips32__ unsigned long read_c0_taghi32(void);


#define SYSMGM_BASE         0xb7c00000
#define SYSMGM_BASE         0xb7c00000
#define SYSMGM_CLK_SEL      (SYSMGM_BASE + 0x100)
#define SYSMGM_CLK_DIV      (SYSMGM_BASE + 0x104)
#define SYSMGM_OP           (SYSMGM_BASE + 0x108)
#define SYSMGM_SWINFO       (SYSMGM_BASE + 0x10c)
#define SYSMGM_AHB_PLL      (SYSMGM_BASE + 0x110) /* DISP_PLL */
#define SYSMGM_AUD_PLL      (SYSMGM_BASE + 0x114)

#define RTC_BASE			0xb0800000
#define RTC_SWINFO			(RTC_BASE + 0x10c)

#define GPIO_BASE           0xb9000000
#define SYSMGM_ALTFUNC_SEL  (GPIO_BASE + 0x28)
#define SYSMGM_SWINFO_COPY  (GPIO_BASE + 0x30) 

#define SW_SHUTDOWN         (1 << 0)
#define SW_SUSPEND_0        (1 << 1)
#define SW_SUSPEND_1        (1 << 2)
#define ST_RESERVED			(1 << 8)	/* SZ usage */
#define ST_ALARM			(1 << 15)

static int inline sys_memsize(void)
{
#ifdef CONFIG_T300
    return (readl(SYSMGM_SWINFO) & 0xffff0000);
#else
    return 16*1024*1024;
#endif
}


#define SYS_ADDR_LOW            0x80000000
#define SYS_ADDR_HI             0xffffffff

#define is_valid_addr(xaddr)  (((unsigned)(xaddr)) >= SYS_ADDR_LOW && ((unsigned)(xaddr)) <= SYS_ADDR_HI)

struct speed_t {
    uint32_t    clk_src;
    uint32_t    cpu_clk;
    uint32_t    disp_clk;
    uint32_t    apb_clk;
    uint32_t    disp_pll;
    uint32_t    sys_clk_sel;
    uint32_t    sys_clk_div;
    uint32_t    sys_clk_pll;
};

#define sec(val)            (sys_cpu_clk / 2 * val)
#define millisec(val)       (sys_cpu_clk / 2 / 1000 * val)
#define microsec(val)       (sys_cpu_clk / 2 / 1000000 * val)
#define is_time_out(val)    ((int)(read_c0_count32() - val) > 0)

#define BIN_LAYOUT_INFO_OFFSET       0x100
extern int sys_firmware_ver(unsigned int *ver, unsigned int *build_date, unsigned int *build_num);

#endif /* __SYS_H */
