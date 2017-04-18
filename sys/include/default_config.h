/**
 *  @file   config.h
 *  @brief  library config.h
 *  $Id: default_config.h,v 1.2 2014/01/27 02:27:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc 	New file.
 *
 */

#ifndef _DEFAULT_CONFIG_H
#define _DEFAULT_CONFIG_H
#include<config.h>
/* ENABLE QUAD I/O FOR QUAD NOR FLASH */
#define ENABLE_QUAD_IO

#define CONFIG_WIN_QUAD_NOR     1

/*
 * CONFIG PARAMETERS
 */

/* QUAD NOR FLASH PARAMETERS */
#ifdef ENABLE_QUAD_IO
	//#define MXIC_QUAD_NOR	/* MXIC MX25LXXXX */
	//#define EON_QUAD_NOR	/* EON EN25QXX */
	//#define WIN_QUAD_NOR	/* WINBOND W25QXXXX */
	//#define AMIC_QUAD_NOR	/* AMIC A25LQXXX */
	//#define P_QUAD_NOR	/* P PM25LQXXX */
	
	#ifdef CONFIG_MXIC_QUAD_NOR
        // p7!=p3, p6!=p2, p5!=p1, p4!=p0 --> high performance mode
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xA5
		#define NONE_TOGGLE_VALUE	0xCC
	#endif /* END OF MAXIC_QUAD_NOR */
	#ifdef CONFIG_EON_QUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0x55
	#endif /* END OF EON_QUAD_NOR */
	#ifdef CONFIG_WIN_QUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0x55
	#endif /* END OF WIN_QUAD_NOR */
	#ifdef CONFIG_AMIC_QUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0x55
	#endif /* END OF AMIC_QUAD_NOR */
	#ifdef CONFIG_P_QUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0x55
	#endif /* END OF P_QUAD_NOR */
	#ifdef CONFIG_GD_CQUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0xCC
	#endif /* END OF GD_CQUAD_NOR */
	#ifdef CONFIG_WIN_CQUAD_NOR
		#define SUPPORT_HPM
		#define HPM_TOGGLE_VALUE	0xAA
		#define NONE_TOGGLE_VALUE	0xCC
	#endif /* END OF WIN_CQUAD_NOR */
	
#endif

#endif /* _CONFIG_H */

