/**
 *  @file   i80.c
 *  @brief  i80 system interface (via Kirin I80 master)
 *  $Id: i80.c,v 1.4 2016/07/22 09:39:05 joec Exp $
 *  $Author: joec $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2012/12/19  Hugo  New file.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <sys.h>
#include <debug.h>

#include "i80.h"


#define RETRY       5
#define CDIV(X, Y)  (((X) + ((Y) - 1)) / (Y))   // compute ceil(X/Y)
#define FDIV(X, Y)  ((X) / (Y))                 // compute floor(X/Y)

struct i80c i80_ctr;
struct i80c *pi80;

/*
 * Panel API (function pointer for performance issue)
 */

#define SAFE_API(F) (F ? F : (void *) safe)
int safe (void) { return 0; }


int (*i80_window) (struct i80c *pi80, int x, int y, int w, int h) = (void *) safe;
int (*i80_locate) (struct i80c *pi80, int x, int y) = (void *) safe;


#include "i80_ctrl_setting.c"



/*
 * Synopsis     int i80_check_cmd (struct i80c *pi80);
 * Description  check result of command issue
 * Parameters   pi80 - point to i80 private data
 * Return       data value for success, -E_CMD_TIMEOUT for timeout error
 */
static int
i80_check_cmd (struct i80c *pi80)
{
    unsigned int val;
    int i;

    for (i = 0; i < RETRY; i++) {
        val = READL (I80_R11);
        if (!(val & CMD_BUSY))
            return (val & DATA_MASK);
        latch_delay (3);
    }

    return -E_CMD_TIMEOUT;
}

/*
 * Synopsis     int i80_write_reg (struct i80c *pi80, int idx, int val);
 * Description  write data (2-byte) to a register
 * Parameters   pi80 - point to i80 private data
 *              idx - i80 register index
 *              val - value to write
 * Return       0 for success, others for any error
 */
int
i80_write_reg (int idx, int val)
{
    int rc;
    pi80 = &i80_ctr;
    /* setup data count */
    if (pi80->bus_width <= BUS_WIDTH_9) {
        WRITEL (pi80->r04 | WD_CNT(2/1), I80_R04);
    } else {
        WRITEL (pi80->r04 | WD_CNT(2/2), I80_R04);
    }

    /* issue command of writing register */
    WRITEL (CMD_INDEX (idx) | CMD_ISSUE | WRITE_REG | CMD_DATA (val), I80_R11);

    /* wait command done */
    rc = i80_check_cmd (pi80);

    return (rc < 0 ? rc : 0);
}

/*
 * Synopsis     int i80_write_regv (struct i80c *pi80, int idx, char *val, int cnt);
 * Description  write data (variable length) to a register
 * Parameters   pi80 - point to i80 private data
 *              idx - i80 register index
 *              val - point to data
 *              cnt - length of data
 * Return       0 for success, others for any error
 */
int
i80_write_regv (int idx, char *val, int cnt)
{
    int i, rc;
    /* setup data count */
    if (pi80->bus_width <= BUS_WIDTH_9) {
        WRITEL (pi80->r04 | WD_CNT((cnt-0)/1), I80_R04);
    } else {
        WRITEL (pi80->r04 | WD_CNT((cnt-1)/2), I80_R04);
    }

    /* issue command of writing register */
    i = 0;
    do {
        WRITEL (CMD_INDEX (idx) | CMD_ISSUE | WRITE_REG | DUMMY_EN | (val[i] << 8) | (val[i+1] << 0), I80_R11);
        latch_delay (300); //Too fast will cause wrong timing.
    } while ((i += 2) < cnt);

    /* wait command done */
    rc = i80_check_cmd (pi80);

    return (rc < 0 ? rc : 0);
}

/*
 * Synopsis     int i80_read_reg (struct i80c *pi80, int idx);
 * Description  read data (2-byte) from a register
 * Parameters   pi80 - point to i80 private data
 *              idx - i80 register index
 * Return       value of data
 */
int
i80_read_reg (int idx)
{
    /* setup write address mode */
    WRITEL (pi80->r04 | WD_CNT (1), I80_R04);

    /* setup data count */
    if (pi80->bus_width <= BUS_WIDTH_9) {
        WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->hrd) | RD_CNT(2/1 + 1), I80_R12);
    } else {
        WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->frd) | RD_CNT(2/2 + 1), I80_R12);
    }

    /* issue command of reading register */
    WRITEL (CMD_INDEX (idx) | CMD_ISSUE | READ_REG | DUMMY_EN, I80_R11);

    /* wait command done */
    return i80_check_cmd (pi80);
}

/*
 * Synopsis     
 * Description
 * Parameters
 * Return
 */
int
i80_read_regv (int idx, char *val, int cnt)
{
    int i, rc;

    /* setup write address mode */
    WRITEL (pi80->r04 | WD_CNT (1), I80_R04);

    for (i = 0; i < cnt; i += 2) {
        /* setup data count */
        if (pi80->bus_width <= BUS_WIDTH_9) {
            WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->hrd + i/1) | RD_CNT(2/1 + 1), I80_R12);
        } else {
            WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->frd + i/2) | RD_CNT(2/2 + 1), I80_R12);
        }

        /* issue command of reading register */
        WRITEL (CMD_INDEX(idx) | CMD_ISSUE | READ_REG | DUMMY_EN, I80_R11);

        /* wait command done */
        rc = i80_check_cmd (pi80);
        if (rc < 0)
            return rc;

        val[i] = (rc >> 8) & 0xff;
        if (i + 1 < cnt)
            val[i+1] = (rc >> 0) & 0xff;
    }

    return 0;
}

/*
 * Synopsis     void i80_write_data (unsigned int val);
 * Description  write data to i80 lcm
 * Parameters   pi80 - point to i80 private data
 *              val - pixel data
 * Return       none
 */
int
i80_write_data (struct i80c *pi80, int val)
{
    int rc;

    /* setup data count */
    if (pi80->bus_width <= BUS_WIDTH_9) {
        WRITEL (pi80->r04 | WD_CNT (2/1), I80_R04);
    } else {
        WRITEL (pi80->r04 | WD_CNT (2/2), I80_R04);
    }

    /* issue command of writing pixel */
    if (pi80->data_width == DATA_WIDTH_16) {
        val = RGB666_TO_RGB565 (val);
        WRITEL (CMD_INDEX (pi80->lcm->wmi) | CMD_ISSUE | WRITE_GRAM | DATA_16BIT | val, I80_R11);
    } else {
        val = CMD_DATA (val);
        WRITEL (CMD_INDEX (pi80->lcm->wmi) | CMD_ISSUE | WRITE_GRAM | DATA_18BIT | val, I80_R11);
    }

    /* wait command done */
    rc = i80_check_cmd (pi80);

    return (rc < 0 ? rc : 0);
}

/*
 * Synopsis     unsigned int i80_read_data (void);
 * Description  read data from i80 lcm
 * Parameters   pi80 - point to i80 private data
 * Return       pixel data
 */
int
i80_read_data (struct i80c *pi80)
{
    int rc;

    /* setup write address mode */
    WRITEL (pi80->r04 | WD_CNT (1), I80_R04);

    /* setup data count */
    if (pi80->bus_width <= BUS_WIDTH_9) {
        WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->hmd) | RD_CNT(2/1 + 1), I80_R12);
    } else {
        WRITEL (pi80->r12 | DUMMY_CNT(pi80->lcm->fmd) | RD_CNT(2/2 + 1), I80_R12);
    }

    /* issue command of reading pixel */
    if (pi80->data_width == DATA_WIDTH_16) {
        WRITEL (CMD_INDEX (pi80->lcm->rmi) | CMD_ISSUE | READ_GRAM | DATA_16BIT | DUMMY_EN, I80_R11);
    } else {
        WRITEL (CMD_INDEX (pi80->lcm->rmi) | CMD_ISSUE | READ_GRAM | DATA_18BIT | DUMMY_EN, I80_R11);
    }

    /* wait command done */
    rc = i80_check_cmd (pi80);
    if (rc < 0)
        return rc;

    if (pi80->data_width == DATA_WIDTH_16) {
        return BGR565_TO_RGB666 (rc);
    } else {
        return BGR666_TO_RGB666 (rc);
    }
}

/*
 * Synopsis     void i80_burst_setup (unsigned int cnt);
 * Description  setup burst pixel count, data width, and issue command
 * Parameters   pi80 - point to i80 private data
 *              cnt - pixel count
 * Return       none
 */
int
i80_burst_setup (struct i80c *pi80, int cnt)
{
    assert (pi80 != NULL);
    assert (cnt <= 0xffffff);

    pi80->total = cnt;
    pi80->count = 0;

    /* reset FIFO */
    WRITEL (FIFO_RESET, I80_R13);

    /* setup pixel count, data width and issue command */
    if (pi80->data_width == DATA_WIDTH_16) {
        WRITEL (BURST_ISSUE | BURST_16BIT | cnt, I80_R13);
    } else {
        WRITEL (BURST_ISSUE | BURST_18BIT | cnt, I80_R13);
    }

    return 0;
}

/*
 * Synopsis     void i80_burst_write (unsigned int data);
 * Description  write burst data into data port
 * Parameters   pi80 - point to i80 private data
 *              data - pixel data
 * Return       none
 */
int
i80_burst_write (struct i80c *pi80, int data)
{
    assert (pi80 != NULL);

#if 0
    /* wait fifo is avail */
    while (READL (I80_R13) & FIFO_FULL);
#endif
    pi80->count++;

    /* handle pixel for 16-bit data width */
    if (pi80->data_width == DATA_WIDTH_16) {
        data = RGB666_TO_RGB565 (data);
        if (pi80->count % 2)
            /* odd */
            pi80->pixel = data;
        else
            /* even */
            data = (data << 16) | pi80->pixel;

        if ((pi80->count % 2) && pi80->count < pi80->total)
            return 0;
    }

    /* push data into fifo */
    WRITEL (data, I80_R14);

    return 0;
}

int
i80_burst_write_is_done (struct i80c *pi80)
{
    assert (pi80 != NULL);

    if (pi80->count == pi80->total) {
        /* wait burst write done */
        while (READL (I80_R13) & BURST_ISSUE);

        return 1;
    }
    
    return 0;
}

/*
 * Synopsis     void i80_set_data_width (int bus_width);
 * Description  set data width
 * Parameters   pi80 - point to i80 private data
 *              bus_width - data bus pins.
 * Return       none
 */
int
i80_set_data_width (int bus_width)
{
    assert (pi80 != NULL);

    if ((bus_width == 16)||(bus_width == 8)) {
        pi80->data_width = DATA_WIDTH_16;
    } else
    if ((bus_width == 18)||(bus_width == 9)) {
        pi80->data_width = DATA_WIDTH_18;
    } else {
        return -E_BAD_BUS_WIDTH;
    }

    return 0;
}

/*
 * Synopsis     int i80_get_data_width (void)
 * Description  get data width
 * Parameters   pi80 - point to i80 private data
 * Return       width - size of a pixel
 */
int
i80_get_data_width (void)
{
    assert (pi80 != NULL);

    return (pi80->data_width == DATA_WIDTH_16 ? 16 : 18);
}

/*
 * Synopsis     void i80_set_bus_width (int width);
 * Description  set output bus width
 * Parameters   pi80 - point to i80 private data
 *              width - output bus width
 * Return       0 for success, -1 for any error
 */
int
i80_set_bus_width (int width)
{
    unsigned int r00 = READL (I80_R00) & ~I80_R00_MASK;
    unsigned int r04 = READL (I80_R04) & ~I80_R04_MASK;
    unsigned int r12 = READL (I80_R12) & ~I80_R12_MASK;

    if (width == 8) {
        pi80->bus_width = BUS_WIDTH_8;
        r00 |= BUS_8BIT;
        r04 |= WA_BYTE | WD_BYTE | WA_CNT(pi80->lcm->hac) | WD_CNT(0);
        r12 |= RD_BYTE | DUMMY_CNT(0) | RD_CNT(0);
    } else
    if (width == 9) {
        pi80->bus_width = BUS_WIDTH_9;
        r00 |= BUS_9BIT;
        r04 |= WA_BYTE | WD_BYTE | WA_CNT(pi80->lcm->hac) | WD_CNT(0);
        r12 |= RD_BYTE | DUMMY_CNT(0) | RD_CNT(0);
    } else
    if (width == 16) {
        pi80->bus_width = BUS_WIDTH_16;
        r00 |= BUS_16BIT;
        r04 |= WA_WORD | WD_WORD | WA_CNT(pi80->lcm->fac) | WD_CNT(0);
        r12 |= RD_WORD | DUMMY_CNT(0) | RD_CNT(0);
    } else
    if (width == 18) {
        pi80->bus_width = BUS_WIDTH_18;
        r00 |= BUS_18BIT;
        r04 |= WA_WORD | WD_WORD | WA_CNT(pi80->lcm->fac) | WD_CNT(0);
        r12 |= RD_WORD | DUMMY_CNT(0) | RD_CNT(0);
    } else {
        /* never reach here */
        return -E_BAD_BUS_WIDTH;
    }

    WRITEL (r00, I80_R00);
    pi80->r04 = r04;
    pi80->r12 = r12;

    return 0;
}

int
i80_get_bus_width (void)
{
    unsigned int r00;

    r00 = READL (I80_R00);
    switch (r00 & BUS_MASK) {
        case BUS_8BIT:
            return 8;
        case BUS_9BIT:
            return 9;
        case BUS_16BIT:
            return 16;
        case BUS_18BIT:
            return 18;
        default:
            /* never reach here */
            return -E_BAD_BUS_WIDTH;
    }
}

int
i80_set_input (int sel)
{
    unsigned int r10;
	
    WRITEL (pi80->r04 | WD_CNT(pi80->lcm->fac), I80_R04);
    r10 = READL (I80_R10) & ~SRC_MASK;
    WRITEL (r10 | SRC_SEL(sel), I80_R10);
    return 0;
}

int
i80_get_input (void)
{
    unsigned int r10;

    r10 = READL (I80_R10) & SRC_MASK;
    return (r10 >> SRC_SHIFT);
}

int
i80_set_output (int sel)
{
    unsigned int r10;

    r10 = READL (I80_R10) & ~MODE_MASK;
    WRITEL (r10 | MODE_SEL(sel), I80_R10);
    return 0;
}

int
i80_get_output (void)
{
    unsigned int r10;

    r10 = READL (I80_R10) & MODE_MASK;
    return (r10 >> MODE_SHIFT);
}
/*
 * Synopsis     int i80_get_bus_width (void);
 * Description  get output bus width
 * Parameters   pi80 - point to i80 private data
 * Return       width - output bus width
 */


/*
 * Synopsis     void i80_reset (void);
 * Description  hardware reset of i80 lcm
 * Parameters   pi80 - point to i80 private data
 * Return       none
 */
void
i80_reset (void)
{
    unsigned int val = READL (I80_R00);

    assert (pi80 != NULL);

    /* pull low SYS_RST_ */
    WRITEL (val & ~SYS_RST_, I80_R00);

    /* wait tRES=1ms */
    usleep (1000);

    /* pull high SYS_RST_ */
    WRITEL (val | SYS_RST_, I80_R00);
}


/*
 * Synopsis     void i80_init (enum I80_PANEL_LIST i80_panel, int width, int height);
 * Description  init i80 master
 * Parameters   pi80 - point to i80 private data
 * Return       none
 */
int
i80_init (enum I80_PANEL_LIST i80_panel, int bus_width, int width, int height)
{
    pi80 = &i80_ctr;
    /****** GPIO pin mux, T582 i80 must set it! *******/
    writel(readl(0xb9000004) | 0x00000002, 0xb9000004);
    writel(readl(0xb9000008) | 0x00000002, 0xb9000008);
    writel(readl(0xb900000c) & 0xfffffffD, 0xb900000c);
    /**************************************************/

    
    /* init controller */   
    switch (i80_panel){
        case ILI9320:
            i80_T582_ili9320_setting(width, height); 
        break;
        case ILI9327:
			i80_T582_ili9327_setting(width, height); 
			break;
        case ILI9341:
            i80_T582_ili9341_setting(width, height); 
			break;
        break;
        case HX8347:            
            i80_T582_ili9320_setting(width, height); 
        break;
        
        default:
            printf("Not yet support!");
        break;
    
    
    }
    i80_set_bus_width (bus_width);
    i80_set_data_width (bus_width);
   
    return 0;
}

/*
 * I80 Panel Common Functions
 */

int
i80_set_point (int x, int y, int color)
{
    if (x < 0 || x >= pi80->lcm->xmax || y < 0 || y >= pi80->lcm->ymax)
        return -E_OUT_OF_RANGE;

    pi80->lcm->locate (pi80, x, y);
    return i80_write_data (pi80, color);
}

int
i80_get_point (int x, int y)
{
    if (x < 0 || x >= pi80->lcm->xmax || y < 0 || y >= pi80->lcm->ymax)
        return -E_OUT_OF_RANGE;

    pi80->lcm->locate (pi80, x, y);
    return i80_read_data (pi80);
}

int
i80_clear (int color)
{
    int xmax = pi80->lcm->xmax;
    int ymax = pi80->lcm->ymax;
    int x, y;

    pi80->lcm->window (pi80, 0, 0, xmax, ymax);
    i80_burst_setup (pi80, xmax * ymax);

    for (y = 0; y < ymax; y++) {
        for (x = 0; x < xmax; x++) {
            i80_burst_write (pi80, color);
        }
    }
    assert (i80_burst_write_is_done (pi80));

    return 0;
}

int
i80_frate (void)
{
    unsigned int deadline;
    int xmax = pi80->lcm->xmax;
    int ymax = pi80->lcm->ymax;
    int i, loop, cnt = 0;

    loop = xmax * ymax;
    if (pi80->data_width == DATA_WIDTH_16)
        loop /= 2;

    pi80->lcm->window (pi80, 0, 0, xmax, ymax);
    deadline = read_c0_count32 () + (sys_cpu_clk / 2);
    while (read_c0_count32 () < deadline) {
        i80_burst_setup (pi80, xmax * ymax);
        if (cnt++ % 2) {
            for (i = 0; i < loop; i++)
                WRITEL (0xffffffff, I80_R14);
        } else {
            for (i = 0; i < loop; i++)
                WRITEL (0x00000000, I80_R14);
        }
        while (READL (I80_R13) & BURST_ISSUE);
        WRITEL (FIFO_RESET, I80_R13);   // reset fifo anyway
    }

    return cnt;
}

/*****************************************************************************/


