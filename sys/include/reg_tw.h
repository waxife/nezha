/**
 *  @file   reg_tw.h
 *  @brief  head file for Terawins of regesters define 
 *  $Id: reg_tw.h,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc 	New file.
 *
 */

#ifndef _REG_TW_H
#define _REG_TW_H

/* CHIP CONFIG. REGISTERS PAGE */
#define PAGE_OFFSET				0x1000 

#define TWIC_P0   				0x00	  
#define TWIC_P1   				TWIC_P0 + PAGE_OFFSET
#define TWIC_P2   				TWIC_P1 + PAGE_OFFSET
#define TWIC_P3					TWIC_P2 + PAGE_OFFSET
#define TWIC_P4					TWIC_P3 + PAGE_OFFSET
#define TWIC_P5					TWIC_P4 + PAGE_OFFSET
#define TWIC_P6					TWIC_P5 + PAGE_OFFSET


//---------------------------------------------------------------------------
// The postfix of _REG defines its register address
// The postfix of _DEFAULT defines the default value of the register of the chip, 
// and these values should not be modified
//---------------------------------------------------------------------------

/*****************************************************************************/
/* RESGISTERS PAGE 0                                                         */
/*****************************************************************************/	
	// Image function control register, 0x90
	//   [7:6] Gamma table select       11: Gamma table R
	//                                            10: Gamma table G
	//                                            01: Gamma table B
	//                                            00: All 3
	//   [5] Gamma_BIST_En             Enable Gamma RAM BIST.
	//   [4:2] reserved
	//   [1] enable Gamma
	//   [0] enable Dithering	
	#define IMG_FUNCTRL_REG				0x90
		#define IMG_FUNCTRL_DEFAULT			0x05
		#define IMG_FUNCTRL					0x00
	
	/* Built-in pattern generator control register
	[7]		R/W		EFMCLR		Enable Frame background color
								Turn on this bit may disable Scaler¡¦s color and show userdefined
								color on LCD panel.
								See 0x9D, 0x9E and 0x9F for user-defined frame color.
	[6]		R/W		ESLDSW		This bit may enable pattern generator shows 9 patterns
								sequentially.
	[5]		R/W		EVBAR		Enable Vertical Bar Patterns
	[4]		R/W		PLBIT		1: indicate 8-bit patterns
								0:indicate 6-bit patterns
	[3:0]	R/W		PTN			Show nth pattern on LCD panel when EFMCLR is enabled
								When Both EFMCLR and ESLDSW are enabled, pattern generator
								may show 0, 1 ,2 ...up to PTNth.
	*/
	#define BTIN_PATTERN_REG			0x91
		#define BTIN_PATTERN_DEFAULT		0x04
		#define BTIN_PATTERN				0x04
	
	/* GAMMA Table Address Port Register
	[7:0]	R/W		GAMMA_ADR	Gamma coefficient table address. The Index range is 00h~FFh
	*/
	#define GAMMA_ADD_REG				0x93
		#define GAMMA_ADD_DEFAULT			0x00
	
	/* GAMMA Table Write Data Port Register
	[7:0]	WO		GAMMA_WR_D		Gamma coefficient write data port.
	*/
	#define GAMMA_WRDATA_REG				0x94
		#define GAMMA_WRDATA_DEFAULT			0x00
	
	// Pattern Bar Width Register
	// [7:0]	R/W		Pattern_Bar_Width		This is for generated pattern vertical bar width (for patterns: Color Bar
	//									or Gray ramp)
	#define PATT_BARWIDTH_REG				0x98
		#define PATT_BARWIDTH_DEFAULT			0x3C 
		#define PATT_BARWIDTH					0xCC
		
	// Pattern color gradient & dithering mode register, 0x9C
	//   [7:4] when P0_09[7:6] set to 11. this bit may set color gradient at pattern 2,3,4,5
	//   [3:0] reserved
	#define PATTERN_GRA_DITH_REG		0x9C
		#define PATTERN_GRA_DITH_DEFAULT	0x00
		#define PATTERN_GRA_DITH			0x02
		
	/* Frame Color Red Configuration Register
	[7:0]	R/W		FMCLR	8 bits of red color depth for frame color.
	*/
	#define FRAME_CRED_CFG_REG		0x9D
	#define FRAME_CGRN_CFG_REG		0x9E
	#define FRAME_CBLU_CFG_REG		0x9F
		#define FRAME_CRED_CFG_DEFAULT		0x00
		#define FRAME_CGRN_CFG_DEFAULT		0x00
		#define FRAME_CBLU_CFG_DEFAULT		0x00

		#define BULE_CRED			0x10
		#define BULE_CGRN			0xFF
		#define BULE_CBLU			0x80


	// output timing control
	// Display window horizontal/vertical start LSB/MSB register, 0xB0~0xB3
	//   [7:0] LSB
	//	when PAUTO_SYNC is enabled, DWVS is set by auto-detection (default ?)
	//
	// Display window horizontal/vertical width(size) LSB/MSB register, 0xB4~0xB7
	//   [7:0] LSB
	//	[2:0] MSB(H)	
	//	[1:0] MSB(V) 
	//
	// Display panel horizontal/vertical total dots per scan line LSB/MSB register,0xB8~0xBB
	//   [7:0] LSB
	//   [7:4] reserved
	//   [3:0] MSB
	//
	// Display panel Hsync/Vsync width LSB/MSB register, 0xBC~0xBF
	//   [7:0] LSB
	//   [7:4] reserved
	//   [3:0] MSB
	//
	#define DWHS_REG				0xB0
	#define DWVS_REG				0xB2
	#define DWHSZ_LSB_REG			0xB4
	#define DWHSZ_MSB_REG			0xB5
	#define DWVSZ_LSB_REG			0xB6
	#define DWVSZ_MSB_REG			0xB7
	#define PH_TOT_LSB_REG			0xB8
	#define PH_TOT_MSB_REG			0xB9
	#define PV_TOT_LSB_REG			0xBA
	#define PV_TOT_MSB_REG			0xBB
	#define PH_PW_REG				0xBC
	#define PV_PW_REG				0xBE
		#define DWHS_DEFAULT			0x20
		#define DWVS_DEFAULT			0x10
		#define DWHSZ_LSB_DEFAULT		0xE0
		#define DWHSZ_MSB_DEFAULT		0x01
		#define DWVSZ_LSB_DEFAULT		0xEA
		#define DWVSZ_MSB_DEFAULT		0x00
		#define PH_TOT_LSB_DEFAULT		0x80
		#define PH_TOT_MSB_DEFAULT		0x03
		#define PV_TOT_LSB_DEFAULT		0x58
		#define PV_TOT_MSB_DEFAULT		0x02
		#define PH_PW_DEFAULT			0x10
		#define PV_PW_DEFAULT			0x02
	

	/* Panel Vsync frame delay control register, 0xC2
	[7]		 RO		 Reserved
	[6:5]	 R/W	 Reserved
	[4]		 R/W	 PSYNC_STR	 For Frame lock, input VSync (if exist) will trigger output VSync
									0: Allow input vsync to trigger output vsync
									1: Block input vsync triggering on output vsync
	[3]		 R/W	 Reserved
	[2]		 RO		 Reserved
	[1]		 R/W	 IGNORE_VSYNC	 Ignore the input VSYNC. This can be used for output free run when
									 input VSYN is not available
	[0]		 WO		 Reserved		 For Chip Test only
	*/
	#define POUT_VSYNC_CTRL_REG			0xC2
		#define POUT_VSYNC_CTRL_DEFAULT		0x00
		#define POUT_VSYNC_CTRL				0x12
		#define POUT_VSYNC_CTRL_IGNOREVS	0x02
	
	// define power management control register (R/W), 0xE0
	//[7] R/W PD_TotalPad_	Set to 0 for Power Down all I/O pads, except I2C I/F.
	//[6] R/W PD_CombLB
	//[5] R/W PD_ADCD_		Set to 0 for Power Down ADC digital portion.
	//[4] R/W PD_VD_		Set to 0 for Power Down Comb Video Decoder block.
	//[3] R/W LLCK1_EN		LLCK1 enable
	//[2] R/W LLCK2_EN		LLCK2 enable
	//[1] R/W LLCK3_EN		LLCK3 enable
	//[0] R/W PD_TC_		Set to 0 for Power down TC interface.	
	#define PW_MGRCTRL_REG				0xE0
		#define PW_MGRCTRL_DEFAULT			0x12
	
		#define PWRDN_TOTALPAD				0x80
		#define PWRDN_COMBLB				0x40
		#define PWRDN_ADC					0x20
		#define PWRDN_VIDEODECODER			0x10
		#define LLCK1						0x08
		#define LLCK2						0x04
		#define LLCK3						0x02
		#define PWRDN_TC					0x01
	// Output pin configuration, 0xE1
	//	[7:6] Row STV select	00: output both
	//						01: output both
	//						10: output STV1
	//						11: output STV2
	//	[5:4] Col STH select		00: output both
	//						01: output both
	//						10: output STH1
	//						11: output STH2
	//	[3] up-down selection
	//	[2] left-right selection
	//	[1] Ext_POLC_sel
	//  [0] Psync_sel: 0=TCON, 1=HSO/VSO
	#define OPIN_CFG_REG				0xE1
		#define OPIN_CFG_DEFAULT			0x00

	/* Shadow Control Configuration
	[7:5]	RO	Reserved
	[4]		R/W	Shadow_Enable	1: Enable registers shadow control
	[3:1]	RO	Reserved
	[0]		WO	Shadow_Sync		Write 1 to sync all shadowed registers
	*/
	#define SHADOW_CTRL_CONF_REG		0xE2
		#define SHADOW_CTRL_CONF_DEFAULT	0x10
		#define SHADOW_CTRL_CONF			0x11
		#define SHADOW_CTRL_ENABLE			0x11
		#define SHADOW_CTRL_DISABLE			0x00
		#define SHADOW_BUSY					0x01

	//-------------------------- OSD2 Register Set -------------------------//
	#define OSD_CFG_INDEX_REG 			0xA8
	#define OSD_CFG_DATA_REG			0xA9
	#define OSD_RAM_AL_REG    			0xAA
	#define OSD_RAM_AH_REG    			0xAA
	#define OSD_RAM_DL_REG    			0xAB
	#define OSD_RAM_DH_REG    			0xAB
	//-----------------------------------------------------------------------//
	
	//-------------------------- OSD2 Register Map -------------------------//
    //Global Setting
    #define OSD_CTRL                    0x00
    #define OSD_CHAR_FONT_SIZE          0x01
    #define OSD_CHAR2BP_FONT_INDEX      0x02
    #define OSD_CHAR4BP_FONT_INDEX      0x03
    #define OSD_CHAR2BP_FONT_MEMADD_L   0x04
    #define OSD_CHAR2BP_FONT_MEMADD_M   0x05
    #define OSD_CHAR4BP_FONT_MEMADD_L   0x06
    #define OSD_CHAR4BP_FONT_MEMADD_M   0x07
    #define OSD_COLOR_LUT_ADR_PORT      0x08
    #define OSD_COLOR_LUT_DATA_PORT     0x09
    #define OSD_WINDOWS_SHADOW          0x0A
    #define OSD_GLOBAL_ALPHA_BLENDING   0x0B
    #define OSD_BSH_COLOR_HIGHBIT       0x0C
    #define OSD_FONTROM_INDEX           0x0D
    #define OSD_LUTW_CHROMAKEY          0x0E
    //Menu-1 Setting
    #define OSD_MENU1_ENABLE            0x10
    #define OSD_MENU1_START_ADR_L       0x11
    #define OSD_MENU1_START_ADR_M       0x12
    #define OSD_MENU1_END_ADR_L         0x13
    #define OSD_MENU1_END_ADR_M         0x14
    //OSD1 ROM Font Setting
    #define OSD_ROM_FONT_ADR_L          0x16
    #define OSD_ROM_FONT_ADR_M          0x17
    //Menue-2 Setting
    #define OSD_MENU2_ENABLE            0x18
    #define OSD_MENU2_START_ADR_L       0x19
    #define OSD_MENU2_START_ADR_M       0x1A
    #define OSD_MENU2_END_ADR_L         0x1B
    #define OSD_MENU2_END_ADR_M         0x1C
    //BMP Setting
    #define OSD_BMP_CTRL                0x20
    #define OSD_BMP_START_ADR_L         0x21
    #define OSD_BMP_START_ADR_M         0x22
    #define OSD_BMP_APHBLD_CTRL         0x23
    #define OSD_BMP_HSIZE_L             0x24
    #define OSD_BMP_HSIZE_M             0x25
    #define OSD_BMP_VSIZE_L             0x26
    #define OSD_BMP_VSIZE_M             0x27
    #define OSD_BMP_HSTART_L            0x28
    #define OSD_BMP_HSTART_M            0x29
    #define OSD_BMP_VSTART_L            0x2A
    #define OSD_BMP_VSTART_M            0x2B
    #define OSD_BMP_LUT_BASSADR         0x2C
    #define OSD_BMP_BACK_COLOR          0x2D
    
    //Pattern Fill
    #define OSD_PT_CTRL                 0x30
    #define OSD_PT_LUT_BASEADD          0x31
    #define OSD_PT_HSIZE                0x32
    #define OSD_PT_VSIZE                0x33
    #define OSD_PT_ROW_SHIFT            0x34
    #define OSD_PT_ALPBL_CTRL           0x35
    #define OSD_PT_BR_PE                0x36
    #define OSD_PT_RAM_WRITE_PORT       0x37
    #define OSD_PT_HSTART_LSB           0x38
    #define OSD_PT_HSTART_MSB           0x39
    #define OSD_PT_VSTART_LSB           0x3A
    #define OSD_PT_VSTART_MSB           0x3B
    #define OSD_PT_HEND_LSB             0x3C
    #define OSD_PT_HEND_MSB             0x3D
    #define OSD_PT_VEND_LSB             0x3E
    #define OSD_PT_VEND_MSB             0x3F
    
    #define OSD_BLK_WRT_LSB             0x40
    #define OSD_BLK_WRT_MSB             0x41
    #define OSD_BLK_WRT_SADD_LSB        0x42
    #define OSD_BLK_WRT_SADD_MSB        0x43
    #define OSD_BLK_WRT_LENGTH          0x44
    #define OSD_BLK_WRT_CTR             0x45

    #define OSD_CHAR8BP_FONT_INDEX      0x50
    #define OSD_CHAR8BP_FONT_MEMADD_L   0x51
    #define OSD_CHAR8BP_FONT_MEMADD_M   0x52
/*****************************************************************************/
/* RESGISTERS PAGE 1                                                         */
/*****************************************************************************/
	// define pin function select register (R/W), 0xAC
	//[4]   1: UART1
	//[3:2] 2: FLOW_CONTROL_FOR_UART1
	//[1:0] 1: UART0
	#define PIN_FUNCTION_REG				0xAC
		#define PIN_FUNCTION_DEFAULT			0x00
	
		#define UART0_MASK					0x03
		#define UART0_EN					0x01
		#define UART1_EN					(1<<4)
        #define RLUD_CPUINT_MASK            0x0C
   		#define RLUD_EN					    (1<<2)
   		#define RS232_AFC_EN				(2<<2)
        #define RL_OUT_CPUINT_EN            (3<<2)
        #define CPUINT_OESEL_MASK           0x03
        #define CPUINT_EN                   0
        #define RS232_EN                    1
        //#define DDC_EN                      2
        #define DDC_EN                      3

/*****************************************************************************/
/* RESGISTERS PAGE 3                                                         */
/*****************************************************************************/

	/* >>>>>>>>>>>>>>>>>>>> I80 MASTER INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
	/* System Register */
	#define I80_BURST_IDX_MREG    0x20
	#define I80_BURST_IDX_LREG    0x21
	#define I80_OUTPUT_ORDER_REG  0x22
		#define ORG_BE         		0   		/* original BE */
		#define ORDER_2LE1      	1   		/* 8/9bit[17:9] bus reorder to LE */
		#define ORDER_2LE2      	2   		/* 8/9bit bus reorder to LE */
		#define ORDER_MASK      	3   		/* 11b, reorder mask */
		#define I80_IDX_SWP         4   		/* index swap */
		#define I80_DATA_SWP        8   		/* data swap */
	#define I80_BUS_CONFIG_REG    0x23
		#define I80_8BIT            (0 << 4)    /* 00b, output bus  8bit */
		#define I80_9BIT            (1 << 4)    /* 01b, output bus  9bit */
		#define I80_16BIT           (2 << 4)    /* 10b, output bus 16bit */
		#define I80_18BIT           (3 << 4)    /* 11b, output bus 18bit */
		#define I80_BUS_MASK        (3 << 4)    /* 11b, output bus mask */
		#define ONE_PREP_DONE       (4 << 4)
		#define I80_VSYNC_POL       (8 << 4)
	#define I80_SYS_REG    		  0x23		
		#define SYS_RST_            (1 << 1)    /* panel reset (active low) */
		#define SYS_ENABLE          (1 << 0)
	/* H GRAM Register */	
	#define HREG_IDX_MREG		0x24 
	#define HREG_IDX_LREG		0x25
	#define HGRAM_AD_MREG		0x26
	#define HGRAM_AD_LREG		0x27
	/* V GRAM Register */
	#define VREG_IDX_MREG		0x28 
	#define VREG_IDX_LREG		0x29
	#define VGRAM_AD_MREG		0x2A	
	#define VGRAM_AD_LREG		0x2B

	#define WR_RE_WIDTH_REG		0x2C
	#define RD_RE_WIDTH_REG		0x2D
	#define WR_STR_WIDTH_REG	0x2E
	#define RD_STR_WIDTH_REG	0x2F
		#define INHIBITED			1
	/* VSYNC & Data retention Register */
	#define WR_COMMAND_CTRL_REG	0x30
		#define WD_CNT_MASK			0x0F   		/* 1111b, write data trans cycle mask */	
		#define WA_CNT_MASK			(3 << 4)    /* 11b,  write address trans cycle mask */	
		#define WD_08BIT			(4 << 4)    /* write data 8bit */	
		#define WA_08BIT			(8 << 4)    /* write address 8bit */
	#define OUT_VSYNC_WH_REG	0x31
	#define DATA_RE_GAP_REG		0x32
	#define WR_HOLD_TIME_REG	0x33
		#define WD_HOLD_MASK		0xF0		/* 1111b, write data */
		#define WA_HOLD_MASK		0x0F		/* 1111b, write address */
	/* Tear Movement */
	#define OUT_VSYNC_DLY_MREG	0x34
	#define OUT_VSYNC_DLY_LREG	0x35
	#define OUT_VSYNC_PER_MREG	0x36
	#define OUT_VSYNC_PER_LREG	0x37
	#define OUT_SYNC_CONFIG_REG	0x38
		#define OUT_FROM_SCALER		(8<<4)
		#define HSYNC_FREE_EN		(4<<4)
		#define DE_POLARITY			(8<<0)
		#define HSYNC_POLARITY		(4<<0)
		#define VS_DE_ASYNC_EN		(2<<0)
		#define VSYNC_FREE_EN		(1<<0)
	#define OUT_HSYNC_PER_MREG	0x39
	#define OUT_HSYNC_PER_LREG	0x3A
	#define OUT_HSYNC_WH_REG	0x3B
	/* Source Select Register */
	#define I80_SRC_SEL_REG		0x3F
		#define	MODE_COMMAND		(0<<4)
		#define	MODE_VSYNC			(1<<4)
		#define MODE_RGB			(2<<4)
		#define MODE_SEL_MASK		(3<<4)
		#define CMD_SOURCE 			(4 << 0)
		#define SRC_CPU             (0 << 0)
		#define SRC_SCALER          (1 << 0)
		#define SRC_SOURCE_SEL_MASK (1 << 0)
	/* Command Register */
	#define CMD_REG_IDX_REG     0x40    /* register low byte */
	#define CMD_CONTROL_REG		0x41    
		#define CMD_ISSUE           (8 << 4)
		#define CMD_WRITE_REG       (0 << 4)   	/* srg=000, Write a register */
		#define CMD_WRITE_PIXEL     (1 << 4)   	/* srg=001, Write a pixel */
		#define CMD_READ_REG        (2 << 4)   	/* srg=010, Read a register */
		#define CMD_READ_PIXEL      (3 << 4)   	/* srg=011, Read a pixel */
		#define CMD_READ_STATUS     (4 << 4)   	/* srg=1xx, Read a status */
		#define CMD_18BIT           (8 << 0)
		#define CMD_DUMMY           (4 << 0)
		#define CMD_DATA_H_MASK     (0x03)		/* Data[17:16] */
	#define CMD_DATA_M_REG		0x42
	#define CMD_DATA_L_REG		0x43
	#define CMD_PAGE_REG		0x44
	#define CMD_CONFIG_REG		0x4B
		#define END_ADDRESS_EN		(1<<2)
		#define SHIFFT2LSB_MASK		(3<<0)		/* for config end address and shifft MSB 8/9bit to LSB */
	#define HGRAM_AD_END_MREG	0x4C
	#define HGRAM_AD_END_LREG	0x4D
	#define VGRAM_AD_END_MREG	0x4E
	#define VGRAM_AD_END_LREG	0x4F
	/* Read Data Retention Register */
	#define RD_COMMAND_CTRL_REG	0x46
		#define RD_CNT_MASK			0x0F    	/* 1111b, read data trans cycle mask */	
		#define DUMMY_CYCLE_MASK	(7 << 4)    /* 111b,  dummy cycle time mask */	
		#define RD_08BIT			(8 << 4)    /* write address 8bit */
	/* HDE Mask Register */
	#define HDE_MASK_REG		0x46
		#define HDE_MK_IDX_MASK		(3<<1)		/* select mask line */
		#define HDE_MASK_EN			(1<<0)		/* enable HDE mask */

			#define I80_RDWR_REG		0x22
			#define I80_HGRAM_REG       0x3E
			#define I80_VGRAM_REG       0x3E
	
	/* >>>>>>>>>>>>>>>>>>>> I2C MASTER INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
	/* I2C Master Register Map */
	#define I2CM_STATUS			0x50
	#define I2CM_MASK			0x51
	#define I2CM_SLAVE_ADDR		0x53
	#define I2CM_COMMAND		0x54
	#define I2CM_DATA0			0x55
	#define I2CM_DATA1			0x56
	#define I2CM_INSTRUCT		0x57
	
	/* I2C Master Command Define */
	#define I2CM_QUICK  			0
	#define I2CM_SINGLE				1
	#define I2CM_BYTE				2
	#define I2CM_WORD				3
	#define I2CM_GENERAL_CALL		4
	
	/* I2C Slave Write/Read bit Define */
	#define S_WRITE				0
	#define S_READ				1
	
	/* I2C Master Status Define */
	#define I2CM_BUSY  			(1<<0)
	#define FINISH_INTR			(1<<1)
	#define DEVICE_ERROR		(1<<2)
	#define BUS_ERROR			(1<<3)
	#define I2CM_FAILED			(1<<4)
	#define I2C_INUSE			(1<<5)
	#define I2CM_400K			(1<<7)
	
	/* I2C Master Setting Define */
	#define KILL_INTR  			(1<<3)
	#define I2C_INTR_EN			(1<<4)
	#define I2CM_WAIT			(1<<5)
	#define I2CM_EN				(1<<6)
	#define START_INTR			(1<<7)
	
	/* I2CMaster Return Status Define */	
	#define I2CM_FINISH			0
	#define I2CM_ERROR			1
    
    /* >>>>>>>>>>>>>>>>>>>> SOSD INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
    #define SOSD_FORCE_AB_REG         		0x80    /* Page 3 */
        #define SOSD_FORCE_EN                  	0x80
        #define FORCE_ALPHA_MAX            		0x40
	
	#define OSPI_DMA_CTRL_REG1              0x86    /* Page 3 */
        #define OSPI_DMA_GO                     0x80    // Done
        #define OSPI_XFER_CNT                   0x0C
        #define HS_LEAD_EDGE                    0x02
        #define SLUT_PTR_RES                    0x01
    #define SLUT_DATA_PORT                  0x87
    #define SOSD_HSTAR_REG1                 0x88    /* Page 3 */
    #define SOSD_HSTAR_REG2                 0x89    /* Page 3 */
        #define SOSD_HSTAR_BIT                  0x07FF
            
    #define SOSD_VSTAR_REG1                 0x8A    /* Page 3 */
    #define SOSD_VSTAR_REG2                 0x8B    /* Page 3 */
        #define SOSD_VSTAR_BIT                  0x03FF
            
    #define SOSD_HSIZE_REG1                 0x8C    /* Page 3 */
    #define SOSD_HSIZE_REG2                 0x8D    /* Page 3 */
        #define SOSD_HSIZE_BIT                  0x07FF
            
    #define SOSD_VSIZE_REG1                 0x8E    /* Page 3 */
    #define SOSD_VSIZE_REG2                 0x8F    /* Page 3 */
        #define SOSD_VSIZE_BIT                  0x03FF
            
    #define SOSD_EMU_ADDR_REG1              0x90    /* Page 3 */
    #define SOSD_EMU_ADDR_REG2              0x91    /* Page 3 */
    #define SOSD_EMU_ADDR_REG3              0x92    /* Page 3 */
        
    #define SOSD_LUT_ADDR_REG1              0x93    /* Page 3 */
    #define SOSD_LUT_ADDR_REG2              0x94    /* Page 3 */
    #define SOSD_LUT_ADDR_REG3              0x95    /* Page 3 */
        
    #define SOSD_IMG_ADDR_REG1              0x96    /* Page 3 */
    #define SOSD_IMG_ADDR_REG2              0x97    /* Page 3 */
    #define SOSD_IMG_ADDR_REG3              0x98    /* Page 3 */

	#define SOSD_SWITCH_TC_REG1				0x9A
	#define SOSD_SWITCH_TC_REG2				0x9B
		#define SOSD_TC_BIT						0x07FF
	#define SOSD_SWITCH_CTRL_REG			0x9B
		#define SOSD_TX_EN						0x80
		#define SOSD_TX_IN_V					0x40
		#define SOSD_TRIG_CS					0x20
		#define SOSD_CS_DONE					0x10
		#define SOSD_DMA_SEL					0x08
		#define SOSD_SW_TC2_BIT					0x07
		#define SOSD_LJUMP_A_REG1               0x9C    /* Page 3 */
		#define SOSD_LJUMP_A_REG2               0x9D    /* Page 3 */
			#define SOSD_LJUMP_A_BIT                0xFFFF
        
    #define SOSD_CONTROL_REG                0x9E    /* Page 3 */
        #define SOSD_EN                         0x80
        #define SOSD_EMU_TRIG                   0x40    // Done
        #define SOSD_EMU_DONE                   0x40
        #define SOSD_LUT_FRM                    0x20
        #define SOSD_LUT_ONCE                   0x10
        #define SOSD_TRIG_LOAD_PSLUT            0x08
        #define SOSD_TRIG_LOAD_SSLUT            0x04
        #define SOSD_HPERF_EN                   0x02
        #define SOSD_SHADOW_UP                  0x01
        
    #define SOSD_EMU_CTR_REG                0x9F    /* Page 3 */
        #define SOSD_EMU_EXIT                   0x80
        #define JUMP_EMU_ADDR                   0x40
        #define PAUSE_FRAME_BIT                 0x3F
            #define EMU_EXIT                        0x81

            /* >>>>>>>>>>>>>>>>>>>> SPI INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
            /* SPI For 51 */
    #define SPI_INSTRUCTION_REG         0xA0
    #define SPI_ADDRESS_REG1            0xA1
    #define SPI_ADDRESS_REG2            0xA2
    #define SPI_ADDRESS_REG3            0xA3
    #define SPI_WR_DATA_REG             0xA4
    #define SPI_COMMAND_REG             0xA5
        #define SPI_ISSUE_CMD               0x80    // Busy
        #define SPI_WR_LENGTH               0x70
        #define SPI_WR_1B                   0x40
        #define SPI_WR_3B                   0x50
        #define SPI_WR_4B                   0x60
        #define SPI_WR_N                    0x70
        #define SPI_RD_LENGTH               0x0C
        #define SPI_RD_1B                   0x04
        #define SPI_RD_3B                   0x08
        #define SPI_RD_N                    0x0C
        #define SPI_CHECK_WIP               0x02
        #define SPI_WAIT_BURST              0x01
            
    #define SPI_RD_BACK_DATA_REG1       0xA6
    #define SPI_RD_BACK_DATA_REG2       0xA7
    #define SPI_RD_BACK_DATA_REG3       0xA8
    #define SPI_DMA_ADDR_REG1           0xA9
    #define SPI_DMA_ADDR_REG2           0xAA
        #define OSDRAM_DATA_PORT            0xF0AB
    #define SPI_DMA_COUNT_REG1          0xAB
    #define SPI_DMA_COUNT_REG2          0xAC
        #define SPI_DMA_COUNT_BIT           0x1FFF
    #define SPI_DMA_CTRL_REG            0xAC
        #define SPI_DMA_GO                  0x80    // Done
        #define SPI_DMA2CHIP                0x40
        #define SPI_DMA_HOLDUP              0x20
        #define SPI_DMA_CNT_BIT             0x1F
            
    #define SPI_4X_ENABLE_REG           0xAD
        #define SPI_PRE_WREN                0x10
        #define SPI_4X_I8051                0x02
        #define SPI_4X_DMA                  0x01
    #define SPI_TOGGLE_WR_DATA          0xAE
                    
            /* SPI Instruction */
    #define WRITE_INSTRUCTION           0x83
    #define WRITE_RSR                   0xC3
    #define WRITE_RSR_WIN               0xB3
    #define READ_RSR                    0x85
    #define BULK_ERASE                  0x83
    #define SE_ERASE                    0xD3
    #define WRITE_BYTE                  0xE3
    #define READ_BYTE                   0xD5
    #define WRITE_DMA                   0x73
    #define READ_DMA                    0x6D
    #define READ_DMA_4X                 0x68
    #define DMA_WRITE                   0xA0
    #define DMA_READ                    0xE0
                        
                        
            /* Flash Instruction */
    #define WRITE_ENABLE                0x06
    #define WRITE_DISABLE               0x04
    #define READ_ID                     0x9F
    #define RS_REG                      0x05
    #define RS_REG_WIN                  0x35
    #define WS_REG                      0x01
    #define READ_DATA                   0x03
    #define FAST_READ                   0x0B
    #define FAST_READ_4X                0xEB
    #define PAGE_PROGRAM                0x02
    #define CHIP_ERASE                  0xC7    //0x60
    #define BLOCK_ERASE                 0xD8    //0x52
    #define SECTOR_ERASE                0x20            
    #define QUAD_PP                     0x38
	#define WS_REG0_GD128M              0x01
	#define WS_REG1_GD128M              0x31
	#define WS_REG2_GD128M              0x11
	#define RS_REG0_GD      			0X05
	#define RS_REG1_GD      			0X35
	#define RS_REG2_GD      			0X15
            
            /* Status Register */
    #define NO_PROTECT                  0x00
    #define QUAD_ENABLE                 0x40
    #define QUAD_ENABLE_WIN             0x02
	#define BLOCK_PROTECT               0X3C
	#define QUAD_ENABLE_GD128	    	0x02
	#define QUAD_STATUS_3_GD128	    	0x60
	#define QUAD_PROTECT_GD128          0X03
            
            /* >>>>>>>>>>>>>>>>>>>> oSPI INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
    #define OSPI_INSTRUCTION_REG        0xB0    
    #define OSPI_ADDRESS_REG1           0xB1    
    #define OSPI_ADDRESS_REG2           0xB2    
    #define OSPI_ADDRESS_REG3           0xB3    
    #define OSPI_WR_DATA_REG            0xB4    
    #define OSPI_COMMAND_REG            0xB5    
        #define OSPI_ISSUE_CMD              0x80    // Busy
        #define OSPI_WR_LENGTH              0x70
            #define OSPI_WR_2B                  0x30
            #define OSPI_WR_1B                  0x40
            #define OSPI_WR_3B                  0x50
            #define OSPI_WR_4B                  0x60
            #define OSPI_WR_N                   0x70
        #define OSPI_RD_LENGTH              0x0C
            #define OSPI_RD_1B                  0x04
            #define OSPI_RD_3B                  0x08
            #define OSPI_RD_N                   0x0C
        #define OSPI_CHECK_WIP              0x02
        #define OSPI_4X                     0x01
            
    #define OSPI_DMA_CTRL_REG2          0xB6    
        #define OSPI_DMA2SLUT               0x40
        #define OSPI_RD_MUX                 0x30
                
    #define OSPI_TOGGLE_WR_DATA         0xB7
            
    #define SOSD_CLK_SEL_REG            0xB8    
        #define OSPI_H_FREQ_CLK             0x80
        #define CLK_SOSD_INV                0x40
        #define CLK_SOSD_SEL                0x3F
        #define SOSD_CLK_SEL_DEFAULT        0x00
            
            /* SPI Arbitrator */
    #define SPI_ARB_EN_I8051            0xC0    
    #define SPI_ARB_EN_DMA              0xC1    
    #define SPI_DMA_BURST_SIZE          0xC1    
    #define SPI_HP_REG                  0xC2    
        #define SPI_HP_EN                   0x01
        #define SPI_HP_EXTRA8T              0x02

    /* >>>>>>>>>>>>>>>>>>>> SOSD INTERFACE <<<<<<<<<<<<<<<<<<<<<<<<<< */
    #define SOSD_SLUT_ADDR_REG1         0xE0    /* Page 3 */
    #define SOSD_SLUT_ADDR_REG2         0xE1    /* Page 3 */
    #define SOSD_SLUT_ADDR_REG3         0xE2    /* Page 3 */
    #define SOSD_SPRITE_ADDR_REG1       0xE3    /* Page 3 */
    #define SOSD_SPRITE_ADDR_REG2       0xE4    /* Page 3 */
    #define SOSD_SPRITE_ADDR_REG3       0xE5    /* Page 3 */
    #define SOSD_SP_LJUMP_A_REG1        0xE6    /* Page 3 */
    #define SOSD_SP_LJUMP_A_REG2        0xE7    /* Page 3 */
    #define SOSD_SP_HSTAR_REG1          0xE8    /* Page 3 */
    #define SOSD_SP_HSTAR_REG2          0xE9    /* Page 3 */
    #define SOSD_SP_VSTAR_REG1          0xEA    /* Page 3 */
    #define SOSD_SP_VSTAR_REG2          0xEB    /* Page 3 */
    #define SOSD_SP_HSIZE_REG           0xEC    /* Page 3 */
    #define SOSD_SP_VSIZE_REG1          0xED    /* Page 3 */
    #define SOSD_SP_VSIZE_REG2          0xEE    /* Page 3 */
    #define SOSD_SP_CONTROL_REG         0xEF    /* Page 3 */
        #define SOSD_SP_EN                  0x80
        #define SOSD_SP_FORCE               0x20
        #define FORCE_ALPHA_MASK            0x1f

    /* >>>>>>>>>>>>>>>>>>>> SOSD TEXTURE DECOMPRESS <<<<<<<<<<<<<<<<<<<<<<<<<< */
    #define SOSD_TD_CTRL_REG          0xD0    /* Page 3 */
        #define SOSD_TD_EN                  0x80
        #define SOSD_TD_COLOR_SPILT_MODE    0x40         
        #define SOSD_TD_SPI_GAP             0x0f
            
    #define SOSD_TD_COLOR_ADDR_REG1     0xD1    /* Page 3 */
    #define SOSD_TD_COLOR_ADDR_REG2     0xD2    /* Page 3 */
    #define SOSD_TD_COLOR_ADDR_REG3     0xD3    /* Page 3 */
    #define SOSD_TD_COLOR_HSIZE_REG1    0xD4    /* Page 3 */
    #define SOSD_TD_COLOR_HSIZE_REG2    0XD5    /* Page 3 */
#endif	/* _REG_TW_H */

