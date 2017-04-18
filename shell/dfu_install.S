/**
 *  @file   irom.S
 *  @brief  irom for t300
 *  $Id: dfu_install.S,v 1.1.1.1 2013/12/18 03:43:35 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 */

#include <config.h>
#include <regdef.h>

#define	T373_SIM			0

#define	TRAP_EJTAG			0
#define	TRAP_CONSOLE		1
#define	TRAP_DFU_MODE		2
#define	TRAP_SPI_FLOAT		3
#define	TRAP_VCO_SEL		4

#define KSEG1               0xa0000000
#define KSEG0               0x80000000

#define UART_BAUD           38400

#define	STATUS_BASE			0xb0403200

#define	S_NORMAL_BOOT		(1 << 0)
#define	S_DFU_MODE			(1 << 1)

#define UART_BASE           0xb2000000
#define AFC_UART_BASE       0xb2400000
#define UART_THR            (0x00)
#define UART_DLL            (0x00)
#define UART_DLH            (0x04)
#define UART_LCR            (0x0c)
#define UART_LSR            (0x14)
#define UART_SCR            (0x1c)

#define BOOT_STATUS         0xb04010c0
#define BOOT_ERROR0         0xb04010d0
#define BOOT_ERROR1         0xb04010e0
#define BOOT_ERROR2         0xb04010f0

#if T373_SIM
#define GPIO_BASE           0xb2800000
#define NOR_DIR				0xbe000000
#else
#define GPIO_BASE           0xb9000000
#define NOR_DIR				0xa0000000
#endif

#define LAYOUT_TEXTSIZE     0x104

#define GPIO_HWTRAP         (0x2c)
#define GPIO_SWINFO_COPY    (0x30)
#define	GPIO_OE				(0x04)
#define	GPIO_SET			(0x08)
#define	GPIO_CLEAR			(0x0c)
#define	GPIO_PINMUX			(0x50)

#define SYSMGM_BASE         0xb7c00000
#define SYSMGM_CLK_SEL      (0x100)
#define SYSMGM_CLK_DIV      (0x104)
#define SYSMGM_OP           (0x108)
#define SYSMGM_SWINFO       (0x10c)
#define SYSMGM_AHB_PLL      (0x110)
#define SYSMGM_AUD_PLL      (0x114)
#define SYSMGM_SWCONTEXT    (0x11c)
#define SYSMGM_SWRAMPAR     (0x120)

#define PHYSADDR(addr)      (addr & 0x1fffffff)

#define PHYSMASK            0x1fffffff

#define SPARE_REG_BASE		0xb0403000
#define DFUINS_DATABUF00	0x00//SPARE_REG00
#define DFUINS_DATABUF01	0x10//SPARE_REG01
#define DFUINS_DATABUF02	0x20//SPARE_REG02
#define DFUINS_DATABUF03	0x30//SPARE_REG03
#define DFUINS_CMD0			0x40//SPARE_REG04
#define DFUINS_DATABUF10	0x50//SPARE_REG05
#define DFUINS_DATABUF11	0x60//SPARE_REG06
#define DFUINS_DATABUF12	0x70//SPARE_REG07
#define DFUINS_DATABUF13	0x80//SPARE_REG08
#define DFUINS_CMD1			0x90//SPARE_REG09
#define DFUINS_DATABUFR0	0xa0//SPARE_REG10
#define DFUINS_DATABUFR1	0xb0//SPARE_REG11
#define DFUINS_DATABUFR2	0xc0//SPARE_REG12
#define DFUINS_DATABUFR3	0xd0//SPARE_REG13
#define DFUINS_STATUS		0xe0//SPARE_REG14
#define DFUINS_CMDECHO		0xf0//SPARE_REG15

#define DEVICE_ID               0x44332213 // version which receive text and resource independent
// Status
#define SV_DONE				(1<<0)
#define SV_ERROR			(1<<1)
#define SV_RESET			(1<<2)
#define SV_TIMEOUT			(1<<3)
#define SV_RESEND           (1<<4)//resend from dfui2c caused by nor read unexpected data
#define SV_RESEND2          (1<<5)//resend ask from timeout (only for linux middleware)
#define SV_RESEND3          (1<<6)//resend caused by i2c unstable

//Command Set
#define CMD_RESET 			0x00
#define CHK_DFU 			0x01
#define CHK_FINISH 			0x02
#define CHK_CRC				0x03
#define RESERVED_04			0x04
#define SET_PG_LEN			0x05
#define SET_REG_STATUS		0x06
#define ERASE_BLK			0x07
#define PUT_DATA			0x08
#define GET_ID				0x09
#define GET_REG_STATUS		0x0a
#define RESERVED_0b			0x0b
#define START_DATA_SESSION	0x0c
#define STOP_DFU_INSTALL	0x0d
#define GET_DEVID           0x0e
#define CMD_RESEND          0x0f
#define CHK_LAYOUT          0x10


#define DATAPACKET			(1<<7)
#define CMD_MASK			0x7C
#define CMD_SHIFT			2
#define TAG_MASK			0x03
#define TAG_MOD				4

#define DFU_BUF_SIZE		16384

#define DFUINS_STATE_INIT 	0
#define DFUINS_STATE_CMD 	1
#define DFUINS_STATE_DATA 	2


#if T373_SIM
#define NOR_CTRL_BASE       0xBF000000
#else
#define NOR_CTRL_BASE       0xBD800000
#endif
#define NOR_RD_PORT			(0x00)
#define NOR_WR_PORT			(0x04)
#define AUTO_RDSR_CFG		(0x08)
#define STATUS_REGISTER		(0x0c)
#define	IP_STATUS			(0x10)
#define NOR_GENERAL_CTRL	(0x14)
#define NOR_TIMING_PARA		(0x18)
#define SPI_CMD				(0x20)
#define SPI_ADDR			(0x24)
#define NOR_STATUS          (0x4c)

#define ERASE_WR_ERR_INT    (1 << 1)
#define ERASE_WR_OK_INT		(1 << 0)

#define SPI_CMD_TYPE_RD		(0 << 22)
#define SPI_CMD_TYPE_BYTEWR	(1 << 22)
#define SPI_CMD_TYPE_PAGEWR	(2 << 22)
#define SPI_CMD_TYPE_ERASE	(3 << 22)

#define AUTO_RDSR_EN		(1 << 21)
#define AUTO_RDSR_DIS		(0 << 21)

#define ADDR_CYCLE_0		(0 << 18)
#define ADDR_CYCLE_3		(3 << 18)

#define OPC_SECERASE        (0xd8 << 24)

#define CMD_SPI_BYTEWR		(SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_WREN		(SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0)
#define CMD_SPI_SECERASE    (OPC_SECERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_CHIPERASE	(SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_0)
#define CMD_SPI_WRSR		(SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1)
#define CMD_SPI_RDSR		(SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1) // can read io_port
#define CMD_SPI_RDID		(SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)

#define RESET_WRITE_POINT	0x100

#define	MASK_STATE		0x3
#define	MASK_TAG		0xc
#define	MASK_DATAIDX	0x10
#define	MASK_STATUS		0xff00
#define	MASK_CMDREG0	0xff
#define	MASK_CMDREG1	0xff00

#define	SHIFT_STATE		0
#define	SHIFT_TAG		2
#define	SHIFT_STATUS	8
#define	SHIFT_DATAIDX	4
#define	SHIFT_CMDREG0	0
#define	SHIFT_CMDREG1	8
#define SHIFT_D0_FIRST  16
#define SHIFT_CRC_ERR   17 // caused by i2c unstable
#define SHIFT_AFCUR     5  // Tarzan AFC UART TX RX Pin select, 0: afc uart

#define	SHIFT_WREN		8
#define	SHIFT_CHPERS	16
#define	SHIFT_WR		24
#define	SHIFT_WRSE		8
#define SHIFT_BUFIDX	16
#define SHIFT_WRSR		8
#define	SHIFT_CMDREG	16

#define	DFU_BUF_SIZE	16384
#define NOR_IP_STATUS	(NOR_CTRL_BASE+IP_STATUS)

.macro	wait_cmd_ready	value
	.set noreorder
1:
	li		sp,	NOR_IP_STATUS
	lw		\value, 0(sp)
	andi	\value, (1 << 1)
	beqz	\value, 1b			/* if IP_STATUS[1] != 1, goto erase_blk */
	.set reorder
.endm

#define UART_CLKDIV  (((_CONFIG_APB_CLK+(UART_BAUD*8))/(UART_BAUD*16)))
/**********************************************************************************
 * start: irom start entry
 **********************************************************************************/
    .section    .text.di
    .set reorder

/******************************************************************************
 * dfu_install:
 *	variable used in dfu_install
 *	s0 = SPARE_REG_BASE
 *	s1 = bufidx | cmdreg1 | cmdreg0
 *		(s1[31:16]=bufidx, s1[15:8]=cmdreq1, s1[7:0]=cmdreq0)
 *	s2 = d0_first | status | dataidx | tag | state
 *		(s2[16]=d0_first, s2[15:8]=status, s2[4]=dataidx, s2[3:2]=tag, s2[1:0]=state)
 *	s3 = data1
 *	s4 = dfu_size | write_size
 *		(s4[31:16]=dfu_size, s4[15:0]=write_size)
 * 	s5 = addr_ptr
 *	s6 = wr | chpers | wren | rdid
 *		(s6[31:24]=wr, s6[23:16]=chpers, s6[15:8]=wren, s6[7:0]=rdid)
 *	s7 = cmdreg | wrsr | rdsr
 *		(s7[23:16]=cmdreg, s7[15:8]=wrsr, s7[7:0]=rdsr)
 *  a0 = cmd
 *  a1 = cmdreg0 or cmdreg1
 *	t8 = tor
 *  t9 = data0
 ******************************************************************************/
dfu_install:
	la		a0, msgDFUMODE
	bal		puts_di

dfu_spare_reg_reset:
    /* init dfu install spare register*/
    li      t0, SPARE_REG_BASE
    sb      zero, DFUINS_DATABUF00(t0)
    sb      zero, DFUINS_DATABUF01(t0)
    sb      zero, DFUINS_DATABUF02(t0)
    sb      zero, DFUINS_DATABUF03(t0)
    sb      zero, DFUINS_DATABUF10(t0)
    sb      zero, DFUINS_DATABUF11(t0)
    sb      zero, DFUINS_DATABUF12(t0)
    sb      zero, DFUINS_DATABUF13(t0)
    sb      zero, DFUINS_DATABUFR0(t0)
    sb      zero, DFUINS_DATABUFR1(t0)
    sb      zero, DFUINS_DATABUFR2(t0)
    sb      zero, DFUINS_DATABUFR3(t0)
    sb      zero, DFUINS_STATUS(t0)
    sb      zero, DFUINS_CMDECHO(t0)
    li      t1, 0xff
    sb      t1, DFUINS_CMD0(t0)
    sb      t1, DFUINS_CMD1(t0)

dfu_nor_init:
	li		t0, S_DFU_MODE
	li		t1, STATUS_BASE
	sb		t0, 0(t1)
	/* config nor RDSR */
	li		t0, NOR_CTRL_BASE
	li		t1, 0x05020038
	sw		t1, AUTO_RDSR_CFG(t0)	

	li		s0, SPARE_REG_BASE
	/* init variable */	
	move	s1, zero
	li      s2, 0x10010				/* set dataidx = 1, d0_first = 1 */
	move	s3, zero
	move	s4, zero
	move	s5, zero
	move	s6, zero
	move	s7, zero
	move	t8, zero
#if 0 // set gpio18 pinmux for debug	
	li		t0, GPIO_BASE
	li		t1, 0x288000
	sw		t1, GPIO_PINMUX(t0)
	li		t1, 0x8040001
	sw		t1, GPIO_OE(t0)
#endif	
	
main_loop:
	lbu		t1, DFUINS_CMD0(s0)		/* t1(cmdreg0) */	
	lbu		t2, DFUINS_CMD1(s0)		/* t2(cmdreg1) */
	li		t0, 0xff
	bne		t0, t1, begin	
	bne		t0, t2, begin
	b		main_loop
	
begin:
	move	s3, zero
	sll		t2, 8
	or		t2, t1					/* t2 = cmdreg1 | cmdreg0 */
	li		t0, 0xffff0000
	and		s1, t0
	or		s1, t2					/* s1 = bufidx | cmdreg1 | cmdreg0 */

	li		t0, DFUINS_STATE_DATA
	andi	t1, s2, MASK_STATE
	beq		t0, t1, data_state
	
cmd_state:	
	andi	t4, s1, MASK_CMDREG1
	srl		t4, SHIFT_CMDREG1		/* t4 = cmdreg1 */
	andi	t1, t4, TAG_MASK		/* t1(tag1) = cmdreg1 & 0x3 */
	
	andi	t0, s2, MASK_TAG
	srl		t0, SHIFT_TAG			/* t0 = tag */
	addi	t2, t0, 1				/* t2 = tag + 1 */
	andi	t2, 0x3					/* t2 = (tag + 1) & 0x3 */
	
	beq		t2, t1, 2f				/* if ((tag + 1) & 0x3 != tag1), goto main_loop */
	addi	t8, 1					/* tor += 1 */
	b		main_loop
2:	
	lbu		t1, DFUINS_CMD1(s0)		/* t1 = cmdreg1_chk */
	beq		t4, t1, 3f				/* if cmdreg1 != cmdreg1_chk, goto main_loop */
	b		main_loop
3:		
#if 1	// for debug
	move	sp, t4
	
	la		a0, msgCMDREG1
	bal		puts_di

	move	a0, sp
	bal		putreg
	
	la		a0, msgGETCMD
	bal		puts_di

	andi	a0, sp, CMD_MASK
	srl		a0, CMD_SHIFT			/* a0 = cmd */
	bal		putreg
	
	la		a0, msgTAG
	bal		puts_di
	andi	a0, sp, TAG_MASK		/* t1(tag) = cmdreg1 & 0x3 */
	bal		putreg
	
	la		a0, msgTOR
	bal		puts_di
	move	a0, t8
	bal		putreg
	
	move	t4, sp
#endif
	move	t8, zero

	andi	a0, t4, CMD_MASK
	srl		a0, CMD_SHIFT			/* a0 = cmd */
	move	a1, t4					/* a1 = cmdreg1 */
	andi	t1, t4, TAG_MASK		/* t1(tag) = cmdreg1 & 0x3 */
	sll		t1, SHIFT_TAG	
	li		t0, (1 << SHIFT_DATAIDX)
	li		t2, ~(0x1c)
	and		s2, t2 
	or		s2, t0, t1				/* update tag, dataidx */
	
	move	s3, zero
	lbu		t0, DFUINS_DATABUF10(s0)
	or		s3, t0
	lbu		t0, DFUINS_DATABUF11(s0)
	sll		t0, 8
	or		s3, t0
	lbu		t0, DFUINS_DATABUF12(s0)
	sll		t0, 16
	or		s3, t0
	lbu		t0, DFUINS_DATABUF13(s0)
	sll		t0, 24
	or		s3, t0
	b		lookup_cmd

data_state:	
	andi	t0, s2, MASK_DATAIDX
	srl		t0, SHIFT_DATAIDX		/* t0 = dataidx */
	bnez	t0, dataidx1
dataidx0:
	andi	t4, s1, MASK_CMDREG1
	srl		t4, SHIFT_CMDREG1		/* t4 = cmdreg1 */
	andi	t1, t4, TAG_MASK		/* t1(tag1) = cmdreg1 & 0x3 */
	
	andi	t0, s2, MASK_TAG
	srl		t0, SHIFT_TAG			/* t0 = tag */
	addi	t2, t0, 1				/* t2 = tag + 1 */
	andi	t2, 0x3					/* t2 = (tag + 1) & 0x3 */
	
	beq		t2, t1, 2f				/* if ((tag + 1) & 0x3 != tag1), goto main_loop */
	addi	t8, 1					/* tor += 1 */
	b		main_loop
2:
	lbu		t1, DFUINS_CMD1(s0)		/* t1 = cmdreg1_chk */
	beq		t4, t1, 3f				/* if cmdreg1 != cmdreg1_chk, goto main_loop */
	addi	t8, 1					/* tor += 1 */
	b		main_loop
3:

#if 0 // gpio18 toggle for debug	
	li		t0, GPIO_BASE
	li		t1, 0x40000
	li		t2, (1 << 31)
	and		t3, s2, t2
	bnez	t3, clear_gpio
set_gpio0:	
	sw		t1, GPIO_SET(t0)
	li		t0, (1 << 31)
	or		s2, t0
	b		start_idx0
clear_gpio0:	
	sw		t1, GPIO_CLEAR(t0)
	li		t0, ~(1 << 31)
	and		s2, t0
start_idx0:		
#endif
	move	t8, zero

	andi	a0, t4, CMD_MASK
	srl		a0, CMD_SHIFT			/* a0 = cmd */
	move	a1, t4					/* a1 = cmdreg1 */
	andi	t1, t4, TAG_MASK		/* t1(tag) = cmdreg1 & 0x3 */


	sll		t1, SHIFT_TAG	
	li		t0, (1 << SHIFT_DATAIDX)
	li		t2, ~(0x1c)	
	and		s2, t2 
	or		s2, t0
	or		s2, t1					/* update tag, dataidx */	
	
	move	s3, zero
	lbu		t0, DFUINS_DATABUF00(s0)
	or		s3, t0
	lbu		t0, DFUINS_DATABUF01(s0)
	sll		t0, 8
	or		s3, t0
	lbu		t0, DFUINS_DATABUF02(s0)
	sll		t0, 16
	or		s3, t0
	lbu		t0, DFUINS_DATABUF03(s0)
	sll		t0, 24
	or		s3, t0
	move	t9, s3
	
	move	s3, zero
	lbu		t0, DFUINS_DATABUF10(s0)
	or		s3, t0
	lbu		t0, DFUINS_DATABUF11(s0)
	sll		t0, 8
	or		s3, t0
	lbu		t0, DFUINS_DATABUF12(s0)
	sll		t0, 16
	or		s3, t0
	lbu		t0, DFUINS_DATABUF13(s0)
	sll		t0, 24
	or		s3, t0
	
	/* the second 16KB data needs switch for every 8 byte */
	li      t0, (1 << SHIFT_D0_FIRST)
	and     t2, s2, t0
	bnez    t2, 1f
	move    t0, t9
	move    t9, s3
	move    s3, t0
1:
	
#if 0 // gpio18 toggle for debug	
	li		t0, GPIO_BASE
	li		t1, 0x40000
	li		t2, (1 << 31)
	and		t3, s2, t2
	bnez	t3, clear_gpio
set_gpio:	
	sw		t1, GPIO_SET(t0)
	li		t0, (1 << 31)
	or		s2, t0
	b		lookup_cmd		
clear_gpio:	
	sw		t1, GPIO_CLEAR(t0)
	li		t0, ~(1 << 31)
	and		s2, t0
#endif
	b		lookup_cmd		
	
dataidx1:
#if 0
	move	sp, a0
	
	la		a0, msgDATAIDX1
	bal		puts_di
	
	move	a0, sp
#endif
	andi	t4, s1, MASK_CMDREG0
	srl		t4, SHIFT_CMDREG0		/* t4 = cmdreg0 */
	andi	t1, t4, TAG_MASK		/* t1(tag0) = cmdreg0 & 0x3 */
	
	andi	t0, s2, MASK_TAG
	srl		t0, SHIFT_TAG			/* t0 = tag */
	addi	t2, t0, 1				/* t2 = tag + 1 */
	andi	t2, 0x3					/* t2 = (tag + 1) & 0x3 */
	
	beq		t2, t1, 2f				/* if ((tag + 1) & 0x3 != tag0), goto main_loop */
	addi	t8, 1					/* tor += 1 */
	b		main_loop
2:	
	lbu		t1, DFUINS_CMD0(s0)		/* t1 = cmdreg0_chk */
	beq		t4, t1, 3f				/* if cmdreg0 != cmdreg0_chk, goto main_loop */
	b		main_loop
3:		
	move	t8, zero

	andi	a0, t4, CMD_MASK
	srl		a0, CMD_SHIFT			/* a0 = cmd */
	move	a1, t4					/* a1 = cmdreg0 */
	andi	t1, t4, TAG_MASK		/* t1(tag) = cmdreg0 & 0x3 */
	sll		t1, SHIFT_TAG	
	li		t2, ~(0x1c)
	and		s2, t2 
	or		s2, t1					/* update tag, dataidx */
	
	move	s3, zero
	lbu		t0, DFUINS_DATABUF00(s0)
	or		s3, t0
	lbu		t0, DFUINS_DATABUF01(s0)
	sll		t0, 8
	or		s3, t0
	lbu		t0, DFUINS_DATABUF02(s0)
	sll		t0, 16
	or		s3, t0
	lbu		t0, DFUINS_DATABUF03(s0)
	sll		t0, 24
	or		s3, t0
	b		lookup_cmd				

lookup_cmd:
	li		t0, ~(0xff0000)
	and		s7, t0
	
	sll		t0, a1, SHIFT_CMDREG
	or		s7, t0

	la		t0, cmd_table
	li		t3, 0xff
1:
	lbu		t2, 0(t0)

	srl		t5, s7, SHIFT_CMDREG
	andi	t5, 0xff
	andi	t5, CMD_MASK
	srl		t5, CMD_SHIFT			/* t5 = cmd */
	beq		t5, t2, find_cmd
	beq		t3, t2, cmd_invalid
	addi	t0, 5
	b		1b
find_cmd:
	ulw		t1, 1(t0)
	jal		t1						/* cmd_service entry */
	beqz	v0, cmd_done
	
cmd_done:	
	srl		t0, s2, SHIFT_STATUS
	sb		t0, DFUINS_STATUS(s0)	
	srl		t0, s7, SHIFT_CMDREG
	sb		t0, DFUINS_CMDECHO(s0)
	b		main_loop
	
cmd_invalid:
	srl		t0, s2, SHIFT_STATUS
	sb		t0, DFUINS_STATUS(s0)	
	srl		t0, s7, SHIFT_CMDREG
	sb		t0, DFUINS_CMDECHO(s0)
	
	move	sp, a0
	la		a0, msgErrDFUCMD
	bal		puts_di
	move	a0, sp
	bal		putreg
	b		main_loop
	
/**
 * putc($a0 = char)
 *   register: k0, k1 will be destoried
 */

putc:
    li      k0, 0xb9000028  /* PIN_PAD GPIO28*/
    lw      k0, 0(k0)
    andi    k0, (1 << SHIFT_AFCUR)    /*afc uart related tx rx  pin, may different for different project*/
    beqz    k0, afc_uart
    li      k0, GPIO_BASE
    lbu     k0, GPIO_HWTRAP(k0)
    //andi    k0, (1 << TRAP_CONSOLE)        /* bit1 0 disable character output */
    //beqz    k0, 3f
    li      k0, UART_BASE
    lbu     k0, UART_SCR(k0)    /* SCR[0] == 1, UART timeout mark */
    li	    k1, 6000
    beqz    k0, 1f              /* if ! timeout ? */    
    li	    k1, 200
1:  sub     k1, 1
    beqz    k1, 4f              /* timeout ? */
    li      k0, UART_BASE
    lbu     k0, UART_LSR(k0)    
    andi    k0, 0x40            /* until transmitter emtpy */
    beqz    k0, 1b
    li      k0, UART_BASE
    sb      zero, UART_SCR(k0)  /* clear timeout mark */
2:  sb      a0, UART_THR(k0)
3:  jr      ra

4:  li      k1, 1               /* mark timeout */
    li      k0, UART_BASE
    sb      k1, UART_SCR(k0)
    b       2b
afc_uart:
    li      k0, GPIO_BASE
    lbu     k0, GPIO_HWTRAP(k0)
    //andi    k0, (1 << TRAP_CONSOLE)        /* bit1 0 disable character output */
    //beqz    k0, 3f
    li      k0, AFC_UART_BASE
    lbu     k0, UART_SCR(k0)    /* SCR[0] == 1, UART timeout mark */
    li      k1, 6000
    beqz    k0, 1f              /* if ! timeout ? */
    li      k1, 200
1:  sub     k1, 1
    beqz    k1, 4f              /* timeout ? */
    li      k0, AFC_UART_BASE
    lbu     k0, UART_LSR(k0)
    andi    k0, 0x40            /* until transmitter emtpy */
    beqz    k0, 1b
    li      k0, AFC_UART_BASE
    sb      zero, UART_SCR(k0)  /* clear timeout mark */
2:  sb      a0, UART_THR(k0)
3:  jr      ra

4:  li      k1, 1               /* mark timeout */
    li      k0, AFC_UART_BASE
    sb      k1, UART_SCR(k0)
    b       2b


/**
 * putreg($a0 = reg)
 *   register: t6, t3, t2, t1, t0 will be destoried 
 */
putreg:
    move    t6, ra
    move    t0, a0
    li      t2, 10
    li      t3, 32
1:  sub     t3, 4
    srl     t1, t0, t3
    and     t1, 0x0f
    add     a0, t1, '0'
    blt     t1, t2, 2f
    add     a0, t1, 'A' - 10
2:  bal     putc
    bnez    t3, 1b
    jr      t6


/******************************************************************************
 * norwrite:
 *	input a2: cur_bufidx
 *
 *	register: t0 ~ t6 will be destoried
 * 			  s5 will be updated	
 * 		t0 = NOR_CTRL_BASE
 *  	t1 = tmp
 *  	t2 = cur_bufidx
 *  	t3 = nor_data_buf + cur_bufidx
 *  	t4 = addr of nor_data_buf
 *  	t5 = tmp 
 *  	t6 = cur_bufidx + 1024
 *  	s5 = addr_ptr 
 ******************************************************************************/
norwrite:
	li		t0, NOR_CTRL_BASE	
	la		t4, nor_data_buf
	addi	t6, a2, 1024				/* t6 = cur_bufidx + 1024 */
	
__write:
	wait_cmd_ready	t3	
	li		t0, NOR_CTRL_BASE	
	la		t4, nor_data_buf	
	wait_cmd_ready	t3
	/* reset buffer pointer */
	li		t1, RESET_WRITE_POINT
	sw		t1,	NOR_GENERAL_CTRL(t0)
	addi	t1, a2, 32					/* t1 = cur_bufidx + 32 */		
1:	
	add		t3, a2, t4					/* t3 = nor_data_buf + cur_bufidx */	
	lw		t5, 0(t3)
	sw		t5, NOR_WR_PORT(t0)
	addi	a2, 4
	blt		a2, t1, 1b					/* if cur_bufidx < (cur_bufidx+32), goto 1b */		
	wait_cmd_ready	t3
	
	/* wren cmd */
	li		t0, NOR_CTRL_BASE
	srl		t1, s6, SHIFT_WREN 
	sll		t1, 24						/* t1 = wren << 24 */
	li		t5, CMD_SPI_WREN
	or		t1, t5
	sw		t1, SPI_CMD(t0)
	wait_cmd_ready	t3
	
	/* write pa(addr) */
	li		t0, NOR_CTRL_BASE	
	sw		s5, SPI_ADDR(t0)

	/* wr cmd */
	li		t0, NOR_CTRL_BASE
	srl		t1, s6, SHIFT_WR
	sll		t1, 24						/* t1 = wr << 24 */	
	li		t5, CMD_SPI_BYTEWR
	or		t5, t1
	li		t1, 32
	or		t5, t1						/* t5 = wr << 24 | CMD_SPI_BYTEWR | wlen */
	sw		t5, SPI_CMD(t0)	
#if 1
	li		t0, NOR_CTRL_BASE	
1:
	lw		t1, NOR_STATUS(t0)
	andi	t1, 0x3
	beqz	t1, 1b			/* if IP_STATUS[1] != 1, goto erase_blk */
	
	li		t0, NOR_CTRL_BASE	
	lw		t1, NOR_STATUS(t0)
	sw		t1, NOR_STATUS(t0)	
#endif
	
mydebug:
	//li		t1, 32
	//add		s5, t1						/* addr_ptr += 32 */
	addi		s5, 32
	
	blt		a2, t6, __write
	jal		ra

#if 0
/**
 * flush_cache_all
 * t0, t1, t2, t3 will destoried
 */

flush_cache_all:
    li      t3, KSEG0
    li      t2, 4           /* 4 ways */
    mtc0    zero, $28       /* TagLo = 0 */
2:  move    t0, t3          /* t0 = start */
    add     t1, t0, 2048    /* t1 = 2K end */
1:  cache   0x00, 00(t0)    /* Index Invalidate I */
    cache   0x00, 16(t0)
    cache   0x00, 32(t0)
    cache   0x00, 48(t0)
    cache   0x09, 00(t0)    /* Index Store Tag D */
    cache   0x09, 16(t0)
    cache   0x09, 32(t0)
    cache   0x09, 48(t0)
    add     t0, 64
    blt     t0, t1, 1b
    add     t3, 2048        /* next way */
    sub     t2, 1
    bnez    t2, 2b
    jr      ra

get_ver:
    la      v0, IROM_Version
    jr      ra

get_bdate:
    la      v0, IROM_Build_Date
    la      v1, IROM_Build_Time
    jr      ra

get_id:
    la      v0, Chip_ID
    la      v1, Chip_Revision
    jr      ra
#endif

/**
* puts_di($a0 = pointer of string)
*   register: t6, t0 will be destoried
*/
puts_di:
    move    t6, ra
    move    t0, a0
1:  lbu     a0, 0(t0)
    beqz    a0, 2f
    bal     putc
    addu    t0, 1
    b       1b
2:
    jr      t6
    
/******************************************************************************
 * norread:
 *	register: t0 ~ t6 will be destoried
 * 		t0 = NOR_DIR
 *  	t1 = idx
 *  	t2 = idx + NOR_DIR
 *  	t3 = value
 *  	t4 = addr of nor_data_buf
 *  	t5 = idx + nor_data_buf
 *		t6 = tmp
 ******************************************************************************/
norread:
	move	t1, zero	
	la		t0, NOR_DIR
	move	t2, s5						/* t2 = addr_ptr */
	li		t6, DFU_BUF_SIZE
	sub		t2, t6						/* t2 -= 16KB */
	add		t0, t2
	la		t4, nor_data_buf
1:	
	add		t2, t0, t1
	lw		t3, 0(t2)	
	add		t5, t1, t4
	sw		t3, 0(t5)
	addi	t1, 4
	blt		t1, DFU_BUF_SIZE, 1b
	jal		ra

/********************************************************************************
* set_spare_reg :
*    input  a0 == status
*    output none
*    affected register, t0, t1
********************************************************************************/
set_spare_reg:
    li      t0, BOOT_STATUS
    srl     t1, a0, 8
    sb      t1, 0x10(t0)
    srl     t1, a0, 16
    sb      t1, 0x20(t0)
    srl     t1, a0, 24
    sb      t1, 0x30(t0)
    sb      a0, 0x00(t0)

    jr      ra

/******************************************************************************
 * cmd_srv	0x00, cmd_reset
 * cmd_srv	0x01, chk_dfu
 * cmd_srv	0x02, chk_finish
 * cmd_srv	0x03, chk_crc
 * cmd_srv	0x04, reserved_04
 * cmd_srv	0x05, set_pg_len
 * cmd_srv	0x06, set_reg_status
 * cmd_srv	0x07, erase_blk
 * cmd_srv	0x08, put_data
 * cmd_srv	0x09, get_norid
 * cmd_srv	0x0a, get_reg_status
 * cmd_srv	0x0b, reserved_0b
 * cmd_srv	0x0c, start_data_session
 * cmd_srv	0x0d, stop_dfu_install
 ******************************************************************************/    
cmd_reset:
	li		s4, SV_DONE	
	move	v0, zero
	jr		ra

chk_dfu:
	move	s6, s3
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0	
	
	move	v0, zero
	jr		ra
	
chk_finish:
	/* clear status */
	li		t0, ~(0xff00)
	and		s2, t0
	
	andi	t0, s4, 0xffff			/* t0 = write_size */
	srl		t1, s4, 16				/* t1 = dfu_size */
	beq		t0, t1, __finish
	/* set status */	
#if 1	// for debug
	move	sp, ra
	move	a2, a0
	
	la		a0, msgFINISHNG
	bal		puts_di
	move	a0, s4
	bal		putreg
	
	move	a0, a2
	move	ra, sp	
#endif
	li		t0, 0xff
	sb		t0, DFUINS_CMD0(s0)		/* t1(cmdreg0) */	
	sb		t0, DFUINS_CMD1(s0)		/* t2(cmdreg1) */	
	
	li		t0, SV_ERROR	
	sb		t0, DFUINS_STATUS(s0)	
	srl		t0, s7, SHIFT_CMDREG
	sb		t0, DFUINS_CMDECHO(s0)
	
	j		dfu_install
__finish:
#if 1	// for debug
	move	sp, ra
	move	a2, a0
	
	la		a0, msgFINISHOK
	bal		puts_di
	move	a0, s4
	bal		putreg
	
	move	a0, a2
	move	ra, sp	
#endif	
	li		t0, DFUINS_STATE_CMD
	li		t1, ~(0x3)
	and		s2, t1
	or		s2, t0
	/* set status */
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0	
	
	move	v0, zero
	jr		ra
	
chk_crc:
	move	a2, zero	
	move	a3, ra	

/* calculate crc before nor write -->*/

    /* crc check */
    la      t0, nor_data_buf        /* start */
    li      t1, DFU_BUF_SIZE
    add     t5, t0, t1              /* end */
    move    t1, zero                /* index */
    move    t3, zero                /* value */
1:
    add     t2, t0, t1
    lw      t4, 0(t2)
    xor     t3, t3, t4
    add     t1, 4
    blt     t1, DFU_BUF_SIZE, 1b

    #if 0 // for debug
    move    sp, ra

    la      a0, msgCRC
    bal     puts_di

    la      a0, msgEMPTY
    bal     puts_di

    move    a0, t3
    bal     putreg

    move    ra, sp
    #endif
#if 0 // for emulating i2c unstable test
    li      t2, (1 << 17)   // 128K
    and     t1, s5, t2
    beqz    t1, 1f
    li      t2, (1 << 18)   // err make bit
    and     t4, s2, t2
    bnez    t4, 1f
    addi    t3, 1
    add     s2, t2
1:
#endif
	/* the location of crc_ap value is on d0 or d1, so need switch */	
	li      t0, (1 << SHIFT_D0_FIRST)
	and     t2, s2, t0
	bnez    t2, 2f
	move    s3, t9
2:
    beq     t3, s3, __norwrite1k        /* s3 = crc_ap, v0 = xor_result */
    li      t0, 1 << SHIFT_CRC_ERR      /* crc_err caused by i2c unstable */
    or      s2, t0

/* calculate crc before nor write <--*/

	/* nor write */	
__norwrite1k:
	bal		norwrite

	move	ra, a3	
	srl		t0, s1, SHIFT_BUFIDX		/* t0 = bufidx */
	blt		a2, t0, __norwrite1k		/* if (t1 < bufidx), goto __norwrite1k */
	
	/* update write_size */
	srl		t0, s1, SHIFT_BUFIDX		/* t0 = bufidx */
	srl		t0, 10						/* t0 = bufidx/1024 */
	add		s4, t0						/* s4 += bufidx/1024 */
	
	/* clear nor_data_buf */
	la		t0, nor_data_buf
	move	t1, zero					/* t1 = index */	
1:
	add		t2, t1, t0
	sw		zero, 0(t2)
	addi	t1, 4		
	blt		t1, DFU_BUF_SIZE, 1b
	
__crc_chk:

	/* nor read 16KB */
	bal		norread
	move	ra, a3
	
	/* clear status */
	li		t0, ~(0xff00)
	and		s2, t0

	/* crc check */
	la		t0, nor_data_buf		/* start */
	li		t1, DFU_BUF_SIZE
	add		t5, t0, t1				/* end */
	move	t1, zero				/* index */	
	move	t3, zero				/* value */
1:
	add		t2, t0, t1
	lw		t4, 0(t2)
	xor		t3, t3, t4
	add		t1, 4
	blt		t1, DFU_BUF_SIZE, 1b	

#if 1 // for debug
	move	sp, ra
	move	a1, a0
	move	t9, t3
	move	t8, t1
	
	la		a0, msgCRC
	bal		puts_di
	move	a0, s3
	bal		putreg

	la		a0, msgEMPTY
	bal		puts_di
	move	a0, t9
	bal		putreg
	
	la		a0, msgEMPTY
	bal		puts_di
	move	a0, s5
	bal		putreg

	la		a0, msgEMPTY
	bal		puts_di
	move	a0, s2
	bal		putreg
	
	move	t3, t9
	move	a0, a1
	move	ra, sp
#endif	

#if 0 // for emulating nor write unstable test
    li      t2, (1 << 17)   // 128K
    and     t1, s5, t2
    beqz    t1, 1f
    li      t2, (1 << 18)   // err make bit
    and     t4, s2, t2
    bnez    t4, 1f
    addi    t3, 1
    add     s2, t2
1:
#endif
	bne		t3, s3, chk_err				/* s3 = crc_ap, v0 = xor_result */

#if 0   // for emulating i2c unstable test
    li      t0, (1 << SHIFT_CRC_ERR)
    and     t1, s2, t0
    bnez    t1, chk_err
#endif

    /* clear bufidx */
	andi    s1, 0xffff
chk_ok:	
#if 0 // for debug
	move	sp, ra
	
	la		a0, msgCRCOK
	bal		puts_di
	
	move	ra, sp
#endif
	/* set status */
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	li      t2, (1 << SHIFT_D0_FIRST)
	and     t1, s2, t2
	add     t1, t2                          /*  ...                     */

	and     t1, t2                          /* d0_first = (d0_first+1)%2*/
	or      s2, t0	
	and     s2, ~(1 << SHIFT_D0_FIRST)
	or      s2, t1
	move	v0, zero

	jr		ra
chk_err:
#if 1 // for debug
	move	sp, ra
	
	la		a0, msgCRCERR
	bal		puts_di
	
	move	ra, sp
#endif	
	li      t2, (1 << SHIFT_D0_FIRST)
	and     t1, s2, t2
	add     t1, t2                          /* ...                      */
	and     t1, t2                          /* d0_first = (d0_first+1)%2*/
	and     s2, ~(1 << SHIFT_D0_FIRST)
	or      s2, t1

    li      t0, SV_RESEND
    sll     t0, SHIFT_STATUS
    or      s2, t0

    and     t2, s2, (1 << SHIFT_CRC_ERR)    /* crc_err caused by i2c unstable */
    beqz    t2, 1f
    and     s2, ~(1 << SHIFT_CRC_ERR)
    li      t0, SV_RESEND3                  /* cause by i2c unstable */
    sll     t0, SHIFT_STATUS
    and     s2, ~(0xff << 8)                /* clear status, s2[15:8] */
    or      s2, t0
1:
    /*  update s1:addr_ptr, s4[15:0]:write_size*/
                                            /*  write_size - bufidx/1024*/
    srl     t0, s1, SHIFT_BUFIDX            /* t0 = bufidx */
    sub     s5, t0                          /* addr_ptr -= bufidx */
    and     s5, 0xffff0000                  /* 64 k alignment     */
    srl     t0, 10                          /* t0 = bufidx/1024  */
    sub     s4, t0                          /* s4 -= bufidx/1024 */
    and     s4, 0xffffffc0                  /* 64 k alignment    */
    andi    s1, 0xffff                      // clear bufidx

    /* erase 1 block for resend data */
    move    t7, ra
    move    a0, s5
    srl     a0, 10                          /* a0=erase start address k byte unit*/
    bal     erase_blk2
    move    ra, t7

    move    v0, zero
    jr      ra
	
reserved_04:	/* no use */
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra
	
set_pg_len:
	andi	t0, s3, 0xffff
	sll		t0, 16
	li		t1, ~(0xffff)
	and		s4, t1
	or		s4, t0
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra
	
set_reg_status:
	li		t0, NOR_CTRL_BASE
	li		t1, RESET_WRITE_POINT
	sw		t1, NOR_GENERAL_CTRL(t0)
	move	t1, zero
	sw		t1, NOR_WR_PORT(t0)
	wait_cmd_ready	t3
	
	/* wrsr cmd */
	srl		t1, s7, SHIFT_WRSR
	sll		t1, 24
	li		t2, CMD_SPI_WRSR
	or		t1, t2
	sw		t1, SPI_CMD(t0)
	wait_cmd_ready	t3
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra

/*erase blk: block(64k) erase*/    
#if 1
erase_blk:
    // yc modify for program test or resource only
    li      t0, 0xffff
    and     t1, s3, t0
    move    t4, t1            /* t4 = block erase addr (k byte unit) = program start address */

    li      t0, 0xffff0000
    and     s4, t0            /* clear write_size*/
    or      s4, t1            /* write_size = program start address for only progream resource case*/

    move    s5, t1            /* data write start address */
    sll     s5, 10            

    //move    t4, zero        /* t4 = block erase addr (k byte unit)*/
#if 1   // for debug
    move    sp, ra

    la      a0, msgBLOCKERASE
    bal     puts_di

    move    ra, sp
    li      t0, NOR_CTRL_BASE
#endif

3:
    wait_cmd_ready  t3

#if 1   // for debug
    move    sp, ra

    la      a0, msgNORWRITE
    bal     puts_di

    move    ra, sp
    li      t0, NOR_CTRL_BASE
#endif


    /* wren cmd */
    li      t0, NOR_CTRL_BASE
    srl     t1, s6, SHIFT_WREN
    sll     t1, 24
    li      t2, CMD_SPI_WREN
    or      t1, t2
    sw      t1, SPI_CMD(t0)

    wait_cmd_ready  t3

    sll     t5, t4, 10              /* pa = t4 * 1024 */
    sw      t5, SPI_ADDR(t0)        /* SPI_ADDR(t0) = pa */

    /* clear Reg 0x4c[1:0] */
    li      t1, 0x3
    sw      t1, NOR_STATUS(t0)

    /* block erase cmd */
    li      t1, CMD_SPI_SECERASE
    sw      t1, SPI_CMD(t0)

2:
    lw      t1, NOR_STATUS(t0)      /* st = t1 */
    andi    t1, 0x1
    li      t3, 0x1
    bne     t1, t3, 2b

    add     t4, 64                  /* t4 += 64 (k) */
    srl     t5, s4, 16              /* t5 = dfu_size */
    blt     t4, t5, 3b              /* if(pa < dfu_size) goto 3b*/

    /* set status */
    li      t0, ~(0xff00)
    and     s2, t0
    li      t0, SV_DONE
    sll     t0, SHIFT_STATUS
    or      s2, t0

    move    v0, zero
    jr      ra
#endif

/*erase blk: block(64k) erase*/    
#if 1
erase_blk2:
    move    t4, a0            /* t4 = block erase addr (k byte unit), a0 = init block erase size*/

#if 1   // for debug
    move    sp, ra

    la      a0, msgBLOCKERASE2
    bal     puts_di

    move    ra, sp
    li      t0, NOR_CTRL_BASE
#endif

3:
    wait_cmd_ready  t3

#if 0   // for debug
    move    sp, ra

    la      a0, msgNORWRITE
    bal     puts_di

    move    ra, sp
    li      t0, NOR_CTRL_BASE
#endif

#if 1
    /* wren cmd */
    li      t0, NOR_CTRL_BASE
    srl     t1, s6, SHIFT_WREN
    sll     t1, 24
    li      t2, CMD_SPI_WREN
    or      t1, t2
    sw      t1, SPI_CMD(t0)

    wait_cmd_ready  t3

    sll     t5, t4, 10              /* pa = t4 * 1024 */
    sw      t5, SPI_ADDR(t0)        /* SPI_ADDR(t0) = pa */

    /* clear Reg 0x4c[1:0] */
    li      t1, 0x3
    sw      t1, NOR_STATUS(t0)

    /* block erase cmd */
    li      t1, CMD_SPI_SECERASE
    sw      t1, SPI_CMD(t0)

2:
    lw      t1, NOR_STATUS(t0)      /* st = t1 */
    andi    t1, 0x1
    li      t3, 0x1
    bne     t1, t3, 2b
#endif
    jr      ra
#endif

/**/
#if 0
erase_blk:
	wait_cmd_ready	t3
	
	/* wren cmd */
	li		t0, NOR_CTRL_BASE
	srl		t1, s6, SHIFT_WREN
	sll		t1, 24
	li		t2, CMD_SPI_WREN
	or		t1, t2
	sw		t1, SPI_CMD(t0)
	
	wait_cmd_ready	t3
	
	/* clear Reg 0x4c[1:0] */
	li		t1, 0x3
	sw		t1, NOR_STATUS(t0)
	
	/* chip erase cmd */
	srl		t1, s6, SHIFT_CHPERS
	sll		t1, 24
	li		t2, CMD_SPI_CHIPERASE
	or		t1, t2
	sw		t1, SPI_CMD(t0)
#if 1	// for debug
	move	sp, ra
	move	a2, a0
	
	la		a0, msgCHIPERASE
	bal		puts_di
	
	move	a0, a2
	move	ra, sp
	li		t0, NOR_CTRL_BASE
#endif	
	
2:	
	lw		t1, NOR_STATUS(t0)
	lw		t2, IP_STATUS(t0)
	andi	t1, 0x1
	andi	t2, 0xff
	li		t3, 0x1
	bne		t1, t3, 2b
	li		t3, 0xa
	bne		t2, t3, 2b
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0	
	
	move	v0, zero
	jr		ra
#endif

put_data:
	li		t0, 0x3
	and		t1, s2, t0
	li		t0, DFUINS_STATE_DATA
	bne		t1, t0, __putdata_err
	

	li		t0, (1 << SHIFT_DATAIDX)
	and		t1, s2, t0
	beqz	t1, put_data_done

	/* clear status */
	li		t0, ~(0xff00)
	and		s2, t0
	
	srl		t1, s1, SHIFT_BUFIDX		/* t1 = bufidx */
	move	t2, t1						/* t2 = t1 */
	
	addi	t2, 0x8
	bgt		t2, DFU_BUF_SIZE, __putdata_err	/* if (bufidx + 4) > DFU_BUF_SIZE, goto err */
__putdata_ok:
	/* put data */
	la		t0, nor_data_buf
	add		t0, t1
	sw		t9, 0(t0)
	addi	t0, 4
	sw		s3, 0(t0)
	
	li		t0, 0x80000
	add		s1, t0						/* bufidx += 8 */
	
put_data_done:
	/* set status */
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra
__putdata_err:
	li		t0, 0xff
	sb		t0, DFUINS_CMD0(s0)		/* t1(cmdreg0) */	
	sb		t0, DFUINS_CMD1(s0)		/* t2(cmdreg1) */	
	
	li		t0, SV_ERROR	
	sb		t0, DFUINS_STATUS(s0)	
	srl		t0, s7, SHIFT_CMDREG
	sb		t0, DFUINS_CMDECHO(s0)
	
	j		dfu_install
	
get_norid:
	li		t0, 0xffff
	and		t1, s3, t0
	li		t0, ~(0xffff)
	and		s7, t0
	or		s7, t1
	
	li		t0, NOR_CTRL_BASE
	/* read id cmd */
	and		t1, s6, 0xff	
	sll		t1, 24
	li		t2, CMD_SPI_RDID
	or		t1, t2
	sw		t1, SPI_CMD(t0)
	
	wait_cmd_ready	t3	
	lw		t1, NOR_RD_PORT(t0)
	
	li		t3, SPARE_REG_BASE
	srl		t2, t1, 16
	sb		t2, DFUINS_DATABUFR0(t3)
	srl		t2, t1, 8
	sb		t2, DFUINS_DATABUFR1(t3)
	sb		t1, DFUINS_DATABUFR2(t3)
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra
	
/* get dev id*/
get_devid:
    li      t1, DEVICE_ID
    li      t3, SPARE_REG_BASE
    sb      t1, DFUINS_DATABUFR0(t3)
    srl     t2, t1, 8
    sb      t2, DFUINS_DATABUFR1(t3)
    srl     t2, t1, 16
    sb      t2, DFUINS_DATABUFR2(t3)
    srl     t2, t1, 24
    sb      t2, DFUINS_DATABUFR3(t3)
    /* set status */
    li      t0, ~(0xff00)
    and     s2, t0
    li      t0, SV_DONE
    sll     t0, SHIFT_STATUS
    or      s2, t0

    move    v0, zero
    jr      ra

/* check layout, return text size to dfui2c*/
chk_layout:
    li      t0, NOR_DIR
    lw      t1, LAYOUT_TEXTSIZE(t0)
    li      t3, SPARE_REG_BASE
    sb      t1, DFUINS_DATABUFR0(t3)
    srl     t2, t1, 8
    sb      t2, DFUINS_DATABUFR1(t3)
    srl     t2, t1, 16
    sb      t2, DFUINS_DATABUFR2(t3)
    srl     t2, t1, 24
    sb      t2, DFUINS_DATABUFR3(t3)
    /* set status */
    li      t0, ~(0xff00)
    and     s2, t0
    li      t0, SV_DONE
    sll     t0, SHIFT_STATUS
    or      s2, t0

    move    v0, zero
    jr      ra

/* */
get_reg_status:
	li		t0, NOR_CTRL_BASE
	/* read reg status cmd */
	and		t1, s7, 0xff	
	sll		t1, 24
	li		t2, CMD_SPI_RDSR
	or		t1, t2
	sw		t1, SPI_CMD(t0)
	wait_cmd_ready	t3
	
	lw		t1, NOR_RD_PORT(t0)
	move	v1, t1
	li		t3, SPARE_REG_BASE
	sb		t1, DFUINS_DATABUFR0(t3)
	srl		t2, t1, 8
	sb		t2, DFUINS_DATABUFR0(t3)
	srl		t2, t1, 16
	sb		t2, DFUINS_DATABUFR0(t3)
	srl		t2, t1, 24
	sb		t2, DFUINS_DATABUFR0(t3)
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	move	v0, zero
	jr		ra
	
reserved_0b: /* no use */
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	
	move	v0, zero
	jr		ra
	
start_data_session:
	li		t0, DFUINS_STATE_DATA
	li		t1, ~(0x3)
	and 	s2, t1
	or		s2, t0
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or      t0, (1 << SHIFT_D0_FIRST)         //  d0_first = 1;
	or		s2, t0
	move	v0, zero
	jr		ra
	
stop_dfu_install:
	/* set status */
	li		t0, ~(0xff00)
	and		s2, t0
	li		t0, SV_DONE
	sll		t0, SHIFT_STATUS
	or		s2, t0
	move	v0, zero
	jr		ra

    //.section .rodata_di
.macro cmd_srv  cmd, entry
_it_\cmd:
    .byte       \cmd
    .long       \entry
.endm

.align      0
cmd_table:
    cmd_srv CMD_RESET,          cmd_reset
    cmd_srv CHK_DFU,            chk_dfu
    cmd_srv CHK_FINISH,         chk_finish
    cmd_srv CHK_CRC,            chk_crc
    cmd_srv RESERVED_04,        reserved_04
    cmd_srv SET_PG_LEN,         set_pg_len
    cmd_srv SET_REG_STATUS,     set_reg_status
    cmd_srv ERASE_BLK,          erase_blk
    cmd_srv PUT_DATA,           put_data
    cmd_srv GET_ID,             get_norid
    cmd_srv GET_REG_STATUS,     get_reg_status
    cmd_srv RESERVED_0b,        reserved_0b
    cmd_srv START_DATA_SESSION, start_data_session
    cmd_srv STOP_DFU_INSTALL,   stop_dfu_install
    cmd_srv GET_DEVID,          get_devid
    cmd_srv CHK_LAYOUT,         chk_layout
end_of_cmd_table:
    .byte 0xff
    .align      2

msgGETCMD:          .asciiz         " CMD "
msgTAG:             .asciiz         " TAG "
msgCRC:             .asciiz         "\r\ncrc check "
msgCRCOK:           .asciiz         "\r\ncrc ok "
msgCRCERR:          .asciiz         "\r\ncrc err "
msgCHIPERASE:       .asciiz         "\r\n Chip Erase ... "
msgBLOCKERASE:      .asciiz         "\r\n Block Erase\r\n"
msgBLOCKERASE2:     .asciiz         "\r\n Block Erase2\r\n"
msgNORWRITE:        .asciiz         "\r\n."
msgFINISHOK:        .asciiz         "\r\n Check Finish OK "
msgFINISHNG:        .asciiz         "\r\n Check Finish Failed "
msgDFUMODE:         .asciiz         "\r\nDFU mode (resend)"
msgDUMP:            .asciiz         "\r\n\r\nDump reg: "
msgTOR:             .asciiz         " tor : "
msgEMPTY:           .asciiz         "  "
msgCMDREG1:         .asciiz         "\r\ncmdreg1:   "
msgErrDFUCMD:       .asciiz         "\r\nUnknown CMD: "
msgWarnDGEN:        .asciiz         "\nWarn: DGEN failed\r\n"
msgDBG_CHK_DFU:     .asciiz         "\r\n CHK_DFU"
msgDBGNORINIT:      .asciiz         "\r\n nor init"
msgDBGMAINLOOP:     .asciiz         "\r\n main loop"
msgDBGBEGIN:        .asciiz         "\r\n begin"
msgDBGCMDSTATE:     .asciiz         "\r\n cmd state"
msgDBGDIDX0:        .asciiz         "\r\n data idx 0"
msgDBGDIDX1:        .asciiz         "\r\n data idx 1"
msgDBGRESERVE4:     .asciiz         "\r\n reserved 4"
	//.section .data_buf_di
    .align      4
nor_data_buf:	.space	DFU_BUF_SIZE
