/**
 *  @file   sh_sd.c
 *  @brief  nand test command
 *  $Id: sh_sd.c,v 1.15 2015/08/10 11:10:05 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.15 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <fat32.h>
#include <debug.h>
#include <stopwatch.h>
#include <sd.h>
#include <gpio.h>
#include <serial.h>
#include <heap.h>

command_init (sh_sdii, "sdii", "sdii");
command_init (sh_sdr, "sdr", "sdr <addr> <sector>");
command_init (sh_sdw, "sdw", "sdw <addr> <sector>");
command_init (sh_sdw2, "sdw2", "sdw <addr> <sector>");
command_init (sh_sdrwc, "sdrwc", "sdrwc <sector> <sector>");
command_init (sh_sdd, "sdd", "sdd <dbglevel>");
command_init (sh_sd_seqw, "sd_seqw", "sd_seqw <sector> <iosize> <size>");
command_init (sh_sd_nseqw, "sd_nseqw", "sd_nseqw <sector> <iosize> <size>");
command_init (sh_sd_status, "sd_status", "sd_status");
//command_init (sh_sdinfo, "sdinfo", "sdinfo");
//command_init (sh_sd1bit, "sd1bit", "sd1bit [0 | 1]");
command_init (sh_sd_evaluate, "sd_evaluate", "sd_evaluate [<seek offset>]");
command_init (sh_sd_speed, "sd_speed", "sd_speed");
command_init (sh_sd_speed_check, "sd_speed_check", "sd_speed_check");
command_init (sh_sd_dump, "sd_dump", "sd_dump -s <sector> -c <cluster> [<count>]");

extern int sdr_write(void *handle, int startsector, int sectorcount, char *buf);

#if 0
static int sh_sdinfo(int argc, char **argv, void **result, void *sys_ctx)
{
    extern int sd_info(void);

    sd_info();
    return 0;
}


static int sh_sd1bit(int argc, char **argv, void **result, void *sys_ctx)
{
    extern int sd_1bit(int on);

    if (argc < 2)
        sd_1bit(0);
    else
        sd_1bit(*argv[1]);

    return 0;
}
#endif

static int sh_sdd(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;
    int sddbg = 3;

    if (argc < 2)
        goto EXIT;
    
    rc = get_val(argv[1], &sddbg);
    if (rc < 0)
        goto EXIT;

EXIT:
    printf("dbg = %d\n", sddbg);

    return 0;
}

#define     MAXTIMEOUT      0x1fffffff


#define sectorsize			512

static int sh_sdii(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
    int size;
	
    sd_op = fat->op;
    sd = fat->hcard;    
    rc = sd_op->init_card(sd);
    if (rc < 0) {
        dbg(2, "failed invoke init_card %d\n", rc);
    }

    size = sd_op->device_size (sd);

    printf ("SD size: %u KB (%u MB)\n", size*512/1024, size*512/1024/1024);

    return 0;
}

static int sh_sdrwc(int argc, char **argv, void **result, void *sys_ctx)
{
    int ssec, esec, sec;
    unsigned char data[2048];
    int rc = 0;
    int i;
    int sddbg = 3;
    int mysddbg = 3;//sddbg;
    int werr = 0;
    int rerr = 0;
    int derr = 0;
    unsigned long pattern, p;
    void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;

#if 0
    dbg(0, "sd_op %p hcard %p \n", sd_op, sd);
    rc = sd_op->init_card(sd);
    if (rc < 0) {
        dbg(2, "failed invoke init_card %d\n", rc);
    }

    if (!sd_op) {
        printf("Command 'sdii' before the other sd operation command\n");
        goto EXIT;
    }
#endif
    if (argc < 2)
        goto EXIT;
    
    rc = get_val(argv[1], &ssec);
    if (rc < 0)
        goto EXIT;

    if (argc > 2) {
        rc = get_val(argv[2], &esec);
        if (rc < 0)
            goto EXIT;
    } else {
        esec = ssec;
    }

    sddbg = 0;

    
    for (sec = ssec; sec <= esec; sec++) {    	
        p = pattern = rand();
        for (i = 0; i < sectorsize; i+=4, p++) {
            memcpy(&data[i], &p, sizeof(int));
        }
		
        rc = sd_op->write_sector(sd, sec, 1, sectorsize, (char *)data);
        if (rc < 0) {
            printf("Err sd_write_sector %d sec %d\n", rc, sec);
            werr++;
        }
        
        memset(data, 0, sectorsize);

        rc = sd_op->read_sector(sd, sec, 1, sectorsize, (char *)data);
        if (rc < 0) {
            printf("Err sd_read_sector %d sec %d\n", rc, sec);
            rerr++;
        }
        
        p = pattern;
        for (i = 0; i < sectorsize;  i+=4, p++) {
            if (memcmp(&data[i], &p, sizeof(int)) != 0) {
                printf("Err data sec %d-%d %08lx (%08lx)\n", 
                        sec, i, *(unsigned long *)&data[i], p);
                derr++;
                break;
            }       
        }

        printf("sec %d done (werr %d rerr %d derr %d)\r", sec, werr, rerr, derr);
    }

    printf("\nsd write read test sec %d - %d \n", ssec, esec);

    sddbg = mysddbg;
    return 0;

EXIT:
    sddbg = mysddbg;
    printf("Error %d\n", rc);
    return -1;

}


static int sh_sdr(int argc, char **argv, void **result, void *sys_ctx)
{
    char *addr;
    int sec;
    char *data;
    int rc = 0;
    int nsec = 1;
    void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;


    if (!sd_op) {
        printf("Command 'sdii' before the other sd operation command\n");
        goto EXIT;
    }


    if (argc < 3)
        goto EXIT;
    
    rc = get_addr(argv[1], &addr);
    if (rc < 0)
        goto EXIT;

    rc = get_val(argv[2], &sec);
    if (rc < 0)
        goto EXIT;

    if (argc > 3) {
        rc = get_val(argv[3], &nsec);
        if (rc < 0)
            nsec = 1;
    }

    if (nsec <= 0)
        nsec = 1;


    printf("read sd sector %d+%d to %p\n", sec, nsec, addr);

    data = addr;
    memset(data, 0xc0, sectorsize);

    rc = sd_op->read_sector(sd, sec, 1, sectorsize, data);
    if (rc < 0) {
        printf("Failed to invoke read_sector %d\n", rc);
        goto EXIT;
    }

    return 0;

EXIT:
    printf("Error %d\n", rc);
    return -1;

}


static int sh_sdw(int argc, char **argv, void **result, void *sys_ctx)
{
    char *addr;
    int sec;
    char *data;
    int rc = 0;
    void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;


    if (!sd_op) {
        printf("Command 'sdii' before the other sd operation command\n");
        goto EXIT;
    }

    if (argc < 3)
        goto EXIT;
    
    rc = get_addr(argv[1], &addr);
    if (rc < 0)
        goto EXIT;

    rc = get_val(argv[2], &sec);
    if (rc < 0)
        goto EXIT;


    data = addr;
    rc =  sd_op->write_sector(sd, sec, 1, sectorsize, data);
    if (rc < 0) {
        printf("Failed to invoke sd_write_sector %d\n", rc);
        goto EXIT;
    }

    return 0;

EXIT:
    printf("Error %d\n", rc);
    return -1;

}


static int sh_sdw2(int argc, char **argv, void **result, void *sys_ctx)
{
    char *addr;
    int sec, i;
    char *data, buf[512*5];
    int rc = 0;
    void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;

    if (!sd_op) {
        printf("Command 'sdii' before the other sd operation command\n");
        goto EXIT;
    }

    if (argc < 3)
        goto EXIT;
    
    rc = get_addr(argv[1], &addr);
    if (rc < 0)
        goto EXIT;

    rc = get_val(argv[2], &sec);
    if (rc < 0)
        goto EXIT;

    data = addr;
    printf("write ... \n");
    rc =  sd_op->write_sector(sd, sec, 5, sectorsize, data);
    if (rc < 0) {
        printf("Failed to invoke sd_write_sector %d\n", rc);
        goto EXIT;
    }
    
    printf("read ... \n");
    rc =  sd_op->read_sector(sd, sec, 5, sectorsize, buf);
    if (rc < 0) {
        printf("Failed to invoke sd_write_sector %d\n", rc);
        goto EXIT;
    }
    
    printf("compare ... \n");
    for (i = 0; i < 512*5; i++) {
    	if (buf[i] != *(data + i)) {
    		printf("ERROR: idx %d ecpect %02x actual %02x \n", i, *(data+i), buf[i]);
    		goto EXIT;
    	}	
    }
    

    return 0;

EXIT:
    printf("Error %d\n", rc);
    return -1;

}

static int sdwraw_test(void *sd, int sec, char *d, int sec_count)
{
	int rc = -1;
	
	//dbg(0, ">>> sec %d \n", sec);
	while(rc != 0) {
		rc = sdr_write(sd, sec, sec_count, d);
		if (rc < 0) {
	        printf("Failed to invoke sd_write_sector %d\n", rc);
	        goto EXIT;
	    } else if (rc > 0) {
	    	//printf("<%d> SD card or controller busy %d!\n", sec, rc);
	    	continue;
	    }
	}
	
	return 0;
EXIT:
	return -1;
}

static int sh_sd_seqw(int argc, char **argv, void **result, void *sys_ctx)
{
    int sec, second, sc;
    int rc = -1;
    uint32_t start; // sector number of sd card
    int i, j, cnt, size, iosize;    
	char buf[1024];
	int	dbuf[10];
	uint32_t ticks, total_tick;
	void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;

    rc = sd_op->init_card(sd);
    if (rc < 0) {
        dbg(2, "failed invoke init_card %d\n", rc);
        goto EXIT;
    }	
	
    if (!sd_op) {
        printf("fail to get sd_op \n");
        goto EXIT;
    }

    if (argc < 4)
        goto EXIT;

    rc = get_val(argv[1], &sec);
    if (rc < 0)
        goto EXIT;

	rc = get_val(argv[2], &iosize);
    if (rc < 0)
        goto EXIT;

    rc = get_val(argv[3], &size);
    if (rc < 0)
        goto EXIT;

    for (i = 0; i < 1024; i++) {
    	buf[i] = i;
    }    
    
    /* write data to sd card */
	/* sequence write */
	sc = iosize / 512;   /* sc: sector count */
	cnt = size/(10*iosize);
	total_tick = 0;	
	dbg(0, "sec %d-%d iosize %dKB io_cnt %d \n", sec, sec+size/512, iosize/1024, size/iosize);
	for (i = 0; i < cnt; i++) {
		/* sec 0 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 0, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[0] = ticks;		
		ticks = (ticks >> 3);
		total_tick += ticks;
	
		/* sec 1 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 1*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;		
		dbuf[1] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 2 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 2*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[2] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 3 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 3*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[3] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 4 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 4*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[4] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 5 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 5*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[5] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 6 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 6*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[6] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 7 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 7*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[7] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 8 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 8*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[8] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 9 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 9*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[9] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		for (j = 0; j < 10; j++) {
			printf("%d\n", dbuf[j]);
		}		
	}	
	
	second = total_tick / (CPU_HZ >> 3);
	printf("cpu clk %ld total_tick %ld \n", sys_cpu_clk, total_tick);
	printf("sequntial write: %d MB spend %d sec \n", size/(1024*1024), second);
	
    return 0;

EXIT:
	print_usage (sh_sd_seqw);
    printf("Error %d\n", rc);
    return -1;

}

static int sh_sd_nseqw(int argc, char **argv, void **result, void *sys_ctx)
{    
    int sec, i, j, size, second, cnt, sc, iosize;
    int rc = -1;
    uint32_t start; // sector number of sd card    
	char buf[1024];
	int dbuf[10];
	unsigned long ticks, total_tick;
	void *sd;
	drive_op_t *sd_op = NULL;
	struct sysc_t *sys = (struct sysc_t *)sys_ctx;
	struct fat_t *fat = sys->pfat;
	//struct sdc *gsdc = sys->psd;

	sd_op = fat->op;
	sd = fat->hcard;

	/* init sd card */
	rc = sd_op->init_card(sd);
    if (rc < 0) {
        dbg(2, "failed invoke init_card %d\n", rc);
        goto EXIT;
    }

    if (!sd_op) {
        printf("fail to get sd_op \n");
        goto EXIT;
    }

    if (argc < 4)
        goto EXIT;

    rc = get_val(argv[1], &sec);
    if (rc < 0)
        goto EXIT;

   	rc = get_val(argv[2], &iosize);
    if (rc < 0)
        goto EXIT;


    rc = get_val(argv[3], &size);
    if (rc < 0)
        goto EXIT;

    for (i = 0; i < 1024; i++) {
    	buf[i] = i;
    }
        
    /* write data to sd card */
	/* noe-sequencial write */
	sc = iosize / 512;   /* sc: sector count */
	cnt = size/(10*iosize);
	total_tick = 0;
	dbg(0, "sec %d-%d iosize %dKB io_cnt %d \n", sec, sec+size/512, iosize/1024, size/iosize);
	for (i = 0; i < cnt; i++) {
		/* sec 0 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 1*sc, buf, sc);		
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[0] = ticks;		
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 1 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 2*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[1] = ticks;
		ticks = (ticks >> 3);				
		total_tick += ticks;
		
		/* sec 2 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 3*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[2] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;				
		
		/* sec 3 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 4*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[3] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;				
		
		/* sec 4 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 0*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[4] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 5 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 6*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[5] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 6 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 7*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[6] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 7 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 8*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[7] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 8 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 9*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[8] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		
		/* sec 9 */
		start = read_c0_count32();
		rc = sdwraw_test(sd, i*(10*sc) + sec + 5*sc, buf, sc);
		if (rc < 0) {
			goto EXIT;
		}
		ticks = read_c0_count32() - start;
		dbuf[9] = ticks;
		ticks = (ticks >> 3);
		total_tick += ticks;
		for (j = 0; j < 10; j++) {
			printf("%d\n", dbuf[j]);
		}		
	}

	second = total_tick / (CPU_HZ >> 3);
	printf("cpu clk %ld total_tick %ld \n", sys_cpu_clk, total_tick);
	printf("non-sequntial write: %d MB spend %d sec \n", size/(1024*1024), second);

    return 0;

EXIT:
	print_usage (sh_sd_nseqw);
    printf("Error %d\n", rc);
    return -1;

}

#if 0
static int sh_fwrite(int argc, char **argv, void **result, void *sys_ctx)
{
	int fd = 0, rc;
	int *data;
	char *name, fwname[128];
	
	if (argc < 2)
		goto EXIT;		
	
	data = NULL;
	
	name = argv[1];
	strncpy(fwname, name, 128);
	fd = open(fwname, O_WRONLY);
	if (fd < 0) {
		dbg(0, "failed to open %s \n", fwname);
		goto EXIT;
	}

	data = (int *)0x80900000;
	rc = write(fd, data, 30*1024);
	if (rc < 0) {
		printf("failed to write !!! \n");
		goto EXIT;
	}
	printf("copy 0x%p len 30KB to SD ... \n", data);

	close(fd);
	
	return 0;
	
EXIT:
	print_usage (sh_fwrite);
	if (fd)
		close(fd);
		
	return -1;
}
#endif

static int sh_sd_evaluate (int argc, char **argv, void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat_t *fat = sys->pfat;
    drive_op_t *sd_op = fat->op;
    void *sd = fat->hcard;
    unsigned char buf[512];
    int offset = 64;
    int total, i;
    int rc;

    if (argc > 1)
        get_val (argv[1], &offset);

    sdc_debug |= SDC_DEBUG_READ;

    total = sd_op->device_size (sd);
    printf ("total sector: %d, seek offset: %d\n", total, offset);
    for (i = 0; i < total; i += offset) {
        rc = sd_op->read_sector(sd, i, 1, 512, buf);
        if (rc < 0)
            ERROR ("read_sector(%d)\n", i);
    }

    sdc_debug &= ~SDC_DEBUG_READ;

    return 0;
}
static int
sh_sd_speed (int argc, char **argv, void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat_t *fat = sys->pfat;
    drive_op_t *sd_op = fat->op;
    void *sd = fat->hcard;
    char *buf = heap_alloc(32*1024);
    unsigned int w_block_ticks[32];
    unsigned int ticks;
    unsigned int total_time;
    unsigned int rticks[64];
    unsigned int wticks[64];
    int i, j, k,start;
    int one_ms_tick = sys_cpu_clk/2/1000;
    int his_idx=0;
    int writ_size = 0;
    int rc;
    
    printf("SD Performance Test.\n");
    
    rc = sd_op->init_card(sd);
    if (rc < 0) {
        printf("Failed to probe sd card rc =%d\n", rc);
    }
  
    for(k=0; k<5; k++){
        
        if(k==0)
            writ_size = 2*512;
        else
            writ_size = k*2*4096;
        
        printf("-----> Write 32MB and %d byte at a time.\n", writ_size);
        memset (rticks, 0, sizeof (rticks));
        memset (wticks, 0, sizeof (wticks));
        memset (w_block_ticks, 0, sizeof (w_block_ticks));
       

        for (i = 0; i < 16; i++) {  //16 x 2MB
            start = i * 1024*1024;  // Histogram one time with 2MB 
            if((i%8)==0)
                printf ("\n");
            putchar ('.');
            for (j = 0; j < (2*1024*1024)/writ_size; j++) { //Histogram one time with 2MB 
                start += writ_size;                

                ticks = read_c0_count32();
                rc = sd_op->write_sector (sd, start/512, writ_size/512, 512, buf);
                if (rc < 0) {
                    printf("Failed to write sector i=%d j=%d rc=%d\n", i, j, rc);
                   
                }
                wticks[i] += read_c0_count32() - ticks;
                
                his_idx = (read_c0_count32() - ticks)/(one_ms_tick*5);
                if(his_idx>=32)
                    his_idx = 31;
                w_block_ticks[his_idx]++;
                if(his_idx>=3){
                    printf("%ldms",(read_c0_count32() - ticks)/(one_ms_tick));
                }
                
                //ticks = read_c0_count32();
                //sd_op->read_sector (sd, start/512, writ_size/512, 512, buf);
                //if (rc < 0) {
                //    printf("Failed to read sector i=%d j=%d rc=%d\n", i, j, rc);
                   
                //}
                //rticks[i] += read_c0_count32() - ticks;
                
                //ticks = read_c0_count32();
                //while((read_c0_count32() - ticks)<(one_ms_tick*15));
            }
        }
        printf ("\n");

        total_time = 0;
        printf ("   MB  R(MB/s)  W(MB/s)\n");
        printf ("  ---  -------  -------\n");
        for (i = 0; i < 64; i++) {
            unsigned int rtime, wtime;  // ms
            rtime = rticks[i]/(sys_cpu_clk/2/1000);
            wtime = wticks[i]/(sys_cpu_clk/2/1000);
            total_time += wtime;
            unsigned int rrate, wrate;  // 1000kb/s
            rrate = 1000000 / rtime;
            wrate = 1000000 / wtime;
            printf ("  %3d  %3d.%03d  %3d.%03d\n", i,
                    rrate/1000, rrate%1000,
                    wrate/1000, wrate%1000);               
            
        }
        printf("Total Write 32 MB = %d ms\n", total_time);
        
        printf("--------------Write %d kByte Period Histogram--------------\n", writ_size);
        for(j = 0; j < 31; j++) {
            printf ("%3d~%3d ms: %4d times\n",j*5, (j+1)*5,  w_block_ticks[j]); 
        }
            printf (">%3d    ms: %4d times\n",j*5, w_block_ticks[j]); 
    
    }
    heap_release(buf);
    return 0;
}

sh_sd_speed_check (int argc, char **argv, void **result, void *sys_ctx)
{
    int wtime;
    
    
    wtime = sdc_sd_speed_verify();
    if(wtime>50){
        wtime = sdc_sd_speed_verify();
        if(wtime>50)
            printf("Low performance sd card!!\n");
    }
  
    return 0;
}

#define CLUS2SECT(CLUS) (fat->root_start + ((CLUS) - FIRST_ROOTCLUSTER) * fat->sect_per_clus)
static int
sh_sd_dump (int argc, char **argv, void **result, void *sys_ctx)
{
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct fat_t *fat = sys->pfat;
    drive_op_t *sd_op = fat->op;
    void *sd = fat->hcard;
    int start, end, count, i;
    char buf[512];
    int rc = -1;

    if (argc < 3)
        goto EXIT;
    if (strcmp ("-s", argv[1]) == 0) {
        start = atoi (argv[2]);
    }
    else
    if (strcmp ("-c", argv[1]) == 0) {
        struct sysc_t *sys = (struct sysc_t *)sys_ctx;
        struct fat32 *fat = sys->pfat->fat;
        int clus = atoi (argv[2]);
        start = CLUS2SECT (clus);
    } else
        goto EXIT;


    count = argc > 3 ? atoi (argv[3]) : 1;
    end = start + count;

    for (i = start; i < end; i++) {
        rc = sd_op->read_sector (sd, start, 1, 512, buf);
        printf ("sector: %d\n", start);
        hexdump (start * 512, 512, buf);
        printf ("\n");
    }
    return 0;

EXIT:
    print_usage (sh_sd_dump);
    return rc;
}

static int sh_sd_status(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;

    rc = sdc_status();
    switch (rc) {
    case SDC_NONE:
        printf("No SD.\n");
        break;
    case SDC_INSERT:
        printf("SD is inserted.\n");
        break;
    case SDC_MOUNTED:
        printf("SD is mounted.\n");
        break;
    default:
        printf("Unknown SD state.\n");
        break;
    }
    
    return rc;
}
