/**
 *  @file   keypad.c
 *  @brief  Cheetah SAR keypad driver
 *  $Id: keypad.c,v 1.2 2014/01/28 02:15:14 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/03/24  C.N.Yu  New file.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <debug.h>
#include <io.h>

#define REG_SCALER_BASE         0xb0400000
#define PAGE_0                  0
#define PAGE_1                  0x1000
#define PAGE_2                  0x2000
#define PAGE_3                  0x3000
#define REG_SCALER_INT_STS_MASK (REG_SCALER_BASE | PAGE_2 | (0xE1 << 4))
#define REG_SCALER_SAR_MASK     (REG_SCALER_BASE | PAGE_2 | (0xE7 << 4))
#define REG_SAR1_CTRL           (REG_SCALER_BASE | PAGE_3 | (0x43 << 4))
#define REG_SAR1_THRES          (REG_SCALER_BASE | PAGE_3 | (0x45 << 4))
#define REG_SAR_SEL             (REG_SCALER_BASE | PAGE_3 | (0x48 << 4))
#define REG_SAR1_RLTM_VAL       (REG_SCALER_BASE | PAGE_3 | (0x44 << 4))
#define REG_SAR1_LTCH_VAL       (REG_SCALER_BASE | PAGE_3 | (0x47 << 4))

#define INT_STS_MASK_SAR_TOG    0x02
#define SCALER_SAR1_MASK        0x40
#define SCAKER_SAR1_TOG         0x02

#define SAR1_VALUE_MASK         0xF0


#define GPI_SWITCH              0xb9000028         
#define GPI_SWTC_ON             0x01     
#define GPI_SWTC_OFF            0xfe
#define GPI_READ                0xb9000000
#define GPI_READ_MASK           0x10

static int sar1_open(void);
static int sar1_get_keyvalue(void);
static int sar1_close(void);

static int gpio_open(void);
static int gpio_get_keyvalue(void);
static int gpio_close(void);

struct keypad_func_info {
    int (*open)(void);
    int (*get_keyvalue)(void);
    int (*close)(void);
};

struct keypad_func_info keypad_func[] = {
    { sar1_open, sar1_get_keyvalue, sar1_close }, /* SAR1 */
    { gpio_open, gpio_get_keyvalue, gpio_close }, /* GPIO */
};

int keypad_id = 0; /* default keypad ID is 0 */
int last_key_val = -1;
unsigned int key_time_thres = 0;
unsigned int start_time = 0, response_time = 0;
#if 0
enum KEU_STAT {
    KEY_STAT_UP = 0, 
    KEY_STAT_DOWN,
    KEY_STAT_DOWN_LONG
};
int key_stat = KEY_STAT_UP;
#endif

static int sar1_open(void)
{
    /* select SAR1 source */
    writeb(readb(REG_SAR_SEL) & ~0x30, REG_SAR_SEL);

    /* Set threshold */
    writeb(0x08, REG_SAR1_THRES);

    /* Power on SAR1 */
    writeb(readb(REG_SAR1_CTRL) & ~0x02, REG_SAR1_CTRL);

    /* not mask SAR1 of interrupt */
    writeb(readb(REG_SCALER_SAR_MASK) & ~SCALER_SAR1_MASK, REG_SCALER_SAR_MASK);

    /* clear SAR1 of interrupt status */
    writeb(readb(REG_SCALER_SAR_MASK) & SCAKER_SAR1_TOG, REG_SCALER_SAR_MASK);

    key_time_thres = millisec(80);
    last_key_val = -1;
    start_time = 0;
    response_time = 0;

    return 0;
}

#if 0
static int inline sar1_read_ltchval(void)
{
    int key_val = 0;
    nt ltch_val = 0;

    ltch_val = readl(REG_SAR1_LTCH_VAL);
    switch (ltch_val & SAR1_VALUE_MASK) {
    case 0xE0:
        key_val = 1;
        break;
    case 0xD0:
        key_val = 2;
        break;
    case 0xA0:
        key_val = 3;
        break;
    case 0x80:
    case 0x70:
        key_val = 4;
        break;
    case 0x50:
        key_val = 5;
        break;
    case 0x30:
    case 0x20:
        key_val = 6;
        break;
    default:
        key_val = -1;
        break;
    }

    return key_val;
}
#endif

static int inline sar1_read_rltmval(void)
{
    int key_val = 0;
    int rltm_val = 0;

    rltm_val = readl(REG_SAR1_RLTM_VAL);
    switch (rltm_val & SAR1_VALUE_MASK) {
    case 0xE0:
        key_val = 1;
        break;
    case 0xD0:
        key_val = 2;
        break;
    case 0xA0:
        key_val = 3;
        break;
    case 0x80:
    case 0x70:
        key_val = 4;
        break;
    case 0x50:
        key_val = 5;
        break;
    case 0x30:
    case 0x20:
        key_val = 6;
        break;
    default:
        key_val = -1;
        break;
    }

    return key_val;
}

static int sar1_get_keyvalue(void)
{
    int rltm_val = -1;
    int key_val = -1;
    unsigned int key_time = 0;
#if 0
    int ltch_val = -1;

    if ((readl(REG_SCALER_INT_STS_MASK) & INT_STS_MASK_SAR_TOG)) {
        writel(SCAKER_SAR1_TOG, REG_SCALER_SAR_MASK);
        rltm_val = sar1_read_rltmval();
        ltch_val = sar1_read_ltchval();

        if (rltm_val < 0) {
            key_stat = KEY_UP;
        } else {
            key_stat = KEY_DOWN;
            start_time = read_c0_count32();
            last_key_val = rltm_val;
        }
    } else {
        rltm_val = sar1_read_rltmval();
        if (key_stat == KEY_DOWN && rltm_val == last_key_val) {
            key_time = read_c0_count32() - start_time;
            if (key_time > key_time_thres) {
                key_stat = KEY_DOWN_LONG;
                key_val = last_key_val;
            }
        } else if (key_stat == KEY_DOWN_LONG && rltm_val == last_key_val) {
            /* press the same key long time */
        }
    }
#else
    if ((readl(REG_SCALER_INT_STS_MASK) & INT_STS_MASK_SAR_TOG)) {
        writel(SCAKER_SAR1_TOG, REG_SCALER_SAR_MASK);
        rltm_val = sar1_read_rltmval();

        start_time = read_c0_count32();
        response_time = key_time_thres;
        last_key_val = rltm_val;
    } else {
        rltm_val = sar1_read_rltmval();
        if (rltm_val == last_key_val) {
            key_time = (read_c0_count32() - start_time);
            if (response_time && key_time > response_time) {
                key_val = last_key_val;
                response_time = 0;
            }
        } else {
            start_time = read_c0_count32();
            response_time = key_time_thres;
            last_key_val = rltm_val;
        }
    }
#endif

    return key_val;
}

static int sar1_close(void)
{
    /* clear SAR1 of interrupt status */
    writeb(readb(REG_SCALER_SAR_MASK) & SCAKER_SAR1_TOG, REG_SCALER_SAR_MASK);

    /* not mask SAR1 of interrupt */
    writeb(readb(REG_SCALER_SAR_MASK) | SCALER_SAR1_MASK, REG_SCALER_SAR_MASK);

    /* Power off SAR1 */
    writeb(readb(REG_SAR1_CTRL) | 0x02, REG_SAR1_CTRL);

    return 0;
}


static int gpio_open(void)
{
    writel(readl(GPI_SWITCH)|GPI_SWTC_ON, GPI_SWITCH);
    
    key_time_thres = millisec(100);
    last_key_val = -1;
    start_time = 0;

    return 0;
}

static int gpio_get_keyvalue(void)
{
	unsigned int hold_time;
	
    if (!(readl(GPI_READ) & GPI_READ_MASK)) {
        if(last_key_val == 1) {
        	hold_time = read_c0_count32() - start_time;
        	if(hold_time < (key_time_thres * 20))
        	    return -1;
        	else {
        		last_key_val = 2;
        	    return 2;
        	}
        }
        else if(last_key_val == 2) {
        	return 3;
        }
        else {
        	start_time = read_c0_count32();
        	last_key_val = 1;
        	return -1;
        }
    } else {
        if(last_key_val == 1) {
        	last_key_val = -1;
        	hold_time = read_c0_count32() - start_time;
        	if(hold_time > key_time_thres)
        		return 1;
        	else
        		return -1;
        }
        else if(last_key_val == 2) {
        	last_key_val = -1;
        	return -1;
        }
        else {
        	return -1;
        }
    }
}

static int gpio_close(void)
{
    writel(readl(GPI_SWITCH)&~GPI_SWTC_ON, GPI_SWITCH);

    return 0;
}

int keypad_setup(int id)
{
    if (id >= 0 && id < sizeof(keypad_func) / sizeof(struct keypad_func_info))
        keypad_id = id;
    else
        return -1;

    return 0;
}

int keypad_open(void)
{
    return keypad_func[keypad_id].open();
}

int keypad_get_keyvalue(void)
{
    return keypad_func[keypad_id].get_keyvalue();
}

int keypad_close(void)
{
    return keypad_func[keypad_id].close();
}

