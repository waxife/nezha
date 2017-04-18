/**
 *  @file   uartbridge.c
 *  @brief: use uart path to read write reigsters
 *  $Id: uartbridge.c,v 1.1 2014/05/21 07:41:22 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 */


#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <config.h>
#include <sys.h>


#define UBCMDBUF_SIZE 600
#define UARTBRIDGE_VER "1.01"
#define REG_BASE 0xA0000000
//#define debug_msg
#define uart_comm

unsigned long AHB_reg=0;
unsigned long AHB_write=0;
unsigned long AHB_read=0;

/*
 * v1.01, 20110512
    1. concern dataport write issue
 * */

int command_check(char* buf, char* pcks)
{
    int len, i;
    char xorval;
    len = strlen(buf);
    xorval = buf[0];

    //if(buf[len-4] != 'C')
    //    return 0;

    for(i = 1; i < len-2; i++) // skip cks(2bytes),
    {
        xorval ^= buf[i];
    }
    *pcks = xorval;
    return len;
}

char* bufptr;
char rettok[32]; //return token

char* strtok(char* buf, char* sepbuf)
{
    char sep;
    static int sidx, toklen;
    sep = sepbuf[0];
    if(buf != NULL)
    {
        bufptr = buf;
        sidx = 0;
        toklen = 0;
    }
    sidx += toklen;
    toklen = 0;

    while(bufptr[sidx+toklen] != 0 && bufptr[sidx+toklen] != sep)
    {
            toklen++;
    }
    memset(rettok, 0, 32);
    memcpy(rettok, bufptr+sidx, toklen);
    toklen++;

    return rettok;
}

int ahb_wproc(int page, int reg, int len, char* cmdbuf, char* data)
{
	//char cmdbuf[UBCMDBUF_SIZE];
    //char data = 0;
    //char* token;
	char* Ack;
	//char Ack[7];
    //unsigned int regadr;
    //int page, reg, len, clen;
    //char cks;
    //int i;

	if (len != 1)
	{
#ifdef debug_msg
		printf("AHB only support LEN 1\n");
#endif
		return 1;
	}

    //memset(cmdbuf, 0, UBCMDBUF_SIZE);
	switch (reg){
		//ahb reg
		case 0x00:	
					AHB_reg = (AHB_reg & 0xFFFFFF00) | (*data<<(reg*8));
					break;
		case 0x01:
					AHB_reg = (AHB_reg & 0xFFFF00FF) | (*data<<(reg*8));
					break;
		case 0x02:
					AHB_reg = (AHB_reg & 0xFF00FFFF) | (*data<<(reg*8));
					break;
		case 0x03:
					AHB_reg = (AHB_reg & 0x00FFFFFF) | (*data<<(reg*8));
					break;
					
		//ahb write	
		case 0x04:
					AHB_write= (AHB_write & 0xFFFFFF00) | (*data<<((reg-4)*8));
					break;
		case 0x05:
					AHB_write= (AHB_write & 0xFFFF00FF) | (*data<<((reg-4)*8));
					break;
		case 0x06:
					AHB_write= (AHB_write & 0xFF00FFFF) | (*data<<((reg-4)*8));
					break;
		case 0x07:
					AHB_write= (AHB_write & 0x00FFFFFF) | (*data<<((reg-4)*8));
					writel(AHB_write,AHB_reg+REG_BASE);
					printf("read back AHB write val = %x\n",readl(AHB_reg+REG_BASE));
					break;
					*data=(AHB_write>>((reg-4)*8)) & 0xFF;
					break;
		//ahb read	
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:	
					break;
	}
#ifdef debug_msg	
	printf("AHB reg = %lx\n",AHB_reg);
	printf("AHB write = %lx\n",AHB_write);
	printf("A\r\n");
	printf("A[%s]\r\n\n\n", cmdbuf);
#endif	
	Ack = "A";

#ifdef uart_comm
	puts(Ack);
	puts(cmdbuf);
#else
	afc_puts(Ack);
	afc_puts(cmdbuf);
#endif

	
    return 0;

}

int ahb_rproc(int page, int reg, int len, char* cmdbuf)
{
	//char cmdbuf[UBCMDBUF_SIZE];
    char data = 0;
    //char* token;
	char* Ack;
	//char Ack[7];
    //unsigned int regadr;
    //int page, reg, len, clen;
    char cks;
    int i=0;

	if (len != 1)
	{
		printf("AHB only support LEN 1\n");
		return 1;
	}
#ifdef debug_msg	
	printf("1.AHB reg = %lx\n",AHB_reg);
	printf("1.AHB read = %lx\n",AHB_read);
#endif
	switch (reg){
		//ahb reg
		case 0x00:	
		case 0x01:
		case 0x02:
		case 0x03:
					data=(AHB_reg>>(reg*8)) & 0xFF;
					break;
		//ahb write	
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
					data=(AHB_write>>((reg-4)*8)) & 0xFF;
					break;
		//ahb read	
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:	
					AHB_read = readl(AHB_reg+REG_BASE);
					data=(AHB_read>>((reg-8)*8)) & 0xFF;
					break;
	}
#ifdef debug_msg	
	printf("2.AHB reg = %lx\n",AHB_reg);
	printf("2.AHB read = %lx\n",AHB_read);
#endif
	sprintf(cmdbuf+i, "%02x,", data);
	i += 3;

    sprintf(cmdbuf+i, "C,11");
	i+=2;
    command_check(cmdbuf, &cks);
    sprintf(cmdbuf+i, "%02x\r\n", cks);
#ifdef debug_msg	
	printf("A[%s]\r\n\n\n", cmdbuf);
#endif

	Ack = "A";
#ifdef uart_comm
	puts(Ack);
	puts(cmdbuf);
#else
	afc_puts(Ack);
	afc_puts(cmdbuf);
#endif
    return 0;

}
int uart_bridge_proc()
{
    char cmdbuf[UBCMDBUF_SIZE];
    char data[280]; // 256 value + C + cks
    char* token;
	char* Ack;
    unsigned int regadr;
    int page, reg, len, clen;
    char cks;
    int i, datasize, bport;	


       	memset(cmdbuf, 0, UBCMDBUF_SIZE);
        gets(cmdbuf);
		clen = strlen(cmdbuf);
        if(cmdbuf[clen-1] == 0x0d && cmdbuf[clen-2] == 0x0a)
        {
            cmdbuf[clen-1] = 0;
            cmdbuf[clen-2] = 0; // remove \r \n
        }
        /*
        else
        {
            printf("E\r\n");
            continue;
        }
        */
        clen = command_check(cmdbuf, &cks);
        if(clen==0)
        {
            return 0;
        }


        //printf("cks in: 0x%x, cks c: 0x%x\n", atoh(&cmdbuf[clen-2]), cks);
        if(atoh(&cmdbuf[clen-2]) != cks)
        {		
			Ack = "E";
			puts(Ack);		
            return 0;
        }
        
        if(cmdbuf[0] == 'E')
            return 0;

        token = strtok(cmdbuf, ",");
        switch(token[0])
        {
            case 'S':
#ifdef debug_msg				
				printf("----->WRITE\n");	
#endif
				page = atoh(strtok(NULL, ","));
                reg  = atoh(strtok(NULL, ","));
                datasize = 0;
                do
                {
                    token = strtok(NULL, ",");
                    if(strcmp(token, "C") == 0)
                        break;
                    data[datasize] = atoh(token);
                    datasize++;
                    if(datasize >= 256)
                        break;

                }while(*token != 0);

				/* AHB bus Write check */
				if (page == 0x1E)
				{
					ahb_wproc(page, reg, datasize, cmdbuf, data);
					break;
				}
				
				/* Not AHB bus Write */
				page = (page - 0x50)/2;
				regadr = 0xb0400000 | (page << 12) | (reg << 4);
				
                // P0_71, P0_94,P0_A1,P0_A4
                bport = 0;
                if(page == 0 && (reg == 0x71 || reg == 0x94 || reg == 0xa1 || reg == 0xa4))
                    bport = 1;

                if(bport == 0)
                {
                    for(i = 0; i < datasize; i++)
                    {
                        writeb(data[i], regadr);
                        reg++;
                        regadr = (regadr & 0xfffff00f) | (reg << 4);
                    }
                }
                else
                {
                    for(i = 0; i < datasize; i++)
                        writeb(data[i], regadr);
                }
				Ack = "A";
				puts(Ack);
				puts(cmdbuf);
				break;
            case 'G':
#ifdef debug_msg				
				printf("----->READ\n");
#endif
				page = atoh(strtok(NULL, ","));
                reg  = atoh(strtok(NULL, ","));
                token = strtok(NULL, ",");
				
                if(strcmp(token, "C") == 0)
                {
                    len = 1;
                }
                else
                {
                    len = atoh(token);
                }
				memset(cmdbuf, 0, UBCMDBUF_SIZE);
				/* AHB BUS Read check */
				if (page == 0x1E)
				{
					
					ahb_rproc(page, reg, len, cmdbuf);
					break;
				}
								
				/*NOT AHB BUS Read process*/
				page = (page - 0x50)/2;
				regadr = 0xb0400000 | (page << 12) | (reg << 4);
				
                for(i = 0; i < len*3; )
                {
                    sprintf(cmdbuf+i, "%02x,", readb(regadr));
                    i += 3;
                    reg++;
                    regadr = (regadr & 0xfffff00f) | ((reg) << 4);
                }

                sprintf(cmdbuf+i, "C,11");
				i+=2;
                command_check(cmdbuf, &cks);
                sprintf(cmdbuf+i, "%02x\r\n", cks);
				
				Ack = "A";
				puts(Ack);
				puts(cmdbuf);
				break;
            case 'E':
				Ack = "A";
				puts(Ack);
#ifdef debug_msg
				printf("End of uartbridge\n");
#endif
				return 0;
            default:				
				Ack = "A";
				puts(Ack);

#ifdef debug_msg				
                printf("Error Command\n");
#endif
				break;
        }

    return 0;
}

void uart_bridge(void)
{
    if (readl(0xB9000028) & 0x20) {
        printf("select AFC TX/RX for UART birdge.\n");
    	writel(readl(0xb9000028) & 0xFFFFFFDF, 0xb9000028);
    }
    
    uart_bridge_proc();
}

