/*----------------------------------------------------------------------
#  @file   i2cget.c
#  @brief  i2cget middleware, copy data to memory through i2c path
#
#  $Id: i2cget.c,v 1.1.1.1 2013/12/18 03:43:38 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.1.1.1 $
#
#  Copyright (c) 2010 Terawins Inc. All rights reserved.
#
#  @date   2010/12/24  yc_shih New file.
----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <errno.h>
#include "i2cget.h"
#include <sys/types.h>

static const char i2c_file_version[] = "v1.00";
//#define uint32_t unsigned long
#define GPIO_BASE 0xb2800000

/***********************************************/
//Spare Register Define		(Total 16 Bytes)
/***********************************************/
/*
[0-3]	DataBuf0			
[4] 		Command0
			[8]    1: data, 0: cmd
			[7:3] Command
			[1:0] Tag
[5-8]	DataBuf1
[9]		Command1
[10~13]	DataReturn
			[31:0]
[14] 	Status
			[2] RESET
			[1] ERROR
			[0] DONE
[15]  	Cmd Echo
			[8]    data/cmd
			[7:3] Command
			[1:0] Tag
*/
/*********************************************/
/*
page3: 56, 00~0F 
#define SPARE_REG_BASE		0xb0403000
#define SPARE_REG_BASE		0xb04030F0

1. 50, 0xE2:0
2. 56, 0x10:1
3. TWTOOL, sync shadow reg don't choose
*/
/********************************************/

#define SPARE_REG_BASE		0xb0403000
#define SPARE_REG00			0xb0403000
#define SPARE_REG01			0xb0403010
#define SPARE_REG02			0xb0403020
#define SPARE_REG03			0xb0403030
#define SPARE_REG04			0xb0403040
#define SPARE_REG05			0xb0403050
#define SPARE_REG06			0xb0403060
#define SPARE_REG07			0xb0403070
#define SPARE_REG08			0xb0403080
#define SPARE_REG09			0xb0403090
#define SPARE_REG10			0xb04030a0
#define SPARE_REG11			0xb04030b0
#define SPARE_REG12			0xb04030c0
#define SPARE_REG13			0xb04030d0
#define SPARE_REG14			0xb04030e0
#define SPARE_REG15			0xb04030f0

//#define DFUINS_DATABUF0		SPARE_REG_BASE
#define DFUINS_DATABUF00	SPARE_REG00
#define DFUINS_DATABUF01	SPARE_REG01
#define DFUINS_DATABUF02	SPARE_REG02
#define DFUINS_DATABUF03	SPARE_REG03
#define DFUINS_CMD0		    SPARE_REG04
#define DFUINS_DATABUF10	SPARE_REG05
#define DFUINS_DATABUF11	SPARE_REG06
#define DFUINS_DATABUF12	SPARE_REG07
#define DFUINS_DATABUF13	SPARE_REG08
#define DFUINS_CMD1		    SPARE_REG09
#define DFUINS_DATABUFR0	SPARE_REG10
#define DFUINS_DATABUFR1	SPARE_REG11
#define DFUINS_DATABUFR2	SPARE_REG12
#define DFUINS_DATABUFR3	SPARE_REG13
#define DFUINS_STATUS		SPARE_REG14
#define DFUINS_CMDECHO		SPARE_REG15

// Status
#define SV_DONE				(1<<0)
#define SV_ERROR			(1<<1)
#define SV_RESET			(1<<2)
#define SV_TIMEOUT          (1<<3)
#define SV_RESEND           (1<<4)
//Command Set
#define CMD_RESET 			0x00
#define CHK_DFU 			0x01
#define CHK_FINISH 			0x02
#define CHK_CRC				0x03
#define SET_PG_BLK			0x04
#define SET_PG_LEN			0x05
#define SET_REG_STATUS		0x06
#define ERASE_BLK			0x07
#define PUT_DATA			0x08
#define GET_ID				0x09
#define GET_REG_STATUS		0x0a
#define WRITE_FLASH			0x0b
#define START_DATA_SESSION	0x0c
#define STOP_DFU_INSTALL	0x0d
#define GET_DEVID			0x0e
#define CMD_RESEND          0x0f

#define DATAPACKET			(1<<7)
#define CMD_MASK			0x7C
#define CMD_SHIFT			2
#define TAG_MASK			0x03
#define TAG_MOD				4
#define DFU_BUF_SIZE		16384


/* Debug.h */
#define DBG_LEVEL   1
#define DBG_PRINT   printf
#define DBG_ABORT   abort
#define assert(expr) \
    ({ if (! (expr)) { DBG_PRINT("assert failed: %s on %s %d\n", #expr, __FILE__, __LINE__); DBG_ABORT(); } })
#define dbg(lvl, fmt, arg...)  \
    ({if (lvl <= DBG_LEVEL) DBG_PRINT("%s:%d " fmt, __FILE__, __LINE__, ##arg);})

#define DFUINS_STATE_INIT 	0
#define DFUINS_STATE_CMD 	1
#define DFUINS_STATE_DATA 	2


typedef struct _dfu_ins{
	int dfu_size;
	int dfu_remain;
	unsigned int crc_flash;
	unsigned int crc_ap;
	//int start_pu;	
	//int end_pu;
	unsigned char buf[DFU_BUF_SIZE];	
	unsigned int nor_flash_id;
	unsigned int register_status;
	int state;
	int cur_buf_idx;  // current buffer index
	int cur_addr_ptr; // current flash address
	int cur_write_size;
	int d0_first;
}dfu_ins;
//dfu_ins dfuins;


uint32_t exclusiveor32(uint32_t exor32, uint32_t *s, unsigned int len)
{
    unsigned int i;
  
    for (i = 0;  i < len;  i ++) {
        exor32 = exor32 ^ s[i];
    }
    return exor32;
}


#define _usleep(delaytime) do{volatile int usleep_count; for (usleep_count = 0; usleep_count < delaytime*100; usleep_count++);}while(0)

static int tor = 0;
//FILE* fwo = NULL;
char* fwo = NULL;
char* fileaddr = NULL;
int test = 0;

#define bool int 
#define true 1
#define TRUE 1
#define false 0
#define FALSE 0
#define REPEAT_CHK 6
#define GPIO_DBG 0

int i2cget(char* filename)
{
	uint32_t db0, db1, db2, db3, db4, db5, db6, db7; //data byte 0~3
	uint32_t rdid, wren, chpers, rdsr, wr, wrsr;
	unsigned char status = 0;
	unsigned long retval = 0;
	//bool needCloseFile = FALSE;
	//int rc;
	int chktimes = 0;
    int pd_cnt = 0; //put data count
    int crc_err_cnt = 0;
	unsigned char cmdreg, cmdreg0, cmdreg1;
    dfu_ins dfuins;
	rdid = 0; wren = 0; chpers = 0; rdsr = 0; wr = 0; wrsr = 0;
	
	dbg(2, "dfu install \n");

	memset(&dfuins, 0, sizeof(dfuins));
	dfuins.d0_first = 1;

#if GPIO_DBG
        //gpio init setting
	//#define GPIO_PINMUX 0xb2800050
	writel(0x288000,  0xb2800050);//pin mux
	writel(0x8060001, 0xb2800004);//GPIO enable
#endif

	int tag = 0;
	int tag0 = 0;
	int tag1 = 0;
	unsigned char cmdreg0_chk = 0;
	unsigned char cmdreg1_chk = 0;
	int dataidx = 1;
	unsigned char command, command0, command1;
	command = 0;
	cmdreg = 0;
	db0 = 0; db1 = 0; db2 = 0; db3 = 0; db4 = 0; db5 = 0; db6 = 0; db7 = 0;
	while(1)
	{	
		cmdreg0 = readb(DFUINS_CMD0);
		cmdreg1 = readb(DFUINS_CMD1);
		tag0 = (cmdreg0 & TAG_MASK);
		tag1 = (cmdreg1 & TAG_MASK);
		command0 = (cmdreg0 & CMD_MASK) >> CMD_SHIFT;
		command1 = (cmdreg1 & CMD_MASK) >> CMD_SHIFT;
		
		if(cmdreg0 == 0xff && cmdreg1 == 0xff)//CMD_IDLE
		{
			usleep(10);
			continue;
		}

		if(tor>5000000)
		{
			//dbg(2, 
            printf("tor: %d, tag: %d, tag0: %d, tag1: %d, regcmd: 0x%x, regcmd0: 0x%x, regcmd1: 0x%x\n",tor, tag, tag0, tag1, cmdreg, cmdreg0, cmdreg1);
            status = SV_TIMEOUT;
            retval = -1;
			goto EXIT;
		}
		// if state is not data state, command always from data1
		if(dfuins.state != DFUINS_STATE_DATA)
		{
			if((tag+1)%TAG_MOD == tag1)
			{
				cmdreg1_chk = readb(DFUINS_CMD1);
				if(cmdreg1 != cmdreg1_chk)
				{
					chktimes = 0;
					continue;
				}
				_usleep(1);
				chktimes ++;
				if(chktimes < REPEAT_CHK)
					continue;
				chktimes = 0;

				cmdreg = cmdreg1;
				dataidx = 1;
				tag = tag1;
				command = command1;
				db0 = readb(DFUINS_DATABUF10);
				db1 = readb(DFUINS_DATABUF11);
				db2 = readb(DFUINS_DATABUF12);
				db3 = readb(DFUINS_DATABUF13);
				dfuins.d0_first = 1;
				tor = 0;
			}else{
				continue;
			}
		}
		else{
			if(dataidx == 0)
			{

				if((tag+1)%TAG_MOD == tag1)
				{
                    tor = 0;
					cmdreg1_chk = readb(DFUINS_CMD1);
					if(cmdreg1 != cmdreg1_chk)
					{
						chktimes = 0;
						continue;
					}
					_usleep(1);
					chktimes ++;
                    if(chktimes < REPEAT_CHK)
                        continue;
                    chktimes = 0;
					cmdreg = cmdreg1;					
					command = command1;					
					tag = tag1;
					dataidx = 1;
					#if GPIO_DBG //debug using
					writel(0x40000, 0xb2800008);//GPIO SET
					#endif
					db0 = readb(DFUINS_DATABUF00);
					db1 = readb(DFUINS_DATABUF01);
					db2 = readb(DFUINS_DATABUF02);
					db3 = readb(DFUINS_DATABUF03);
                    db4 = readb(DFUINS_DATABUF10);
                    db5 = readb(DFUINS_DATABUF11);
                    db6 = readb(DFUINS_DATABUF12);
                    db7 = readb(DFUINS_DATABUF13);
					#if GPIO_DBG // debug using
					writel(0x40000, 0xb280000c);
					#endif
					//printf("1 cmdreg = 0x%x, tag = %d, command = 0x%x\n", cmdreg, tag, command);
				}
				else
				{
					tor++;
					//printf("2 cmdreg = 0x%x, tag = %d, command = 0x%x\n", cmdreg, tag, command);
					continue;
				}
			}
			else
			{
				if((tag+1)%TAG_MOD == tag0)
				{
                    tor = 0;
					cmdreg0_chk = readb(DFUINS_CMD0);
					if(cmdreg0 != cmdreg0_chk)
					{
						chktimes = 0;
						continue;
					}
					_usleep(1);
					chktimes ++;
                    if(chktimes < REPEAT_CHK)
                        continue;
                    chktimes = 0;

                    cmdreg = cmdreg0;
                    command = command0;
                    tag = tag0;
                    dataidx = 0;

                    if(command == CHK_CRC || command == CHK_FINISH || command == STOP_DFU_INSTALL || command == CMD_RESEND)
                    {
                        #if GPIO_DBG //debug using
                        writel(0x40000, 0xb2800008);
                        #endif
                        db0 = readb(DFUINS_DATABUF00);
                        db1 = readb(DFUINS_DATABUF01);
                        db2 = readb(DFUINS_DATABUF02);
                        db3 = readb(DFUINS_DATABUF03);
                        #if GPIO_DBG //debig isomg
                        writel(0x40000, 0xb280000c);
                        #endif
                    }
                    else
                    {
                        continue;
                    }

					//printf("3 cmdreg = 0x%x, tag = %d, command = 0x%x\n", cmdreg, tag, command);
				}else{
					tor++;
					//printf("4 cmdreg = 0x%x, tag = %d, command = 0x%x\n", cmdreg, tag, command);
					continue;
				}
			}
		}
		
		switch(command)
		{
			case CMD_RESET:
				{
					#if 0
					memset(&dfuins, 0, sizeof(dfuins));
					dfuins.state = DFUINS_STATE_INIT;
					tag = 0;
					#endif
					status = SV_DONE;
					dbg(2, "CHK_RESET, SV_DONE\n");
				}
				break;
			case CHK_DFU:
				{
					/*
						how to check dfu mode?? 
						check tboot header 55aa
					*/
					rdid = db0; 
					wren = db1;
					chpers = db2; 
					wr = db3;
					//if(1)
					status = SV_DONE; //0x11
					//else
					//status = SV_ERROR;

					dbg(2, "CHK_DFU status = %d, rdid = 0x%02x, wren = 0x%02x, chpers = 0x%02x, write = 0x%02x\n", 
						status, (unsigned char)rdid, (unsigned char)wren, (unsigned char)chpers, (unsigned char)wr);
				}
				break;			
			case CHK_FINISH:
				{
					if(dfuins.cur_write_size == dfuins.dfu_size)
					{
						dfuins.state = DFUINS_STATE_CMD;
						status = SV_DONE;
					}
					else
					{
						//dbg(2, 
                        printf("CHK_FINISH ERROR write size = 0x%x, dfu_size = 0x%x, remain = 0x%x\n", dfuins.cur_write_size, dfuins.dfu_size, dfuins.dfu_remain);
						status = SV_ERROR;
						retval = -28; //-ECOMM;
						goto EXIT;
					}
					dbg(2, "CHK_FINISH status = 0x%x\n", status);
				}
				break;			
			case CHK_CRC:			  
				{
					// write to file -->
					{
						//WREN, WRITE
                        #ifdef I2CFILE
						rc = fwrite(dfuins.buf, 1, dfuins.cur_buf_idx, fwo);
						dbg(2, "CHK_CRC, write buffer size = 0x%x\n", dfuins.cur_buf_idx);
						if(rc != dfuins.cur_buf_idx)
						{
							status = SV_ERROR;
							retval = -ENOSPC;
							goto EXIT;
						}
                        #else

                        memcpy(fileaddr + dfuins.cur_write_size, dfuins.buf, dfuins.cur_buf_idx);
                        //if fail
                        #endif

						dfuins.cur_addr_ptr += 1024;
					}
					dfuins.d0_first = (dfuins.d0_first + 1)%2;
					dfuins.cur_write_size += dfuins.cur_buf_idx;
					dbg(2, "HK_CRC, current write = %d\n", dfuins.cur_write_size);
					// write to file <--

					if(dataidx == 0)
						dfuins.crc_ap = (db3 << 24 | db2 << 16 | db1 << 8 | db0);
					else
						dfuins.crc_ap = (db7 << 24 | db6 << 16 | db5 << 8 | db4);
					// 2. calculate crc
					dfuins.crc_flash = 0;
					dfuins.crc_flash = exclusiveor32(dfuins.crc_flash, (uint32_t*)dfuins.buf, DFU_BUF_SIZE/4);
					memset(dfuins.buf, 0, DFU_BUF_SIZE);
					if( ((dfuins.cur_buf_idx != DFU_BUF_SIZE) && 
					     (dfuins.dfu_remain != 0)) ||
                         (dfuins.crc_flash != dfuins.crc_ap ) )
					{
						//dbg(2, 
                            printf("CRC ERROR: pd_cnt = %d, crc_flash = 0x%08x, crc_ap = 0x%08x, addr = 0x%x, idx = 0x%x, size = 0x%x\n", 
                            pd_cnt,
                            dfuins.crc_flash, 
							dfuins.crc_ap, 
							dfuins.cur_addr_ptr, 
							dfuins.cur_buf_idx, 
							dfuins.cur_write_size);
                            pd_cnt = 0;

                            crc_err_cnt++;
                            if(crc_err_cnt < 10)
                            {
                                //int rt1;
                                dfuins.cur_write_size -= dfuins.cur_buf_idx;
                                dfuins.dfu_remain += dfuins.cur_buf_idx;
                                dfuins.cur_buf_idx = 0;

                                #ifdef I2CFILE
                                rt1 = fseek(fwo, dfuins.cur_write_size, SEEK_SET);
                                if(rt1 != 0)
                                {
                                    status = SV_ERROR;
                                    retval = -ENOSPC;
                                    printf("CHK_CRC: fseek fail at 0x%x\n", dfuins.cur_write_size);
                                    goto EXIT;
                                }
                                #endif


                                status = SV_RESEND;
                                printf("CRC_ERR: resend cur_buf_idx = 0x%x, write size = 0x%x, remain = 0x%x\n", dfuins.cur_buf_idx, dfuins.cur_write_size, dfuins.dfu_remain);
                            }
                            else
                            {
                                dfuins.cur_buf_idx = 0;
						        status = SV_ERROR;
						        retval = -28; //-ECOMM;
                                goto EXIT;
                            }
					}
					else						
					{
                        dfuins.cur_buf_idx = 0;
                        crc_err_cnt = 0;
						dbg(2, "CRC CORRECT pd_cnt = %d, crc_flash = 0x%08x, crc_ap = 0x%08x, addr = 0x%x, idx = 0x%x, size = 0x%x k\n", 
                            pd_cnt,
							dfuins.crc_flash, 
							dfuins.crc_ap, 
							dfuins.cur_addr_ptr, 
							dfuins.cur_buf_idx, 
							dfuins.cur_write_size);
                        printf("Write Data OK: write size = 0x%x, remain = 0x%x\n",  dfuins.cur_write_size, dfuins.dfu_remain);
                        pd_cnt = 0;
						status = SV_DONE;
					}
				}
				break;
            case CMD_RESEND:
                {
                    //int rtrsd = 0;
                    int cur_buf_idx = 0;
                    if(dataidx == 0)
                        cur_buf_idx = (db3 << 24 | db2 << 16 | db1 << 8 | db0);
                    else
                        cur_buf_idx = (db7 << 24 | db6 << 16 | db5 << 8 | db4);

                    dfuins.d0_first = (dfuins.d0_first + 1)%2;
                    dfuins.cur_write_size -= cur_buf_idx;
                    dfuins.dfu_remain += cur_buf_idx;
                    dfuins.cur_buf_idx = 0;

                    #ifdef I2CFILE
                    rtrsd = fseek(fwo, dfuins.cur_write_size, SEEK_SET);

                    if(rtrsd != 0)
                    {
                       status = SV_ERROR;
                       retval = -ENOSPC;
                       printf("CMD_RESEND: fseek fail at 0x%x\n", dfuins.cur_write_size);
                       goto EXIT;
                    }
                    #endif
                    status = SV_DONE;
                    printf("CMD_RESEND: resend cur_buf_idx = 0x%x, write size = 0x%x, remain = 0x%x\n", dfuins.cur_buf_idx, dfuins.cur_write_size, dfuins.dfu_remain);
                }
                break;
			case SET_PG_LEN:
				{
					dfuins.dfu_size = (db3 << 24 | db2 << 16 | db1 << 8 | db0);
					dfuins.cur_write_size = 0;
					dfuins.dfu_remain = dfuins.dfu_size;
					status = SV_DONE;
					dbg(2, "dfu_size = %d\n", dfuins.dfu_size);
					dbg(2, "SET_PG_LEN: Not Enough Space\n");
				}
				break;
			case SET_REG_STATUS:
				{
					dbg(2, "SET_REG_STATUS\n");
					status = SV_DONE;
					dbg(2, "SET_REG_STATUS status = 0x%x\n", status);
				}
				break;
			case ERASE_BLK:
				{
					/*
						Erase Flash block 
						from start to end
					*/
                    printf("i2cget filename = %s\n", filename);
                    #ifdef I2CFILE
					fwo = fopen(filename, "w");
                    #else

#if 1
                    test = atoi("100");
                    printf("atoi 100 = %d\n", test);
                    test = atoh("100");
                    printf("atoh 100 = %d\n", test);
                    test = atoi("0x100");
                    printf("atoi 0x100 = %d\n", test);
                    test = atoh("0x100");
                    printf("atoh 0x100 = %d\n", test);

                    test = atoi("0400000");
                    printf("atoi 0400000 = %d\n", test);
                    test = atoh("0400000");
                    printf("atoh 0400000 = %d\n", test);
                    test = atoi("0x0400000");
                    printf("atoi 0x0400000 = %d\n", test);
                    test = atoh("0x0400000");
                    printf("atoh 0x0400000 = %d\n", test);
#endif

                    if(filename[0] == '0' && (filename[1] == 'x' || filename[1] == 'X'))
                        fileaddr = (char*) atoh((filename+2));
                    else
                        fileaddr = 0;

                    #endif

                    printf("fwo = 0x%p, fileaddr = 0x%p, test = %d\n", fwo, fileaddr, test);
                    //fileaddr = 0x80200000;
					if(fwo == NULL && fileaddr == NULL)
					{
						retval = -ENOSPC; //create file fail;
						status = SV_ERROR;
                        if(fwo)
						dbg(2, "ERASE_BLK: Create %s Fail\n", filename);

                        if(fileaddr)
                        dbg(2, "ERASE_BLK: Address %p error\n", fileaddr);

						goto EXIT;
					}
					else
					{
						//char testb = 1;
                        //int rt;

                        #ifdef I2CFILE
                        rt = fseek(fwo, dfuins.dfu_size - 1, SEEK_SET);
                        if(rt != 0)
                        {
                            status = SV_ERROR;
                            retval = -ENOSPC;
                            dbg(2, "ERASE_BLK: Not Enough Space\n");
                            goto EXIT;
                        }
                        rt = fwrite(&testb, 1, 1, fwo);
                        if(rt != 1)
                        {
                            status = SV_ERROR;
                            retval = -ENOSPC;
                            goto EXIT;
                        }
						fseek(fwo, 0, SEEK_SET);
                        needCloseFile = tru;
                        #endif

						status = SV_DONE;
                        if(fwo)
						dbg(2, "ERASE_BLK: Create %s success\n", filename);

                        if(fileaddr)
                        dbg(2, "ERASE_BLK: fileaddr %p\n", fileaddr);

					}
				}
				break;
			case PUT_DATA:
				{
					/* check little endian or bigendian ??*/
					status = SV_DONE;
                    pd_cnt ++;
					if(dfuins.cur_buf_idx + 8 <= DFU_BUF_SIZE)
					{
						if(dfuins.d0_first)
						{
							dfuins.buf[dfuins.cur_buf_idx]   = db0;
							dfuins.buf[dfuins.cur_buf_idx+1] = db1;
							dfuins.buf[dfuins.cur_buf_idx+2] = db2;
							dfuins.buf[dfuins.cur_buf_idx+3] = db3;
							dfuins.buf[dfuins.cur_buf_idx+4] = db4;
                            dfuins.buf[dfuins.cur_buf_idx+5] = db5;
                            dfuins.buf[dfuins.cur_buf_idx+6] = db6;
                            dfuins.buf[dfuins.cur_buf_idx+7] = db7;
						}
						else
						{
							dfuins.buf[dfuins.cur_buf_idx]   = db4;
                            dfuins.buf[dfuins.cur_buf_idx+1] = db5;
                            dfuins.buf[dfuins.cur_buf_idx+2] = db6;
                            dfuins.buf[dfuins.cur_buf_idx+3] = db7;
                            dfuins.buf[dfuins.cur_buf_idx+4] = db0;
                            dfuins.buf[dfuins.cur_buf_idx+5] = db1;
                            dfuins.buf[dfuins.cur_buf_idx+6] = db2;
                            dfuins.buf[dfuins.cur_buf_idx+7] = db3;
						}

						if(dfuins.dfu_remain >= 8)
                        {
							dfuins.cur_buf_idx += 8;
                            dfuins.dfu_remain -= 8;
                        }
						else
                        {
							dfuins.cur_buf_idx += dfuins.dfu_remain;
                            dfuins.dfu_remain = 0;
                        }

						status = SV_DONE;
					}
					else
					{
						dbg(2, "DFU Buffer Has no Space to save the data\n");
						status = SV_ERROR;
						retval = -ENOBUFS;
						goto EXIT;
					}
				}	
				break;
			case GET_ID:
				{
					status = SV_DONE;
					dbg(2, "GET_ID status = 0x%x, rdsr = 0x%02x, wrsr = 0x%02x\n", status, (unsigned char)rdsr, (unsigned char)wrsr);
				}
				break;
			case GET_DEVID:
				{ //only using to distinguish middleware in Linux or irom
					db0 = 0x11;
                    db1 = 0x22;
                    db2 = 0x33;
                    db3 = 0x44;
                    writeb(db0, DFUINS_DATABUFR0);
                    writeb(db1, DFUINS_DATABUFR1);
                    writeb(db2, DFUINS_DATABUFR2);
                    writeb(db3, DFUINS_DATABUFR3);
					status = SV_DONE;
                    dbg(2, "GET_DEVID status = 0x%x\n", status);
				}
				break;
			case GET_REG_STATUS:
				{
					status = SV_DONE;
					dbg(2, "GET_REG_STATUS status = 0x%d\n", status);
 				}
				break;
			case START_DATA_SESSION:
				{
					dbg(2, "START DATA SESSION\n");
					dfuins.state = DFUINS_STATE_DATA;
					status = SV_DONE;
				}
				break;
			case STOP_DFU_INSTALL:
				dbg(2, "STOP DFU Install\n");
				writeb(SV_DONE, DFUINS_STATUS);
				writeb(cmdreg, DFUINS_CMDECHO);
				retval = 0;
				goto EXIT;
				break;
			default:
				// show message
				dbg(2, "Wrong Command 0x%x Setting\n", command);
				status = SV_DONE;
				dbg(2, "default status = 0x%x\n", status);
				break;
		}

        unsigned int status_rd, cmdreg_rd;
		writeb(status, DFUINS_STATUS);
        status_rd = readb(DFUINS_STATUS);
		writeb(cmdreg, DFUINS_CMDECHO);
        cmdreg_rd = readb(DFUINS_CMDECHO);
        
        while(status_rd != status || cmdreg_rd != cmdreg)
        {
            printf("!!!!!!!!!!! sts = 0x%x, sts_rd = 0x%x, cmd = 0x%x, cmd_rd = 0x%x\n", status, status_rd, cmdreg, cmdreg_rd);
            writeb(status, DFUINS_STATUS);
            status_rd = readb(DFUINS_STATUS);
            writeb(cmdreg, DFUINS_CMDECHO);
            cmdreg_rd = readb(DFUINS_CMDECHO);
        }
	}
    writeb(0xff, DFUINS_CMD0);
    writeb(0xff, DFUINS_CMD1);

#ifdef I2CFILE
    if(needCloseFile)
    {
        printf("close file 1\n");
        fclose(fwo);
    }
#endif

  return 0;

EXIT:

    writeb(status, DFUINS_STATUS);
    writeb(cmdreg, DFUINS_CMDECHO);
    writeb(0xff, DFUINS_CMD0);
    writeb(0xff, DFUINS_CMD1);

#ifdef I2CFILE
    if(needCloseFile)
    {
        printf("close file 2\n");
        fclose(fwo);
    }
#endif

    dbg(2, "sh_dfu_install exit\n");
    return retval;
}

