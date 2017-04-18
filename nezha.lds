/*
 *  @file tarzan_res.lds
 *  @brief link script for tarzan with resource
 *
 *  $Id: nezha.lds,v 1.17 2015/07/17 07:35:03 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.17 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved
 *
 *  @date   2011/11/24  ycs    New file.
 */
OUTPUT_ARCH(mips)
OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips") 
ENTRY(__start)

__INFO_MAGIC  = 0x1a101a10;
NEZHA_ADDR = 0x80000000;
DATA_ADDR  = 0x8801a000;
STACK_ADDR = 0x88020000;
HEAP_ADDR  = 0x88020000;    /* 88020000 + 64K */

_nezha_ver      = __NEZHA_VER_X;
_build_number   = __BUILD_NUMBER_X;
_build_date     = __BUILD_DATE_X;
_ringtong_num   = __RINGTONG_NUM;
_ringtong_unitsize = __RINGTONG_UNITSIZE;

SECTIONS
{
    . = NEZHA_ADDR;
 
   /*******************************
    * bootsector + code + rodata
    *******************************/
    _ftboot = .;
   /* text output section */
    _ftext = .;

   .text : 
    { 
        ../sys/crt0.o (.text);
	    * (.text);
	. = ALIGN(4096);
            * (.inflate);
        * (.mips16.fn.*);
        * (.mips16.call.*);
	    . = ALIGN(4);
    }

    .rodata : 
    {
	    * (.rodata);
	    * (EXCLUDE_FILE (upgrade.lo) .rodata.*);	/* string and const */
        * (__ex_table);     /* __ex_table put a flush icache_line instruction */

	    . = ALIGN(4);
	    __cmd_start = .;
	    * (.cmd);
	    __cmd_end = .;
        . = ALIGN(4);

        __drvcall_start = .;
        * (.card_drv);
        __drvcall_end = .;
        . = ALIGN(16);

        * (.story);
        . = ALIGN(16);
        __story_start = .;
        * (.story_table);
        __story_end = .;

        . = ALIGN(16);
    } = 0xffff

    
    
    /DISCARD/ : 
    { 
        * (.reginfo) 
    }
    _etext = .;
    
    _bootsector_start = 0;
    _bootsector_size  = 512;
    _code_start       = 512;
    _code_size = _etext - _ftext - _bootsector_size; 
    _text_size = _etext - _ftext;
    _fcode            = _ftext + _bootsector_size;

    /***********************************
     * data + bss
     ***********************************/
    _fshadow = .;
    
    . = DATA_ADDR;

    /* data output section */
    _fdata = .;

    .data : 
    {
        * (.text_shadow.*);
        * (.text_shadow);
	    . = ALIGN(4);
	    * (.data);
        . = ALIGN(16);
    }
    _edata = .;
    _data_start = _code_start + _code_size; 
    _data_size  = _edata - _fdata;
    _etboot = .;
    _eshadow = .;
    /* bss output section */
    _fbss = .;
    .bss : 
    {
        * (.bss)
        * (.common)
    }
    _ebss = .;
    _bss_size = _ebss - _fbss;

    /*******************************
     * stack & heap
     *******************************/
    __STACK_BARRIER = .;
    _fstack_barrier = .;
    /* stack */
    _stack = STACK_ADDR;
    _fstack = _stack;
    _stack_size = _fstack - _fstack_barrier;
    _fheap = HEAP_ADDR;
    _heap_size = 64K;

    OVERLAY HEAP_ADDR : {
        /************************
         * upgrade
         ************************/
        .upgrade  
        {
	        _fupg = .;
            * (.text.upgrade);
            * (.rodata.upgrade);
	        . = ALIGN(16);
            _eupg = .;
        }
        /************************
        * dfuins
        *************************/
        .dfuins 
        {
            _fdfui = .;
            * (.text.di);
	        . = ALIGN(16);
            _edfui = .;
        }
    }
    _upgrade_start = _data_start + _data_size;
    _upgrade_size  = _eupg - _fupg;
    _dfuins_start = _upgrade_start + _upgrade_size;
    _dfuins_size  = _edfui - _fdfui;
 

    _rom_start = 0;
    _upgrade_start = _text_size + _data_size;
    _dfuins_start = _text_size + _data_size + _upgrade_size;
    _dfuins_nor = _dfuins_start + NEZHA_ADDR;
    _upgrade_nor = _upgrade_start + NEZHA_ADDR;
    _rom_size = _text_size + _data_size + _upgrade_size + _dfuins_size;



    /***********************************
     * resource, res_rp, ringtong, info,  
     ***********************************/
    . = NEZHA_ADDR + _rom_size;    
    .resource :
    {
	    _fresource = .;
        * (*.resource);
        . = ALIGN(16);
	    _eresource = .;
    }
    _res_start = _rom_start + _rom_size;
    _res_size = _eresource - _fresource;

    .res_rp :
    {
	    _fres_rp = .;
        * (.rsp);
        . = ALIGN(16);
	    _eres_rp = .;
    }
    _res_rp_start = _res_start + _res_size;
    _res_rp_size = _eres_rp - _fres_rp; 

    .ebin :
    {
        _febin = .;
        * (.ebin);
        _eebin = .;
        . = ALIGN(16);
        _exebin = .;
    }
    _ebin_start = _res_rp_start + _res_rp_size;
    _ebin_size = _eebin - _febin;
    _ebin_real_size = _exebin - _febin;

    .info :
    {
	    _finfo = .;
        LONG(__INFO_MAGIC);
	    _einfo = .;
	. = ALIGN(64K);
    } = 0xffff
    _einfo_align = .;
    _info_start = _ebin_start + _ebin_real_size;
    _info_size = _einfo - _finfo;
    _info_align_size = _einfo_align - _finfo;
    .ringtong :
    {
	    _fringtong = .;
        * (.rng);
        . = ALIGN(64K);
	    _eringtong = .;
    } =0xffff
    _ringtong_start = _info_start + _info_align_size;
    _ringtong_size = _eringtong - _fringtong;

    . = ALIGN(64K);
    _fregistry = .;
    . += 64K;
    _eregistry = .;
    _registry_start = _ringtong_start + _ringtong_size;
    _registry_size = _eregistry - _fregistry;

    . = ALIGN(64K);
    _fmedium = .;
    _medium_start = _registry_start + _registry_size;

}


