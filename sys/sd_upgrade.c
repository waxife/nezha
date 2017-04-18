/**
 *  @file   sd_upgrade.c
 *  @brief  SD card upgrade function
 *  $Id: sd_upgrade.c,v 1.30 2015/08/05 10:33:01 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.30 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/17  lym 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys.h>
#include <dma.h>
#include <nor.h>
#include <debug.h>
#include <fat32.h>
#include "upgrade.h"
#include "display.h"
#include "r4kcache.h"
#include <watchdog.h>

extern unsigned int _fupg, _eupg;
extern unsigned int _ftext, _etext;
extern unsigned int _fdata, _edata;
extern unsigned int _upgrade_nor, _upgrade_size, _fringtong;

#define CONFIG_FILE "upgrade.sh"

#define DFU_KEY     "dfu"
#define SPI_KEY     "spi"
#define TM_KEY     "time"
#define RING_KEY    "ring"
#define ACTION_KEY  "action"
#define CLEAN_KEY   "clean"

#define KEY_COUNT   (5 + config_ringtong_num)

#define BUF_SIZE    SECT_SIZE
#define LINE_SIZE   (BUF_SIZE/4)

#define EQ_SIGN     (0x3D)
#define NL_SIGN     (0x0D)
#define NL_SIGN_E   (0x0A)
#define SP_SIGN     (0x20)
#define DH_SIGN     (0x5F)
#define DOT_SIGN    (0x2E)
#define COMMA_SIGN  (0x2C)


const unsigned long config_ringtong_num=0;


struct upg_sh {
	char *action_val;
	char *dfu_val;
	char *spi_val;
	char *time_val;
	char **ring_val;
};

__mips32__
static void go32(void *addr)
{
	void (*go_main)(void) = addr;

    cli();
    go_main();    
}

void _init_upg_sh(struct upg_sh *upg_info, char s_buf[KEY_COUNT][LINE_SIZE], char * r_buf[config_ringtong_num])
{
	char ** p_addr = &(upg_info->action_val);
	int i;
	
	for(i=0; i<(KEY_COUNT - config_ringtong_num -1); i++) {
		p_addr[i] = s_buf[i];
		s_buf[i][0] = 0;
	}
	
	upg_info->ring_val = r_buf;
	for(i=0; i<config_ringtong_num; i++) {
	
		upg_info->ring_val[i] = s_buf[i+3];
		
		s_buf[i+3][0] = 0;
	}
}

int _get_token(char *string, char sign, char *token)
{
	int i;
	int len = strlen(string);
	
	for(i = 0; i < len; i++) {
		if(sign == string[i]) {
			token[i] = 0;
			return 1;
		}
		token[i] = string[i];
	}
	
	token[i] = 0;	
	
	return 0;
}

void _get_val_string(const char *line, char *val)
{
	int i, v_ic = 0;
	for(i=0; line[i]!=0 && i<LINE_SIZE; i++) {
		if(line[i] != SP_SIGN) {
			val[v_ic++] = line[i];
		}
	}
	
	val[v_ic] = 0;
	
}

void _get_val_info(const int key_no, struct upg_sh *upg_info, const  char *line)
{
	if(key_no < 4) {
		char ** p_addr = &(upg_info->action_val);	
		_get_val_string(line, p_addr[key_no]);		
	}
	else {
		char ** p_addr = upg_info->ring_val;	
		_get_val_string(line, p_addr[key_no - 3]);		
	}
}

void _get_info_from_line(const char *line, struct upg_sh *upg_info)
{
	char key_string[LINE_SIZE];
	int i, ks_size = 0, val_start_idx = 0;

	char key_buf[config_ringtong_num][16];
	char * key_tbl[KEY_COUNT];
	
	/* note: The order of key words is reffering to the upg_info struct */
	key_tbl[0] = ACTION_KEY;
	key_tbl[1] = DFU_KEY;
	key_tbl[2] = SPI_KEY;
	key_tbl[3] = TM_KEY;
	
	for(i=1; i<=config_ringtong_num; i++) {
		
		sprintf(key_buf[i-1], "%s%d", RING_KEY, i);
		key_tbl[i+2] = key_buf[i-1];
	}
	

	key_tbl[KEY_COUNT-1] = CLEAN_KEY;
	
    for(i=0; line[i]!= 0 && i< LINE_SIZE; i++) {
			
    	if(line[i] == EQ_SIGN) {
    		val_start_idx = i+1; 
    		break;
    	}
    	else if(line[i] != SP_SIGN){
    		key_string[ks_size++] = line[i];

    	}
    } 



    key_string[ks_size] = 0;
    for(i=0; i<KEY_COUNT && ks_size > 0; i++) {
		
    	/* Find Key */
    	if(!strncmp(key_string, key_tbl[i], ks_size)) {
    		/* Get Val  */	

    	    key_string[ks_size] = 0;
    		_get_val_info(i, upg_info, (line+val_start_idx));   
    		break;
    	}
    }    
}

#define  LEGAL   1
#define  ILLEGAL 0
char _isLegal(char c)
{
	if(SP_SIGN == c || NL_SIGN == c || NL_SIGN_E == c || DH_SIGN == c 
	|| EQ_SIGN == c || DOT_SIGN == c || COMMA_SIGN == c) {
		return LEGAL;
	}
	else if(c > 0x2F && c < 0x3A) { /* 0-9 */
		return LEGAL;
	}
	else if(c > 0x40 && c < 0x5B) { /* A-Z */
		return LEGAL;	
	}
	else if(c > 0x60 && c < 0x7B) { /* a-z */
		return LEGAL;	
	}
	else {
		return ILLEGAL;
	}
}

void _get_upg_info(struct fd32 *fd, struct upg_sh *upg_info) 
{
	char buffer[BUF_SIZE];
	char line[LINE_SIZE];  
	int i, li, rc;
	
	_seek(fd, 0);

	rc = _read(fd, buffer, BUF_SIZE);   
	if(rc < 0) {
		ERROR("read error: %d\n", rc);
	}

	li = 0;
	for(i=0; i<rc + 1 /*BUF_SIZE*/; i++) {
		
		if(!_isLegal(buffer[i])) {   

			line[li] = 0;
			printf("line-1 %s\n", line);
			_get_info_from_line(line, upg_info);
			printf("line-1 done\n");
			break;
        }
        else if((buffer[i] == NL_SIGN || buffer[i] == NL_SIGN_E)) {   

            line[li] = 0;
			printf("line-2 %s\n", line);
            _get_info_from_line(line, upg_info);   
			printf("line-2 done\n");
            li = 0;
        }
        else {
           
            line[li++] = buffer[i];        
        }
    }
    
    printf("------------SD UPGRADE INFO---------------\n");
    printf("action      = %s\n", upg_info->action_val);
    printf("dfu_path    = %s\n", upg_info->dfu_val);
    printf("spi_path    = %s\n", upg_info->spi_val);
	printf("time_init    = %s\n", upg_info->time_val);
    for(i=0; i<config_ringtong_num; i++) {
        printf("ring%d_path  = %s\n", i+1, upg_info->ring_val[i]);    
    }
    printf("------------------------------------------\n");
}


void _do_upg(struct fat_t *fat, struct upg_sh *upg_info, int orders[], int oder_count)
{
	char *fupgrade, *upgrade_nor, *upgrade_size;
    struct fd32 fds[oder_count];

    char ** p_addr = &(upg_info->dfu_val);
    int rc = 0, i;
    fupgrade = (char *)&_fupg;
    upgrade_nor = (char *)&_upgrade_nor;
    upgrade_size = (char *)&_upgrade_size;
    for(i=0; i<oder_count; i++) {		
        if(orders[i]<2 ) {				
            rc = _open (&fds[i], p_addr[orders[i]], 0, FD_RDONLY);			
        }		
        else {
			if(orders[i] ==5) {}			
            else {				
				rc = _open (&fds[i], upg_info->ring_val[orders[i]-2], 0, FD_RDONLY);
            	}
        }
        if (rc < 0 && orders[i] !=5) {			
            ERROR ("_open(), rc=%d\n", rc);			
            return;
        }    		
		_close (&fds[i]);
    }    
    _ctx.upgrade_fds = (struct up_fd32 *)fds;
    _ctx.upgrade_orders = orders;
    nor_wrsr (0x00);
    printf("set nor-flash write unprotect.\n");
    watchdog_disable();
    memcpy (fupgrade, upgrade_nor, (int)upgrade_size);

    go32(upgrade_sequence);    
}

int
sd_auto_upgrade_with_time_stamp (unsigned long time_stamp_mark, char *time)
{		


    struct fat_t *fat = gSYSC.pfat;
    struct sdc *sdc = (struct sdc *)(fat->hcard);
    struct fd32 fd[1];
    char *fupgrade, *eupgrade, *ftext, *etext, *fdata;
    int rc, i;
    struct upg_sh upg_info;
   
    char string_buf[KEY_COUNT][LINE_SIZE];
    const unsigned int KEY_TBL_COUNT = 5 + config_ringtong_num;
    char *             KEY_TBL[KEY_TBL_COUNT];
    char               tbl_buf[config_ringtong_num][16];
    char *             struct_buf[config_ringtong_num];
    
    /* produce the key word table */
	

    KEY_TBL[0] = DFU_KEY;
    KEY_TBL[1] = SPI_KEY;    
	KEY_TBL[2] = ACTION_KEY;  
	KEY_TBL[3] = TM_KEY;
	
	/*clear flash userdata */
	
    for(i=1; i<=config_ringtong_num; i++) {
        sprintf(tbl_buf[i-1], "ring%d", i);
        KEY_TBL[i+1] = tbl_buf[i-1];
    };
	KEY_TBL[KEY_TBL_COUNT-1] = CLEAN_KEY;   


    fupgrade = (char *)&_fupg;
    eupgrade = (char *)&_eupg;
    ftext = (char *)&_ftext;
    etext = (char *)&_etext;
    fdata = (char *)&_fdata;
     
    /* prepare for cache */
    _ctx.dcache.ways = dcache.ways;
    _ctx.dcache.sets = dcache.sets;
    _ctx.dcache.waysize = dcache.waysize;
    _ctx.dcache.waybit = dcache.waybit;
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    /* prepare for dma */
    writel (1, DMA_CSR);    /* set AH0, AH1 little endian, enable DMA */
    /* prepare for fat */
    if (_ismounted () == 0) {
        rc = _upgrade_mount (fat);
        if (rc < 0) {
            ERROR ("_upgrade_mount(), rc=%d\n", rc);
            fat_check_tw_signature(1);
            return -1;
        }
    }
    
    /* prepare for sd */
    _ctx.sdc.rca = sdc->rca;
    _ctx.sdc.card.type = sdc->card.type;
    _ctx.sdc.card.readtimeout = sdc->card.readtimeout;
    printf("00000000000000000000000\r\n");
    /* search firmware */
    rc = _open (fd, CONFIG_FILE, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        _umount();
        fat_check_tw_signature(1);
        return -1;
    }

	printf("11111111111111111111111111\r\n");
    _init_upg_sh(&upg_info, string_buf, struct_buf);    
	printf("2222222222222222222222222222\r\n");
    _get_upg_info(fd, &upg_info);      
	printf("33333333333333333333333333333\r\n");

    _close (fd);

    int idx = 0;
    int order_count = 0;
    int pro_count = 0;   /* just for loop protection */

    char *action_val = upg_info.action_val;
    char token[strlen(action_val)+1];
    int orders[32];


    while(_get_token(action_val+idx, ',', token) && pro_count++ < 16) {
        for(i=0; i<KEY_TBL_COUNT; i++) {
            if(!strcmp(KEY_TBL[i], token)) {
                orders[order_count++] = i;
		
                break;
            }
        }
        idx += strlen(token)+1;    
    }
    for(i=0; i<KEY_TBL_COUNT; i++) {
        if(!strcmp(KEY_TBL[i], token)) {
            orders[order_count++] = i;
	
            break;
        }
    }

	
    orders[order_count] = -1;	
 	printf("This upgrade work with time stamp %08X. \n", (unsigned int)time_stamp_mark);
    _ctx.time_stamp_mark = time_stamp_mark;	


    _do_upg(fat, &upg_info, orders, order_count);

	if (time != NULL)
	{
		strcpy(time,upg_info.time_val);
	}
    
    return 0;
}

int
sd_auto_upgrade (char* time_str) {
	//char time_str[32];
	return sd_auto_upgrade_with_time_stamp(0, time_str);
}

#define REG_RTC_BASE            (0xB0800000)
#define REG_RTC_INFOR_1         (REG_RTC_BASE + 0x10c)
unsigned long get_sd_upgrade_stamp()
{	
	return readl(REG_RTC_INFOR_1);
}

char is_sd_upgrade_stamp_equal(unsigned long time_stamp_mark)
{
	return (time_stamp_mark == get_sd_upgrade_stamp());
}

int
sd_upgrade (char *filename, unsigned char action)
{
    struct fat_t *fat = gSYSC.pfat;
    struct sdc *sdc = (struct sdc *)(fat->hcard);
    struct fd32 fd[1];
    char *fupgrade, *eupgrade, *ftext, *etext, *fdata, *edata;
    int i;
    int rc;

    fupgrade = (char *)&_fupg;
    eupgrade = (char *)&_eupg;
    ftext = (char *)&_ftext;
    etext = (char *)&_etext;
    fdata = (char *)&_fdata;
    edata = (char *)&_edata;

    /* prepare for cache */
    _ctx.dcache.ways = dcache.ways;
    _ctx.dcache.sets = dcache.sets;
    _ctx.dcache.waysize = dcache.waysize;
    _ctx.dcache.waybit = dcache.waybit;

#ifdef UPGRADE_DEBUG
    printf ("_ctx.dcache.ways: %d\n", _ctx.dcache.ways);
    printf ("_ctx.dcache.sets: %d\n", _ctx.dcache.sets);
    printf ("_ctx.dcache.waysize: %d\n", _ctx.dcache.waysize);
    printf ("_ctx.dcache.waybit: %d\n", _ctx.dcache.waybit);
#endif

    /* prepare for dma */
    writel (1, DMA_CSR);    /* set AH0, AH1 little endian, enable DMA */

    /* prepare for fat */
    if (_ismounted () == 0) {
        rc = _upgrade_mount (fat);
        if (rc < 0) {
            ERROR ("_upgrade_mount(), rc=%d\n", rc);
            return -1;
        }
    }

    watchdog_disable();

    /* prepare for sd */
    _ctx.sdc.rca = sdc->rca;
    _ctx.sdc.card.type = sdc->card.type;
    _ctx.sdc.card.readtimeout = sdc->card.readtimeout;

#ifdef UPGRADE_DEBUG
    printf ("_ctx.sdc.rca: %u\n", _ctx.sdc.rca);
    printf ("_ctx.sdc.card.type: %d\n", _ctx.sdc.card.type);
    printf ("_ctx.sdc.card.readtimeout: %d\n", _ctx.sdc.card.readtimeout);
#endif

    /* search firmware */
    rc = _open (fd, filename, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        return -2;
    }

    _ctx.fd.region_cnt = fd->region_cnt;
    for (i = 0; i < fd->region_cnt; i++) {
        _ctx.fd.region[i].start = fat->root_start +
            (fd->region[i].start - FIRST_ROOTCLUSTER) * fat->sect_per_clus;
        _ctx.fd.region[i].end = _ctx.fd.region[i].start +
            fd->region[i].count * fat->sect_per_clus;
    }
#ifdef UPGRADE_DEBUG
    printf ("region_cnt:%d\n", _ctx.fd.region_cnt);
    for (i = 0; i < _ctx.fd.region_cnt; i++) {
        printf ("  R[%d] start:%6d, end:%6d\n", i,
                _ctx.fd.region[i].start, _ctx.fd.region[i].end);
    }
#endif
    _close (fd);

    /* loader */   
#ifdef UPGRADE_DEBUG
    printf ("fupgrade: %p\n", fupgrade);
    printf ("eupgrade: %p\n", eupgrade);
    printf ("ftext: %p\n", ftext);
    printf ("etext: %p\n", etext);
    printf ("fdata: %p\n", fdata);
    printf ("edata: %p\n", edata);
    //printf ("upgrade(): %p\n", _upgrade);
#endif
    memcpy (fupgrade, etext + (fupgrade - fdata), eupgrade - fupgrade);
    
    if(action == 1)
        go32(upgrade_dfu);
    else if(action == 2)
        go32(upgrade_spi);
    
    return 0;
}
int check_upgrade_status()
{
#define INFO_DFU_EN             (1<<0)
#define INFO_SPI_EN             (1<<1)
#define INFO_RING_EN            (1<<2)
#define INFO_DFU_DONE           (1<<3)
#define INFO_SPI_DONE           (1<<4)
#define INFO_RING_DONE          (1<<5)
#define INFO_DFU_FAIL           (1<<6)
#define INFO_SPI_FAIL           (1<<7)
#define INFO_RING_FAIL          (1<<8)

#define REG_RTC_BASE            (0xB0800000)
#define REG_UPGRADE_STATUS      (REG_RTC_BASE + 0x11c)

  
    int sta=0;    
    
    sta = readl(REG_UPGRADE_STATUS);    
    // no upgrade info
    if(!sta & INFO_DFU_EN && !sta & INFO_SPI_EN && !sta & INFO_RING_EN) 
        return sta;
    
    if(sta & INFO_DFU_EN) 
    {     
        if(sta & INFO_DFU_FAIL)           
            printf("DFU sd card upgrade last have error\n");                    
    }
    if(sta & INFO_SPI_EN)
    {   
        if(sta & INFO_SPI_FAIL) 
            printf("SPI sd card upgrade last have error\n");                       
    }
    if(sta & INFO_RING_EN)
    {
       if(sta & INFO_RING_FAIL) 
            printf("Ring sd card upgrade last have error\n");               
    }

    if( (sta & INFO_DFU_FAIL)==0  && (sta & INFO_SPI_FAIL)==0 &&  (sta & INFO_RING_FAIL)==0)    
        return sta;
    else    
        return -sta;
    //printf("sta =%08X\n",(unsigned int)sta);     
}

int sd_upgrade_bin_info (unsigned int *ver, unsigned int *build_date, unsigned int *build_num)
{
    struct fat_t *fat = gSYSC.pfat;
    struct fd32 fd[1];
    struct upg_sh upg_info;
    char string_buf[KEY_COUNT][LINE_SIZE];
    char *struct_buf[config_ringtong_num];
    unsigned int layout_tag;
    int rc = 0;

    if (!ver || !build_date || !build_num) {
        ERROR ("invalid arguments\n");
        return -EINVAL;
    }

    if (_ismounted () == 0) {
        rc = _upgrade_mount (fat);
        if (rc < 0) {
            ERROR ("_upgrade_mount(), rc=%d\n", rc);
            return -1;
        }
    }

    rc = _open (fd, CONFIG_FILE, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        _umount();
        fat_check_tw_signature(1);
        return -1;
    }

    _init_upg_sh(&upg_info, string_buf, struct_buf);    
    _get_upg_info(fd, &upg_info);
    _close (fd);

    rc = _open (fd, upg_info.dfu_val, 0, FD_RDONLY);
    if (rc < 0) {
        ERROR ("_open(), rc=%d\n", rc);
        return -2;
    }

    _seek(fd, BIN_LAYOUT_INFO_OFFSET);

    _read(fd, &layout_tag, 4);
    if (layout_tag !=  0x1A1A1010) {
        printf("file layout information tag (0x%08X) error. (correct tag is 0x1A1A1010)\n", layout_tag);
        rc = -ENXIO;
        goto EXIT;
    }

    _read(fd, ver, 4);
    _read(fd, build_date, 4);
    _read(fd, build_num, 4);

EXIT:
    _close (fd);

    return rc;
}
