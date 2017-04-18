/**
 *  @file   crt0.S
 *  @brief  Startup code compliant to POSIX like library
 *  $Id: crt0.S,v 1.8 2014/04/10 02:23:15 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/21  New file.
 *
 */
#include <config.h>
#include <io.h>
#include <regdef.h>
#include <mipsregs.h>
#include <asm.h>
#include <stackframe.h>

/* 
 * This is the canonical entry point, usuall the first thing in the text
 * segment.  The MIPS ABI says that when the entry point runs, most 
 * registers' values are unspecified, except for:
 *
 * v0($0)   Contains a function pointer to be registered with 'atexit'.
 *          this is how the dynamic linker arranges to have DT_FINI
 *          functions called for shared libraries that have been loaded
 *          before this code runs.
 *
 * sp($29)  The stack contains the arguments and environment:
 *          0(%sp)              argc
 *          4(%sp)              argv[0]
 *          ...
 *          (4*argc)(%sp)       NULL
 *          (4*(argc+1))(%sp)   envp[0]
 *
 * ra($31)  The return address register is set to zero so that programs
 *          that search backword through stack frames recognize the last 
 *          stack frame
 */

#ifdef CONFIG_CODE_NOR_LAYOUT
#define _CODE_NOR_LAYOUT		CONFIG_CODE_NOR_LAYOUT
#else
#define _CODE_NOR_LAYOUT		"(64,0-31)"
#endif

/*
  * We need to call:
  *   main(int argc, char **argv)
  */

.text
    .align  2
    .globl  __start
    .type   __start,@function
    .type   main,@function
    .type   heap_init, @function
    .type   puts,@function
    .type   memset@function
	.long	0x55aa55aa
__start:
    j       nezha_start
    nop


/**************************************
 * nezha_param: app paramenters
 **************************************/
.org 0x10
    .text
nezha_param:
    .word  config_code_nor_layout
	.word  0
/*************************************
 * binary layout information
 *************************************/
.org 0x100
bin_layout_info:
	 .long  0x1a1a1010
     .long  _nezha_ver
     .long  _build_date
     .long  _build_number
     .long  _rom_start
     .long  _rom_size
     .long  _code_start
     .long  _code_size
     .long  _data_start
     .long  _data_size
     .long  _upgrade_start
     .long  _upgrade_size
     .long  _dfuins_start
     .long  _dfuins_size
     .long  _res_start
     .long  _res_size
	 .long  _res_rp_start
	 .long  _res_rp_size
     .long  _ringtong_num
     .long  _ringtong_unitsize
	 .long  _ringtong_start
	 .long  _ringtong_size
	 .long  _registry_start
	 .long  _registry_size
     .long  _medium_start
     .long  _ebin_start
     .long  _ebin_size
     .long  0x10101a1a
     
/***********************************
 * exception table
 ***********************************/
.org 0x180
     .text
     .align  4   
     .set    push     
     .set    reorder

        /*
         * General exception vector.  Used for all mips32 CPUs 
         */
NESTED(except_vec3_generic, 0, sp)
        mfc0    k0, CP0_INDEX
        mfc0    k1, CP0_CAUSE
        la      k0, exception_handlers
        andi    k1, k1, 0x7c
        addu    k0, k0, k1
        lw      k0, (k0)
        jr      k0
END(except_vec3_generic)
        .set    at

/****************************************
 * nezha_start:  app loader
 ****************************************/
.org 0x400

nezha_start:
    li      s0, 1          /* pass argument a0, a1, a2 to main */
    la      s1, nezha_param
    li      s2, 0          /* a0: flash type 1 NAND, 2 NOR, a1: pat, a2: 0x55aa55aa */
    move    ra, $0          /* ra = 0 */
    la      sp, _stack      /* init stack pointer */


    /* clear status and cause */
    mtc0    zero, $12       /* c0_status */
    mtc0    zero, $13       /* c0_cause */
    
    /* flush icache */
    la      a0, _ftext
    la      a1, _etext 
    bal     flush_cache16

#if 1
	/* move .data to shadowed address */
	la		t0, _fshadow
	la		t1, _fdata
	la		t2, _edata
1:	lw		t3, 0(t0)
	add		t0, t0, 4
	sw		t3, 0(t1)
	add 	t1, t1, 4
	bne		t1, t2, 1b
#endif

    /* reset _fbss ~ _ebss */
    la      a0, _fbss
    la      a2, _ebss
    subu    a2, a2, a0
    move    a1, zero
    la      t0, memset
    jalr    t0

    /* invoke main function */
    move    a0, s0
    move    a1, s1
    move    a2, s2
    la      t0, main
    jalr    t0

    /* print Everything is done */
    la      a0, __done
    la      t0, puts
    jalr    t0
hlt:
    b       hlt             /* everything is done */



/**
 * flush_cache16
 * @param: $a0 start text address
 * @param: $a0 end of text address
 */
#if CONFIG_5120
flush_cache16:
//    and     t0, a0, PHYSMASK    /* t0 = start physical address to refill */
//    and     t1, a1, PHYSMASK    /* t1 = end physical address to refill */
    move    t0, a0
    move    t1, a1
1:  cache   0x10, 00(t0)
    cache   0x10, 16(t0)
    cache   0x10, 32(t0)
    cache   0x10, 48(t0)
    add     t0, 64
    blt     t0, t1, 1b
    jr      ra    
#else /* T300 */
flush_cache16:     /* flush both dcache and icache */
    li      t3, KSEG0   
    li      t2, 4           /* 4 ways */
    mtc0    zero, $28       /* TagLo = 0 */
2:  move    t0, t3          /* t0 = start */
    add     t1, t0, 2048    /* t1 = 2K end */
1:  cache   0x00, 00(t0)    /* Index Invalidate I */
    cache   0x00, 16(t0)
    cache   0x00, 32(t0)
    cache   0x00, 48(t0)
    cache   0x01, 00(t0)    /* Index WB invalidate */
    cache   0x01, 16(t0)
    cache   0x01, 32(t0)
    cache   0x01, 48(t0)
    add     t0, 64
    blt     t0, t1, 1b
    add     t3, 2048        /* next way */
    sub     t2, 1
    bnez    t2, 2b
    jr      ra    
#endif
    
.section .rodata
__done:   .string     "==== Everything is done! ===\n"
 
