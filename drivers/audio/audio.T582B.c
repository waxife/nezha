/**
 *  @file   audio2.c
 *  @brief  audio encode/decode driver
 *  $Id: audio.T582B.c,v 1.22 2014/04/29 01:41:59 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.22 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/11  Jonathan New file.
 *          2013/11/01  Hugo     porting from audio2.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <bitops.h>
#include <debug.h>
#include "irq.h"
#include "interrupt.h"

#include "audio.T582B.h"
#include "sdm.h"
#include "../../sys/include/gpio.h"

char au_dec_cnt = 0;
char au_enc_cnt = 0;

#if AUDIO_DRV_STATISTIC
struct au_statistic au_st[1];
#endif /* AUDIO_DRV_STATISTIC */

struct au_ctx_t au_bell[1];

#define DEC_BUFDESC_SIZE    2048
#define DEC_BUFDESC_NUM     2

unsigned char __attribute__((aligned(4))) au_nor_dec_doorbell_buf[DEC_BUFDESC_NUM][DEC_BUFDESC_SIZE];

int doorbell_cmd = 0;
int ring_bell_num = 0;

char audio_sample_rate[5] = {2, 16, 8, 8, 4}; // unit: kbyte/s

/******************************************************************************
 * Sigma-Delta codec and modulator (SDM)
 ******************************************************************************/

static inline void
sdm_adc_poweron (void)
{
    writel(ADC_AMP_2 | ADC_OSR_256, SDM_ADC_CONF_1); // no gain
    writel((0x8 << 4) | SDM_ADC_EN, SDM_ADC_CONF_0);
}

static inline void
sdm_adc_poweroff (void)
{
    writel(SDM_ADC_RESET, SDM_ADC_CONF_0);
}

static inline void
sdm_dac_poweron (void)
{
    writel(DAC_AMP_1, SDM_DAC_CONF_0); // 1 gain
    writel(SDM_DAC_PWR_ON, SDM_DAC_CONF_1);
}

static inline void
sdm_dac_poweroff (void)
{
    writel(SDM_DAC_PWR_OFF, SDM_DAC_CONF_1);
}

/******************************************************************************
 * Sigma-Delta codec and modulator
 ******************************************************************************/

static inline void
auc_sdm_adc_poweron (void)
{
    REG_SET_BIT(AU_CTL_SGMDLT_CONF, SDM_ENC_PWR_OFFSET, 1);
}

static inline void
auc_sdm_adc_poweroff (void)
{
    REG_SET_BIT(AU_CTL_SGMDLT_CONF, SDM_ENC_PWR_OFFSET, 0);
}

static inline void
auc_sdm_dac_poweron (void)
{
    REG_SET_BIT(AU_CTL_SGMDLT_CONF, SDM_DEC_PWR_OFFSET, 1);
    REG_SET_BITS(AU_CTL_SGMDLT_CONF, SDM_DEC_PDN_TEST_OFFSET, SDM_DEC_PDN_TEST_BITS, 0xf);
}

static inline void
auc_sdm_dac_poweroff (void)
{
    REG_SET_BIT(AU_CTL_SGMDLT_CONF, SDM_DEC_PWR_OFFSET, 0);
    REG_SET_BITS(AU_CTL_SGMDLT_CONF, SDM_DEC_PDN_TEST_OFFSET, SDM_DEC_PDN_TEST_BITS, 0x0);
}

static inline int
auc_sdm_get_adc_gain (void)
{
    return REG_GET_BITS(AU_CTL_SGMDLT_CONF, SDM_ENC_GAIN_OFFSET, SDM_ENC_GAIN_BITS);
}

static inline void
auc_sdm_set_adc_gain (int value)
{
    REG_SET_BITS(AU_CTL_SGMDLT_CONF, SDM_ENC_GAIN_OFFSET, SDM_ENC_GAIN_BITS, value);
}

static inline int
auc_sdm_get_dac_gain (void)
{
    return REG_GET_BITS(AU_CTL_SGMDLT_CONF, SDM_DEC_GAIN_OFFSET, SDM_DEC_GAIN_BITS);
}

static inline void
auc_sdm_set_dac_gain (int value)
{
    REG_SET_BITS(AU_CTL_SGMDLT_CONF, SDM_DEC_GAIN_OFFSET, SDM_DEC_GAIN_BITS, value);
}

/******************************************************************************
 * System API (high cohesion enhances readability)
 ******************************************************************************/

#define MAX_LOOP    (10*1000)
static int
wait_ready (void)
{
    int i;

    for (i = 0; i < MAX_LOOP; i++) {
        if ((readl(AU_CTL_STATUS0) & AU_RSTATUS_BUSY) == 0)
            return 0;
        usleep (1);
    }

    /* timeout */
    return -1;
}

static inline void
auc_dec_enable (void)
{
    REG_SET_BIT(AU_CTL_DEC_SYSTEM, DEC_EN_OFFSET, 1);
}

static inline void
auc_dec_disable (void)
{
    REG_SET_BIT(AU_CTL_DEC_SYSTEM, DEC_EN_OFFSET, 0);
}

static inline int
auc_dec_isEnable (void)
{
    int enable;
    enable = REG_GET_BIT(AU_CTL_DEC_SYSTEM, DEC_EN_OFFSET);
    return (enable == 1);
}

int
auc_dec_reset (void)
{
    REG_SET_BIT(AU_CTL_DEC_SYSTEM, DEC_RESET_OFFSET, 1);
    int rc = wait_ready ();
    return rc;
}

static inline void
auc_dec_resume (void)
{
    REG_SET_BIT(AU_CTL_DEC_SYSTEM, DEC_RESUME_OFFSET, 1);
}

static inline void
auc_dec_pause (void)
{
    REG_SET_BIT(AU_CTL_DEC_SYSTEM, DEC_PAUSE_OFFSET, 1);
}

static inline void
auc_enc_enable (void)
{
    REG_SET_BIT(AU_CTL_ENC_SYSTEM, ENC_EN_OFFSET, 1);
}

static inline void
auc_enc_disable (void)
{
    REG_SET_BIT(AU_CTL_ENC_SYSTEM, ENC_EN_OFFSET, 0);
}

int
auc_enc_reset (void)
{
    REG_SET_BIT(AU_CTL_ENC_SYSTEM, ENC_RESET_OFFSET, 1);
    int rc = wait_ready ();
    return rc;
}

/******************************************************************************
 * Configuration API (high cohesion enhances readability)
 ******************************************************************************/

static int
get_enc_format (void)
{
    return REG_GET_BITS(AU_CTL_CONFIG, FORMAT_ENC_OFFSET, FORMAT_ENC_BITS);
}

static void
set_enc_format (int value)
{
    REG_SET_BITS(AU_CTL_CONFIG, FORMAT_ENC_OFFSET, FORMAT_ENC_BITS, value);
}

static int
get_dec_format (void)
{
    return REG_GET_BITS(AU_CTL_CONFIG, FORMAT_DEC_OFFSET, FORMAT_DEC_BITS);
}

static void
set_dec_format (int value)
{
    REG_SET_BITS(AU_CTL_CONFIG, FORMAT_DEC_OFFSET, FORMAT_DEC_BITS, value);
}

static int
get_enc_buildin (void)
{
    return REG_GET_BITS(AU_CTL_CONFIG, BUILDIN_ENC_OFFSET, BUILDIN_ENC_BITS);
}

static void
set_enc_buildin (int value)
{
    REG_SET_BITS(AU_CTL_CONFIG, BUILDIN_ENC_OFFSET, BUILDIN_ENC_BITS, value);
}

static int
get_dec_buildin (void)
{
    return REG_GET_BITS(AU_CTL_CONFIG, BUILDIN_DEC_OFFSET, BUILDIN_DEC_BITS);
}

static void
set_dec_buildin (int value)
{
    REG_SET_BITS(AU_CTL_CONFIG, BUILDIN_DEC_OFFSET, BUILDIN_DEC_BITS, value);
    usleep (10000);
}

/******************************************************************************
 * experimental api
 ******************************************************************************/
#include <config.h>
#include <audio.h>

int
audio_init (void)
{
    int rc;

    rc = auc_enc_reset ();
    if (rc != 0) {
        ERROR ("auc_enc_reset(), rc=%d\n", rc);
        return -EIO;
    }

    rc = auc_dec_reset ();
    if (rc != 0) {
        ERROR ("auc_dec_reset(), rc=%d\n", rc);
        return -EIO;
    }

    return 0;
}

int
audio_init2 (unsigned int mode)
{
    unsigned int temp;
    int rc;

    if (mode & AU_CMD_ENC) {
        rc = auc_enc_reset();
        if (rc != 0) {
            ERROR ("auc_enc_reset(), rc=%d\n", rc);
            return -EIO;
        }
        auc_sdm_set_adc_gain (8);
        auc_sdm_adc_poweron ();
    }

    if (mode & AU_CMD_DEC) {
        rc = auc_dec_reset();
        if (rc != 0) {
            ERROR ("auc_dec_reset(), rc=%d\n", rc);
            return -EIO;
        }
        auc_sdm_set_dac_gain (8);
    }

    temp = readl(AU_CTL_CONFIG);
    temp |= WS_POLAR_FALLING | BCLK_POLAR_INVERSE | ACLK_POLAR_NORMAL | DATA_MODE_STANDARD | LB_STUFF_ZERO;
    writel(temp, AU_CTL_CONFIG);

    return 0;
}

int
audio_config (struct au_ctx_t *au)
{
    int buf_len, des_num;
    int i;

    buf_len = au->cfg.buf_len;
    des_num = au->cfg.des_num;
    for (i = 0; i < des_num; i++) {
        if (au->cmd_mode & AU_CMD_DEC)
            writel (((buf_len - 1) << 15) | (des_num - 1), AU_DEC_BD_CONF(i));

        if (au->cmd_mode & AU_CMD_ENC)
            writel (((buf_len - 1) << 15) | (des_num - 1), AU_ENC_BD_CONF(i));
    }

    if (au->cmd_mode & AU_CMD_ENC) {
        set_enc_format (au->cfg.format);
    }

    if (au->cmd_mode & AU_CMD_DEC) {
        set_dec_format (au->cfg.format);
    }

    return 0;
}

int
audio_open (struct au_ctx_t *au, int mode)
{
    if (mode & AU_CMD_DEC) {
        if (au_dec_cnt) {
            return -EBUSY;
        } else {
            au_dec_cnt++;
        }
    }

    if (mode & AU_CMD_ENC) {
        if (au_enc_cnt) {
            return -EBUSY;
        } else {
            au_enc_cnt++;
        }
    }

    au->cmd_mode = mode;
    audio_init2(au->cmd_mode);
    audio_config (au);

    if (mode & AU_CMD_DEC) {
        gpio_clear(9);  /* enable audio mute */
        set_dec_buildin (BUILDIN_0x0000);
        auc_sdm_dac_poweroff ();
        sdm_dac_poweroff ();
        auc_dec_disable ();
        
        sdm_dac_poweron ();
        auc_dec_enable ();
        set_dec_buildin (BUILDIN_0x8000);
        set_dec_buildin (BUILDIN_0x0000);
        auc_sdm_dac_poweron ();
        set_dec_buildin (BUILDIN_DISABLE);
        gpio_set (9); /* disable audio mute */
    }

    if (mode & AU_CMD_ENC) {
        sdm_adc_poweron ();
        auc_enc_enable ();
    }

    return 0;
}

int
audio_close (struct au_ctx_t *au)
{
    int mode = au->cmd_mode;
    int i;

    if (mode & AU_CMD_DEC) {
        if (au_dec_cnt == 0) {
            WARN("Audio decoder have not be opened.");
            return 0;
        } else {
            au_dec_cnt--;
        }

        gpio_clear(9);  /* enable audio mute */
        set_dec_buildin (BUILDIN_0x0000);
        auc_sdm_dac_poweroff ();
        sdm_dac_poweroff ();
        auc_dec_disable ();
        for (i = 0; i < 4; i++)
            writel (0, AU_DEC_BD_HS(i));
    }

    if (mode & AU_CMD_ENC) {
        if (au_enc_cnt == 0) {
            WARN("Audio encoder have not be opened.");
            return 0;
        } else {
            au_enc_cnt--;
        }

        sdm_adc_poweroff ();
        auc_enc_disable ();
        for (i = 0; i < 4; i++)
            writel (0, AU_ENC_BD_HS(i));
    }

    return 0;
}

int
audio_read_nonblock (struct au_ctx_t *au, void *buffer, int length)
{
    struct au_io *io = &au->rd;
    int idx, val, status;
    int des_num = au->cfg.des_num;
    int i = 0;

    if ((au->cmd_mode & AU_CMD_ENC) == 0) {
        ERROR ("bad cmd_mode(%x)\n", au->cmd_mode);
        return -EIO;
    }

    if ((unsigned int)buffer != io->buffer || length != io->length) {
        /* reset buffer descriptors */
        writel(readl(AU_CTL_INT_STATUS) | 0xE0, AU_CTL_INT_STATUS);
        for (idx = 0; idx < 4; idx++)
            writel (0, AU_ENC_BD_HS(idx));

        /* reset read process */
        io->buffer = (unsigned int)buffer;
        io->length = length;
        io->offset = 0;
        io->bd_head = 0;
        io->bd_tail = 0;
    }

    /* check interrupt status */
    status = readl(AU_CTL_INT_STATUS);
    if (status & INT_FIFO_OF) {
        ERROR ("audio_read_nonblock() get FIFO overflow interrupt status\n");
        INFO("io: offset = %d (0x%08X), bd_head = %d, bd_tail = %d\n",
             io->offset, io->offset, io->bd_head, io->bd_tail);
#if AUDIO_DRV_STATISTIC
        au_st->fifo_overflow_int++;
#endif /* AUDIO_DRV_STATISTIC */
        audio_close(au);
        audio_open(au, AU_MODE_RDONLY);
        status = 0;
        for (i = 0; i < 1000; i++) {
            if (readl(AU_CTL_INT_STATUS) & INT_FIFO_OF) {
                audio_close(au);
                audio_open(au, AU_MODE_RDONLY);
                usleep(100);
            } else {
                break;
            }
        }

        if (i == 1000) {
            ERROR ("FIFO overflow and reset encoder failed.\n");
            return -1;
        }

        au->enc_bd->w_ptr = 0;
        au->enc_bd->r_ptr = 0;
        io->bd_head = io->bd_tail;
        io->offset = (io->bd_head * CONFIG_AUMSG_BUFDES_SIZE);
    } else if (status == 0) {
        goto EXIT;
    }

    /* attach buffer descriptors */
    while (io->offset < io->length && (io->bd_head - io->bd_tail) < des_num) {
        idx = au->enc_bd->w_ptr;
        au->enc_bd->w_ptr = (au->enc_bd->w_ptr + 1) % des_num;
        val = ((io->buffer + io->offset) >> 1) & 0xffff;

        writel (val, AU_ENC_BD_ADDR(idx));
        writel (0x1, AU_ENC_BD_HS(idx));

        io->bd_head++;
        io->offset += CONFIG_AUMSG_BUFDES_SIZE;
    }

    /* detach buffer descriptors valid for CPU */
    if ((INT_ENC_BDES & status)) {
#if AUDIO_DRV_STATISTIC
        au_st->enc_bdes_int++;
#endif /* AUDIO_DRV_STATISTIC */

        while (io->bd_tail < io->bd_head) {
            idx = au->enc_bd->r_ptr;
            val = readl (AU_ENC_BD_HS(idx));

            if ((val & 0x1) != 0) {
                /* codec busy */
                break;
            }

            io->bd_tail++;
            au->enc_bd->r_ptr = (au->enc_bd->r_ptr + 1) % des_num;
            putchar('.');
        }
    }

    /* clear interrtups */
    writel (status & 0xE0, AU_CTL_INT_STATUS);

EXIT:
    return (io->bd_tail * CONFIG_AUMSG_BUFDES_SIZE);
}

int
audio_read (struct au_ctx_t *au, void *buffer, int length)
{
    int rc;
    do {
        rc = audio_read_nonblock (au, buffer, length);
        if (rc < 0) {
            ERROR ("audio_read_nonblock(), rc=%d\n", rc);
            return -1;
        }
    } while (rc < length);

    return length;
}

int
audio_write_nonblock (struct au_ctx_t *au, const void *buffer, int length)
{
    struct au_io *io = &au->wr;
    int idx, val, status;
    int des_num = au->cfg.des_num;

    if ((au->cmd_mode & AU_CMD_DEC) == 0) {
        ERROR ("bad cmd_mode(%x)\n", au->cmd_mode);
        return -EIO;
    }

    if ((unsigned int)buffer != io->buffer || length != io->length) {
        /* reset buffer descriptors */
        writel(readl(AU_CTL_INT_STATUS) | 0x1C, AU_CTL_INT_STATUS);
        for (idx = 0; idx < 4; idx++)
            writel (0, AU_DEC_BD_HS(idx));

        /* reset write process */
        io->buffer = (unsigned int)buffer;
        io->length = length;
        io->offset = 0;
        io->bd_head = 0;
        io->bd_tail = 0;
    }

    /* attach buffer descriptors */
    while (io->offset < io->length && (io->bd_head - io->bd_tail) < des_num) {
        idx = io->bd_head % des_num;
        val = ((io->buffer + io->offset) >> 1) & 0xffff;

        writel (val, AU_DEC_BD_ADDR(idx));
        writel (0x1, AU_DEC_BD_HS(idx));

        io->bd_head++;
        io->offset += CONFIG_AUMSG_BUFDES_SIZE;
    }

    /* check interrupt status */
    status = readl (AU_CTL_INT_STATUS);
    if (status == 0)
        goto EXIT;

    /* detach buffer descriptors valid for CPU */
    if ((INT_DEC_BDES & status)) {
        while (io->bd_tail < io->bd_head) {
            idx = io->bd_tail % des_num;
            val = readl (AU_DEC_BD_HS(idx));

            if ((val & 0x1) != 0)
                /* codec busy */
                break;
            io->bd_tail++;
            putchar('.');
        }
    }

    /* clear interrtups */
    writel(status & 0x1C, AU_CTL_INT_STATUS);

EXIT:
    return (io->bd_tail * CONFIG_AUMSG_BUFDES_SIZE);
}

int
audio_write (struct au_ctx_t *au, const void *buffer, int length)
{
    int rc;

    do {
        rc = audio_write_nonblock (au, buffer, length);
        if (rc < 0) {
            ERROR ("audio_write_nonblock(), rc=%d\n", rc);
            return -1;
        }
    } while (rc < length);

    return length;
}

static int
audio_set_enc_format (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    set_enc_format (*(int*)arg);
    return 0;
}

static int
audio_get_enc_format (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = get_enc_format();
    return 0;
}

static int
audio_set_dec_format (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    set_dec_format (*(int*)arg);
    return 0;
}

static int
audio_get_dec_format (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = get_dec_format();
    return 0;
}

static int
audio_set_enc_pattern (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    set_enc_buildin (*(int*)arg);
    return 0;
}

static int
audio_get_enc_pattern (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = get_enc_buildin ();
    return 0;
}


static int
audio_set_dec_pattern (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    set_dec_buildin (*(int*)arg);
    return 0;
}

static int
audio_get_dec_pattern (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = get_dec_buildin ();
    return 0;
}

static int
audio_set_enc_gain (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    auc_sdm_set_adc_gain (*(int*)arg);
    return 0;
}

static int
audio_get_enc_gain (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = auc_sdm_get_adc_gain ();
    return 0;
}

static int
audio_set_dec_gain (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    auc_sdm_set_dac_gain (*(int*)arg);
    return 0;
}

static int
audio_get_dec_gain (struct au_ctx_t *au, void *arg)
{
    if (arg == NULL)
        return -1;

    *(int*)arg = auc_sdm_get_dac_gain ();
    return 0;
}


int
audio_ioctl (struct au_ctx_t *au, int request, void *arg)
{
    if (request == AUSIFMT)
        return audio_set_enc_format (au, arg);

    if (request == AUGIFMT)
        return audio_get_enc_format (au, arg);

    if (request == AUSOFMT)
        return audio_set_dec_format (au, arg);

    if (request == AUGOFMT)
        return audio_get_dec_format (au, arg);

    if (request == AUSIPAT)
        return audio_set_enc_pattern (au, arg);

    if (request == AUGIPAT)
        return audio_get_enc_pattern (au, arg);

    if (request == AUSOPAT)
        return audio_set_dec_pattern (au, arg);

    if (request == AUGOPAT)
        return audio_get_dec_pattern (au, arg);

    if (request == AUSIGAIN)
        return audio_set_enc_gain (au, arg);

    if (request == AUGIGAIN)
        return audio_get_enc_gain (au, arg);

    if (request == AUSOGAIN)
        return audio_set_dec_gain (au, arg);

    if (request == AUGOGAIN)
        return audio_get_dec_gain (au, arg);

    /* invalid request */
    return -1;
}

static int audec_nor_dec_int(struct au_ctx_t *au)
{
    int pa_end = au->nor_pa_start + au->nor_size;
    int pa_cur = au->nor_pa_cur;
    int i = 0;
    unsigned int addr = 0;
    unsigned int size = 0;

    if (au->nor_pa_cur == pa_end) {
		if(ring_bell_num>1){
			au->nor_pa_cur = au->nor_pa_start;
			pa_cur = au->nor_pa_cur ;
			ring_bell_num = ring_bell_num -1;
			dbg(3,"repeaet %d\n",ring_bell_num);
		}else{
			dbg(0, "End of audio stream.\n");
			return -1;
		}
    }

    i = au->dec_bd->w_ptr;
    //dbg(0, "ptr = %d, addr = 0x%08X, offset = 0x%08X\n", i, au->dec_bd->addr[i], au->dec_bd->offset[i]);
    addr = au->dec_bd->addr[i];
    size = (pa_end - pa_cur) > au->dec_bd->size ? au->dec_bd->size : (pa_end - pa_cur);
    memcpy((char *)addr, (char *)(pa_cur | 0x80000000), size);
    if (size < au->dec_bd->size) {
        memset((char *)(addr + size), 0, au->dec_bd->size - size);
    }

	writel(0x1,AU_DEC_BD_HS(au->dec_bd->w_ptr));
	
    au->nor_pa_cur = pa_cur + size;
    if (au->nor_pa_cur < pa_end) {
        au->dec_bd->w_ptr = (au->dec_bd->w_ptr + 1) % au->dec_bd->num;
    }

    return 1;
}

static void audio_dec_int(int irq,void *dev_id,struct pt_regs *regs)
{
	int rc =0;
	int i,val;
	//int time_threshold =50000;
	struct au_ctx_t *au = (struct au_ctx_t *)dev_id;
	for(i = 0;i < DEC_BUFDESC_NUM;i++){
	    val = readl (AU_DEC_BD_HS(i));

        if ((val & 0x01) == 0){
			rc = audec_nor_dec_int(au);
			writel((0x01<<4), AU_CTL_INT_STATUS);
			if(rc == -1)
				break;
		}
	}
	if(doorbell_cmd == 2){
		disable_irq(IRQ_AUD_CTRL);
		free_irq(IRQ_AUD_CTRL,au);
		audio_close (au);
		dbg(0,"cmd irq disable\n");
		doorbell_cmd = 0;
	}
	#if 0
	if(rc == -1){
		disable_irq(IRQ_AUD_CTRL);
		free_irq(IRQ_AUD_CTRL,au);
		while((readl (AU_DEC_BD_HS(au->dec_bd->w_ptr))&0x01) != 0){
			time_threshold = time_threshold -1; 
			usleep (10);
			if(time_threshold<0)
				break;
		}
	}
	#endif
	if(rc == -1){
		if((readl (AU_DEC_BD_HS(au->dec_bd->w_ptr))&0x01) == 0){
			disable_irq(IRQ_AUD_CTRL);
			free_irq(IRQ_AUD_CTRL,au);
			audio_close (au);
			dbg(0,"irq disable\n");
			doorbell_cmd = 0;
		}
	}
}

int audio_dec_irq(struct au_ctx_t *au)
{
	int rc=0;
	static struct irqaction audio;
	
	audio.handler = audio_dec_int;
    audio.flags = SA_INTERRUPT;
    audio.mask = 0;
    audio.name = "ringbell";
    audio.next = NULL;
    audio.dev_id = (void *)au;

	writel(readl(AU_CTL_INT_MASK) & ~(0x01 << 4), AU_CTL_INT_MASK);

	rc = setup_irq(IRQ_AUD_CTRL,&audio);
	if(rc<0)
	{
		ERROR("audio_open setup_irq IRQ_AUD_CTRL ERROR\r\n");
		goto EXIT;
	}
	dbg(0,"irq initial finish\n");
	return 0;
EXIT:
	disable_irq(IRQ_AUD_CTRL);
    free_irq(IRQ_AUD_CTRL,au);
	return -1;
}

void audio_doorbell_close(struct au_ctx_t *au)
{
	disable_irq(IRQ_AUD_CTRL);
	free_irq(IRQ_AUD_CTRL,au);
	audio_close (au);
	dbg(0,"doorbell is closed\n");
	doorbell_cmd = 0;
}

int audio_init_doorbell_buffer(struct au_ctx_t *au)
{
	int i = 0;
	int rc = 0;
	for(i = 0; i < au->dec_bd->num ; i++){
		rc =audec_nor_dec_int(au);
	}
	return rc ;
}
