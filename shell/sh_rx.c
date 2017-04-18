/**
 *  @file   sh_rx.c
 *  @brief  loadx command
 *  $Id: sh_rx.c,v 1.1.1.1 2013/12/18 03:43:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>


command_init (sh_rx, "rx", "rx <addr>");


#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define BS  0x08


#define TIMEOUT 1
// #define TIMEOUT_LONG 10
#define TIMEOUT_LONG 2
#define MAXERRORS 10

/*
 * <SOH><blk#><255 - blk#>< 128 data bytes><chksum>
 * <STX><blk#><255 - blk#><1024 data bytes><chksum>
 */


static inline void write_byte(int fd, char cc) 
{
	putb(cc);
}

static int read_byte(int fd, unsigned int timeout) 
{
    int c;
    unsigned int mark;
    int i;

    if ((c = getb2()) >= 0)
        return c;

    mark = read_c0_count32() + (sys_cpu_clk/2);

    for (i = 0; i < timeout; i++) {
        while((int)(read_c0_count32()-mark) < 0) {
            if ((c = getb2()) >= 0)
                return c;
        }
        mark += (sys_cpu_clk/2);
    }

    return -1;
}

static void read_flush(int fd)
{
    while(getb2() >= 0);
    while(getb2() >= 0);
    while(getb2() >= 0);
}


//#define DEBUG_RX

#ifdef DEBUG_RX

static char debug[4096];
static char *p, *e;

#define dbg(cond, fmt, args...) \
    if (cond) { p += snprintf(p, e-p, fmt, ##args); }
#define dbg_init(fmt, args...)      \
    ({ p = debug; e = debug+4096;   \
       dbg(1, fmt, ##args); })

#define dbg_dump()      printf("%s\n", debug);

#define note_error(fmt, args...)    dbg(1, fmt, ##args)
#else

#define dbg_init(fmt, args...)      /* do nothing */
#define dbg(cond, fmt, args...)     /* do nothing */
#define dbg_dump()                  /* do nothing */
#define note_error(fmt, args...)    /* do nothing */
#endif



int rx(char *addr)
{
    int ttyfd = 0;
	unsigned int errors = 0;
	unsigned int wantBlockNo = 1;
	unsigned int length = 0;
	int docrc = 1;
	char nak = 'C';
    unsigned int timeout = TIMEOUT_LONG;
    unsigned char *blockBuf;

    blockBuf = (unsigned char *)addr;

    read_flush(ttyfd);

	/* Ask for CRC; if we get errors, we will go with checksum */
	write_byte(ttyfd, nak);

    for (;;) {
		int blockBegin;
		int blockNo = 0, blockNoOnesCompl = 0;
		int blockLength = 0;
		int cksum = 0;
		int crcHi = 0;
		int crcLo = 0;
        int state = 0;

		blockBegin = read_byte(ttyfd, timeout);
		if (blockBegin < 0)
			goto timeout;

        state = 1;
		timeout = TIMEOUT;
		nak = NAK;

		switch (blockBegin) {
		case SOH:
		case STX:
			break;

		case EOT:
			write_byte(ttyfd, ACK);
			goto done;

		default:
			goto error;
		}

		/* block no */
		blockNo = read_byte(ttyfd, TIMEOUT);
		if (blockNo < 0)
			goto timeout;

        state = 2;

		/* block no one's compliment */
		blockNoOnesCompl = read_byte(ttyfd, TIMEOUT);
		if (blockNoOnesCompl < 0)
			goto timeout;

        state = 3;

		if (blockNo != (255 - blockNoOnesCompl)) {
			note_error("bad block ones compl\n");
			goto error;
		}

		blockLength = (blockBegin == SOH) ? 128 : 1024;

		{
			int i;

			for (i = 0; i < blockLength; i++) {
				int cc = read_byte(ttyfd, TIMEOUT);
				if (cc < 0)
					goto timeout;
				blockBuf[i] = cc;
			}
		}

		if (docrc) {
			crcHi = read_byte(ttyfd, TIMEOUT);
			if (crcHi < 0)
				goto timeout;

			crcLo = read_byte(ttyfd, TIMEOUT);
			if (crcLo < 0)
				goto timeout;
            state = 4;
		} else {
			cksum = read_byte(ttyfd, TIMEOUT);
			if (cksum < 0)
				goto timeout;
            state = 5;
		}

		if (blockNo == ((wantBlockNo - 1) & 0xff)) {
			/* a repeat of the last block is ok, just ignore it. */
			/* this also ignores the initial block 0 which is */
			/* meta data. */
			goto next;
		} else if (blockNo != (wantBlockNo & 0xff)) {
			note_error("unexpected block no, 0x%08x, expecting 0x%08x\n", blockNo, wantBlockNo);
			goto error;
		}

		if (docrc) {
			int crc = 0;
			int i, j;
			int expectedCrcHi;
			int expectedCrcLo;

			for (i = 0; i < blockLength; i++) {
				crc = crc ^ (int) blockBuf[i] << 8;
				for (j = 0; j < 8; j++)
					if (crc & 0x8000)
						crc = crc << 1 ^ 0x1021;
					else
						crc = crc << 1;
			}

			expectedCrcHi = (crc >> 8) & 0xff;
			expectedCrcLo = crc & 0xff;

			if ((crcHi != expectedCrcHi) ||
			    (crcLo != expectedCrcLo)) {
				note_error("crc error, %p %x expected 0x%02x 0x%02x, got 0x%02x 0x%02x\n", blockBuf, length, expectedCrcHi, expectedCrcLo, crcHi, crcLo);
				goto error;
			}
		} else {
			unsigned char expectedCksum = 0;
			int i;

			for (i = 0; i < blockLength; i++)
				expectedCksum += blockBuf[i];

			if (cksum != expectedCksum) {
				note_error("checksum error, expected 0x%02x, got 0x%02x\n", expectedCksum, cksum);
				goto error;
			}
		}

        state = 6;
		wantBlockNo++;
		length += blockLength;
        blockBuf += blockLength;


	next:
		errors = 0;
		write_byte(ttyfd, ACK);
		continue;

	error:
	timeout:
		errors++;
        dbg(state >= 1, "S(%x)", blockBegin);
        dbg(state >= 2, "b(%x)", blockNo);
        dbg(state >= 3, "~(%x)", blockNoOnesCompl);
        dbg(state >= 3, "%d", blockLength);
        dbg(state == 4, "CRC(%02x %02x)", crcHi, crcLo);
        dbg(state == 5, "CHK(%02x)", cksum);
        dbg(1, "*to(%d)\n", errors);

		if (errors == MAXERRORS) {
			/* Abort */
			int i;

			// if using crc, try again w/o crc
			if (nak == 'C') {
				nak = NAK;
				errors = 0;
				docrc = 0;
				goto timeout;
			}

			note_error("too many errors; giving up\n");

		// fatal:
			for (i = 0; i < 5; i ++)
				write_byte(ttyfd, CAN);
			for (i = 0; i < 5; i ++)
				write_byte(ttyfd, BS);
			return -1;
		}
    
        read_flush(ttyfd);
		write_byte(ttyfd, nak);
    }


done:
    return length;
}

#undef note_error

static int sh_rx (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned char *addr;
    int len;
    int rc;

	if (argc < 2) {
        dbg_dump();
        goto EXIT;
    }

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;
    

    printf("Ready to download data to %p\n", addr);
    len = rx((char *)addr);
    
    if (len < 0) {
        printf("Error occured or user abandon\n"); 
        dbg_dump();
    } else {
        printf("\naddr: %p\nSize: %xh (%d) received\n",  addr, len, len);
    }

    return 0;

EXIT:
    print_usage(sh_rx);
    return -1;
}

