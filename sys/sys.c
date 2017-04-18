/**
 *  @file   sys.c
 *  @brief  system wide functions and resource management
 *  $Id: sys.c,v 1.10 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.10 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include <gpio.h>
#include <mipsregs.h>
#include <interrupt.h>
#include <unistd.h>


#define AH1_DTBASE          (0xb9800040)
#define AH1_REGBASE         (0xb9800080)
#define AH1_DT_CF           (AH1_DTBASE + 0x00)
#define AH1_DT_SD           (AH1_DTBASE + 0x04)
#define AH1_DT_MS           (AH1_DTBASE + 0x08)
#define AH1_DT_XD           (AH1_DTBASE + 0x0c)
#define AH1_DT_NOR          (AH1_DTBASE + 0x10)
#define AH1_DT_NAND         (AH1_DTBASE + 0x14)
#define AH1_DT_I2S          (AH1_DTBASE + 0x18)

#define AH1_REG_NOR         (AH1_REGBASE + 0x10)

#define AH1_DT_SIZE         0x30000
#define AH1_NOR_DT_SIZE     0x40000

#define CF_BASE             0xba000000
#define SD_BASE             0xba800000
#define MS_BASE             0xbb000000
#define XD_BASE             0xbb800000
#define NAND_BASE           0xbc000000
#define I2S_BASE            0xbd000000

#define NOR_DIR             0xbe000000
#define NOR_BASE            0xbf000000

void ah1_bus_init(void)
{
    /* AH1 Bus Configuration */
    writel(virt_to_phys(NOR_DIR) | AH1_NOR_DT_SIZE, AH1_DT_NOR);
    writel(virt_to_phys(NOR_BASE) | AH1_DT_SIZE, AH1_REG_NOR);
    writel(virt_to_phys(CF_BASE) | AH1_DT_SIZE, AH1_DT_CF);
    writel(virt_to_phys(SD_BASE) | AH1_DT_SIZE, AH1_DT_SD);
    writel(virt_to_phys(MS_BASE) | AH1_DT_SIZE, AH1_DT_MS);
    writel(virt_to_phys(XD_BASE) | AH1_DT_SIZE, AH1_DT_XD);
    writel(virt_to_phys(NAND_BASE) | AH1_DT_SIZE, AH1_DT_NAND);
    writel(virt_to_phys(I2S_BASE) | AH1_DT_SIZE, AH1_DT_I2S);
}

int sysclk_info(void)
{
    int div2d = 0;
    int n = 0;
    int m = 0;
    int od = 0;
    unsigned long fvco = 0;
    unsigned long sysmgm_clk_div, sysmgm_ahb_pll, system_sd_clk;
    unsigned long system_va_clk, system_disp_clk;
    int i = 0;
 
    sysmgm_clk_div = readl(SYSMGM_CLK_DIV);
    sysmgm_ahb_pll = readl(SYSMGM_AHB_PLL);

    m = (sysmgm_ahb_pll >> 10) & 0x1F;
    n = (sysmgm_ahb_pll >> 3) & 0x7F;
    od = (sysmgm_ahb_pll >> 15) & 0x03;
    div2d = (sysmgm_ahb_pll >> 2) & 0x01;

    fvco = (sys_clk_src * (n + 2)) / (2 * (m + 2));
    sys_plld_clk = fvco;
    for (i = 0; i < (od + div2d); i++) {
        sys_plld_clk /= 2;
    }
    if(!(readl(SYSMGM_CLK_SEL)&0x01)){
        printf("***PLLD Bypass XCLK %lu!\n", sys_clk_src);
        sys_plld_clk = sys_clk_src;
    }
    
    system_va_clk = sys_plld_clk / (((sysmgm_clk_div>>24) & 0x0F) + 1) / (((sysmgm_clk_div>>28) & 0x0F) + 1);    
    if(!(readl(SYSMGM_CLK_SEL)&0x04)){
        printf("***VACLK Bypass XCLK %lu!\n", sys_clk_src);
        system_va_clk = sys_clk_src;
    }
    system_sd_clk = sys_plld_clk / (((sysmgm_clk_div>>16) & 0x0F) + 1);
    system_disp_clk = sys_plld_clk / (((sysmgm_clk_div>>20) & 0x0F) + 1);
   

    printf("Clock Source         : %lu.%02luMHz\n", sys_clk_src/1000000,  (sys_clk_src/10000)%100); 
    printf("PLLD Clock           : %lu.%02luMHz\n", sys_plld_clk/1000000,  (sys_plld_clk/10000)%100);
    printf("CPU Clock(AHB_CLK)   : %lu.%02luMHz\n", sys_cpu_clk/1000000,  (sys_cpu_clk/10000)%100);    
    printf("SD Src Clock(SD_PCLK): %lu.%02luMHz\n", system_sd_clk/1000000, (system_sd_clk/10000)%100);
    printf("DISPLAY Clock        : %lu.%02luMHz\n", system_disp_clk/1000000, (system_disp_clk/10000)%100);
    printf("VACLKO Clock         : %lu.%02luMHz\n", system_va_clk/1000000, (system_va_clk/10000)%100);
    
    printf("\n");
#if 0
    printf("fvco          : %lu.%02luMHz\n", fvco / 1000000, (fvco / 10000) % 100);
    printf("fvco = clk_src * (n+2)/2*(m+2)*2^(od+div2d)\n");
    printf("n = %d, m = %d, od = %d, div2d = %d\n", n, m, od, div2d);
 
    printf("\n");
#endif
    return 0;
}

int sysclk(void)
{
    int div2d = 0;
    int n = 0;
    int m = 0;
    int od = 0;
    unsigned long fvco = 0;
    unsigned int sysmgm_clk_div, sysmgm_ahb_pll;
    int i = 0;
 
    writel(0, 0xb7c00100);
    writel(8, 0xb7c00108);
    writel(VAL_SYSMGM_CLK_DIV, 0xb7c00104);
    writel(VAL_SYSMGM_AHB_PLL, 0xb7c00110);
    writel(VAL_SYSMGM_CLK_SEL, 0xb7c00100);
    writel(8, 0xb7c00108);
    usleep(100);

    sysmgm_clk_div = readl(SYSMGM_CLK_DIV);
    sysmgm_ahb_pll = readl(SYSMGM_AHB_PLL);

    m = (sysmgm_ahb_pll >> 10) & 0x1F;
    n = (sysmgm_ahb_pll >> 3) & 0x7F;
    od = (sysmgm_ahb_pll >> 15) & 0x03;
    div2d = (sysmgm_ahb_pll >> 2) & 0x01;

    fvco = (sys_clk_src * (n + 2)) / (2 * (m + 2));
    sys_plld_clk = fvco;
    for (i = 0; i < (od + div2d); i++) {
        sys_plld_clk /= 2;
    }

    sys_cpu_clk = sys_plld_clk / ((sysmgm_clk_div & 0x0F) + 1);
                                                             
    return 0;
}
                                                                                                                
void set_err(int no, const char *fmt, ...)
{
    struct sysc_t *sys = &gSYSC;
    va_list ap;

    sys->err_no = no;
    memset(sys->err_msg, 0, sizeof(sys->err_msg));
    va_start(ap, fmt);
    __doprnt(sys->err_msg, sizeof(sys->err_msg) - 1, fmt, ap);
    va_end(ap);
}

int get_err(char *errmsg)
{
    struct sysc_t *sys = &gSYSC;
    int rc = 0;

    if (sys->err_no) {
        rc = sys->err_no;
        strncpy(errmsg, sys->err_msg, strlen(sys->err_msg)+1); /* + '\0' */

        /* reset error number and message */
        sys->err_no = 0;
        memset(sys->err_msg, 0, sizeof(sys->err_msg));
    }

    return rc;
}

#if 0
unsigned long volatile card_detect_time = 0;

void gpio_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned int red, fed;

    red = readl(GPIO_RED_STATUS);
    fed = readl(GPIO_FED_STATUS);

    writel(red, GPIO_RED_STATUS);
    writel(fed, GPIO_FED_STATUS);

    card_detect_time = read_c0_count() + sys_cpu_clk/16; /* 0.25 sec */
    if (card_detect_time == 0)
        card_detect_time++;
}

extern void sd_disk_remove(void);
extern void sd_disk_insert(void);
extern void ms_disk_remove(void);
extern void ms_disk_insert(void);


int check_card_detect(void)
{
    if (card_detect_time == 0)
        return 0;

#if 0
    if ((int)(read_c0_count() - card_detect_time) >= 0) {
        if ((readl(GPIO_LEVEL) & SD_CARDDECT)) 
            sd_disk_remove();
        else 
            sd_disk_insert();

#if CONFIG_MS
        if ((readl(GPIO_LEVEL) & MS_CARDDECT)) 
            ms_disk_remove();
        else
            ms_disk_insert();
#endif
        
        card_detect_time = 0;
        return 1;
    }
#endif
    
    return 0;
}



int enable_carddect_irq(int card)
{
    int rc = 0;
    static int irq_init = 0;
    uint32_t oe;

    if (!irq_init) {
        rc = request_irq(IRQ_GPIO, gpio_interrupt, 0, "gpio", 0);
    }

    oe = ~card & readl(GPIO_OE);
    writel(oe, GPIO_OE);

    oe = card | readl(GPIO_RED);
    writel(oe, GPIO_RED);

    oe = card | readl(GPIO_FED);
    writel(oe, GPIO_FED);

    oe = ~card & readl(GPIO_INT_MASK);
    writel(oe, GPIO_INT_MASK);

    return rc;
    
}
#endif
