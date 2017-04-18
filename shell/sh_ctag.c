/**
 *  @file   sh_ctag.c
 *  @brief  system init entry
 *  $Id: sh_ctag.c,v 1.2 2014/07/09 05:32:38 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
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
#include <mipsregs.h>
#include <sys.h>
#include <r4kcache.h>

command_init (sh_ctag, "$tag", "-$tag [ <addr-len> | <addr> ]");
command_alias (sh_ctag, 0, "$tagd", "-$tagi [ <addr-len> | <addr> ]");
command_alias (sh_ctag, 1, "$tagi", "-$tagi [ <addr-len> | <addr> ]");
command_init (sh_clock, "$locki", "-$locki [ <addr-len> | <addr> ]");
command_alias(sh_clock, 0, "$lockd", "-$lockd [ <addr-len> | <addr> ]");
command_init (sh_cfill, "$filli", "-$filli [ <addr-len> | <addr> ]");
command_alias(sh_cfill, 0, "$filld", "-$filld [ <addr-len> | <addr> ]");


#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	mips0					\n"	\
	"	.set	reorder"					\
	:								\
	: "i" (op), "m" (*(unsigned char *)(addr)))

#define Index_Load_Tag_I	    0x04
#define Index_Load_Tag_D	    0x05
#define Hit_Fetch_Lock_I        0x1c
#define Hit_Fetch_Lock_D        0x1d


extern struct cache_desc dcache, icache;

__mips32__
static void ctag_dump(unsigned long addr, unsigned long len, int all, int ic, int dc)
{
	unsigned long start = addr;
	unsigned long end = start+len;
    unsigned long itag, idata;
    unsigned long dtag, ddata;
	unsigned long ad;
    unsigned long pa;
	unsigned long iws_inc = 1UL << icache.waybit;
	unsigned long iws_end = icache.ways <<
	                       icache.waybit;
	unsigned long dws_inc = 1UL << dcache.waybit;
	unsigned long dws_end = dcache.ways <<
	                       dcache.waybit;

	unsigned long ws;
    unsigned long iimask;
    unsigned long dimask;
    int dp, ip;
    
    iimask = icache.waysize - 1;
    dimask = dcache.waysize - 1;
    
    for (ad = start; ad < end; ad += 4) {
        dp = 0; ip = 0;
        if (ic) {
	    for (ws = 0; ws < iws_end; ws += iws_inc) {
    		cache_op(Index_Load_Tag_I, ws| (ad & ~(3 << icache.waybit)) );
            itag = read_c0_taglo();
            idata = read_c0_datalo();
            pa = (itag & ~iimask) | (ad & iimask);
            //pa = itag;
            if (all || virt_to_phys(ad)  == pa) {
          	    printf("#%lx I%d> PA(%08lx) %c%c%c Data 0x%08lx\n", 
                     ad, (int)(ws/iws_inc), pa, 
                    ((itag & (1 << 7)) ? 'V' : '-'), 
                    ((itag & (1 << 6)) ? 'D' : '-'), 
                    ((itag & (1 << 5)) ? 'L' : '-'), 
                    idata);
                ip = 1;
            }
        }

        if (!ip) {
            printf("#%lx Ix> PA(-       ) --- Data 0x--------\n", ad);
        }
        }

        if (dc) {
	    for (ws = 0; ws < dws_end; ws += dws_inc) {
    		cache_op(Index_Load_Tag_D, ws| (ad & ~(3 << dcache.waybit)) );
            dtag = read_c0_taglo();
            ddata = read_c0_datalo();
            // pa = (dtag & dwmask) | (ad & dpamask);
            pa = (dtag & ~dimask) | (ad & dimask);
            if (all || virt_to_phys(ad) == pa) {
      	        printf("#%lx D%d> PA(%08lx) %c%c%c Data 0x%08lx\n", 
                    ad, (int)(ws/dws_inc), pa, 
                    ((dtag & (1 << 7)) ? 'V' : '-'), 
                    ((dtag & (1 << 6)) ? 'D' : '-'), 
                    ((dtag & (1 << 5)) ? 'L' : '-'), 
                    ddata);
                dp = 1;
            }
        }
        
        if (!dp) {
            printf("#%lx Dx> PA(-       ) --- Data 0x--------\n", ad);
        }
        }

	}
	
}


static int sh_ctag (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned char *addr;
    int len;
    int rc;
    int all;
	
	if (argc < 2) 
        goto EXIT;
    	
    if (strchr(argv[1], '+') || strchr(argv[1], '-')) {
  	    rc = get_addr_len(argv[1], &addr, &len);
  	} else {
        rc = get_addr(argv[1], &addr);
        len = 4;
    }
    
    if (argc > 2) {
        rc =  get_val(argv[2], &all);
        if (rc < 0)
            all = 0;
    }
    
    if (rc < 0)
    	goto EXIT;
    

    if (addr < (unsigned char *)KSEG0 || addr >= (unsigned char *)KSEG1) {
        printf("Invalid cache addr %p\n", addr);
        goto EXIT;
    }
        
    printf("Dump Cache $tag from %p to %p (%d)\n", addr, addr+len, len);

    if (strcmp(argv[0], "$tagi") == 0) {
        ctag_dump((unsigned long)addr,len, all, 1, 0);
    } else if (strcmp(argv[0], "$tagd") == 0) {
        ctag_dump((unsigned long)addr,len, all, 0, 1);
    } else {
        ctag_dump((unsigned long)addr,len, all, 1, 1);
    }

    return 0;

EXIT:
    print_usage(sh_ctag);
    return -1;
}


__mips32__
static void clock(unsigned long addr, unsigned long len, int cache_i)
{
	unsigned long start = addr;
	unsigned long end = start+len;
	unsigned long ad;

    for (ad = start; ad < end; ad += 4) {
        if (cache_i) {
		    cache_op(Hit_Fetch_Lock_I, ad);
        } else {
		    cache_op(Hit_Fetch_Lock_D, ad);
        }
    }

    if (cache_i) {
        ctag_dump((unsigned long) addr, len, 0, 1, 0);
    } else { 
        ctag_dump((unsigned long) addr, len, 0, 0, 1);
    }

}

__mips32__
static void cfill(unsigned long addr, unsigned long len, int cache_i)
{
	unsigned long start = addr;
	unsigned long end = start+len;
	unsigned long ad;

    for (ad = start; ad < end; ad += 4) {
        if (cache_i) {
		    cache_op(Hit_Fill_I, ad);
        } else {
            readl(ad&(-4));
        }
    }
    if (cache_i) {
        ctag_dump((unsigned long) addr, len, 0, 1, 0);
    } else {
        ctag_dump((unsigned long) addr, len, 0, 0, 1);
    }
}

static int sh_clock (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned char *addr;
    int len;
    int rc;
    int cache_i = 0;
	
	if (argc < 2) 
        goto EXIT;

    if (argv[0][5]== 'i')
        cache_i = 1;

    	
    if (strchr(argv[1], '+') || strchr(argv[1], '-')) {
  	    rc = get_addr_len(argv[1], &addr, &len);
  	} else {
        rc = get_addr(argv[1], &addr);
        len = 4;
    }
    
    if (rc < 0)
    	goto EXIT;
    

    if (addr < (unsigned char *)KSEG0 || addr >= (unsigned char *)KSEG1) {
        printf("Invalid cache addr %p\n", addr);
        goto EXIT;
    }
        
    printf("%cCache Lock Addr:%p+0x%x\n", (cache_i ? 'I' : 'D'), addr, len);

    clock((unsigned long)addr,len, cache_i);

    return 0;

EXIT:
    print_usage(sh_clock);
    return -1;
}

static int sh_cfill (int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned char *addr;
    int len;
    int rc;
    int cache_i = 0;
	
	if (argc < 2) 
        goto EXIT;

    if (argv[0][5]== 'i')
        cache_i = 1;

    	
    if (strchr(argv[1], '+') || strchr(argv[1], '-')) {
  	    rc = get_addr_len(argv[1], &addr, &len);
  	} else {
        rc = get_addr(argv[1], &addr);
        len = 4;
    }
    
    if (rc < 0)
    	goto EXIT;
    

    if (addr < (unsigned char *)KSEG0 || addr >= (unsigned char *)KSEG1) {
        printf("Invalid cache addr %p\n", addr);
        goto EXIT;
    }
        
    printf("%cCache Fill Addr:%p+0x%x\n", (cache_i ? 'I' : 'D'), addr, len);

    cfill((unsigned long)addr,len, cache_i);

    return 0;

EXIT:
    print_usage(sh_clock);
    return -1;
}
