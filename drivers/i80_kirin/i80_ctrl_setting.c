/**
 *  @file   i80_ctrl_setting.c
 *  @brief  i80 system setting (via Kirin I80 master)
 *  $Id: i80_ctrl_setting.c,v 1.1 2014/03/14 08:57:41 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2014/03/14  Kevin  New file.
 */


struct i80_lcm lcm_ctr = {
    .xmax =             400,//XMAX,
    .ymax =             240,//YMAX,
    .wmi =              0x002C,
    .rmi =              0x002E,
    .hac =              1,
    .hrd =              1,
    .hmd =              1,
    .fac =              1,
    .frd =              1,
    .fmd =              1,
    .r00 =              0x002C0403, // WR_IDX=0x002C, INDEX_SWAP=1, BUS_8BIT
    .r01 =              0x002A0000, // HGRAM_AD_INDEX=0x002a
    .r02 =              0x002B0000, // VGRAM_AD_INDEX=0X002b
    .r03 =              0x010D030D,
    .r05 =              0x00000000,
    .r17 =              0x00000004, // END_ADDRESS_EN=1
    .r18 =              0x018F00EF, // HGRAM_AD_END=0x018F, VGRAM_AD_END=0x00EF,  Resolution: 400x240

};

int i80_T582_ili9327_setting(int width, int height){
    /* setup private data */
    memset (pi80, 0, sizeof (struct i80c));
    pi80->lcm = &lcm_ctr;
    pi80->lcm->xmax = width;
    pi80->lcm->ymax = height;
    pi80->lcm->wmi = 0x002C;    // (I80_R11)Write Memory Index  
    pi80->lcm->rmi = 0x002E;    // (I80_R11)Read  Memory Index 
    pi80->lcm->hac = 1;         // (I80_R04)Half bus, Address Cycle count 
    pi80->lcm->hrd = 1;         // (I80_R12)Half bus, Register Dummy cycle count
    pi80->lcm->hmd = 1;         // (I80_R12)Half bus, Memory Dummy cycle count 
    pi80->lcm->fac = 1;         // (I80_R04)Full bus, Address Cycle count 
    pi80->lcm->frd = 1;         // (I80_R12)Full bus, Register Dummy cycle count 
    pi80->lcm->fmd = 1;         // (I80_R12)Full bus, Memory Dummy cycle count 
    WRITEL (0x002C0403, I80_R00);   // WR_IDX=0x002C, INDEX_SWAP=1, BUS_8BIT
    usleep (1);
    WRITEL (0x002A0000, I80_R01);   // HGRAM_AD_INDEX=0x002a
    WRITEL (0x002B0000, I80_R02);   // VGRAM_AD_INDEX=0X002b
    WRITEL (0x010D030D, I80_R03);   // Read/Write Strobe. In T582 you should not modify it!
    WRITEL (0x00000000, I80_R05);   
    WRITEL (0x0C010202, I80_R06);   // I80_DE_POL=1, I80_HSYNC_POL=1
    WRITEL (0x00000004, I80_R17);   // END_ADDRESS_EN=1
    WRITEL (0x018F00EF, I80_R18);   // HGRAM_AD_END=0x018F, VGRAM_AD_END=0x00EF
    return 0;
}

int i80_T582_ili9320_setting(int width, int height){
    /* setup private data */
    memset (pi80, 0, sizeof (struct i80c));
    pi80->lcm = &lcm_ctr;
    pi80->lcm->xmax = width;
    pi80->lcm->ymax = height;
    pi80->lcm->wmi = 0x0022;    // (I80_R11)Write Memory Index  
    pi80->lcm->rmi = 0x0022;    // (I80_R11)Read  Memory Index 
    pi80->lcm->hac = 2;         // (I80_R04)Half bus, Address Cycle count 
    pi80->lcm->hrd = 0;         // (I80_R12)Half bus, Register Dummy cycle count
    pi80->lcm->hmd = 2;         // (I80_R12)Half bus, Memory Dummy cycle count 
    pi80->lcm->fac = 1;         // (I80_R04)Full bus, Address Cycle count 
    pi80->lcm->frd = 0;         // (I80_R12)Full bus, Register Dummy cycle count 
    pi80->lcm->fmd = 1;         // (I80_R12)Full bus, Memory Dummy cycle count 
    WRITEL (0x00220003, I80_R00);   // WR_IDX=0x002C, INDEX_SWAP=1, BUS_8BIT
    usleep (1);
    WRITEL (0x00200000, I80_R01);   // HGRAM_AD_INDEX=0x002a
    WRITEL (0x00210000, I80_R02);   // VGRAM_AD_INDEX=0X002b
    WRITEL (0x010D030D, I80_R03);   // Read/Write Strobe. In T582 you should not modify it!
    WRITEL (0x00000000, I80_R05);   
    WRITEL (0x0C010202, I80_R06);   // I80_DE_POL=1, I80_HSYNC_POL=1
    WRITEL (0x00000000, I80_R17);   // END_ADDRESS_EN=1
    WRITEL (0x00000000, I80_R18);   // HGRAM_AD_END=0x018F, VGRAM_AD_END=0x00EF
    return 0;
}
