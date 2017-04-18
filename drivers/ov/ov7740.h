/**
 *  @file   ov7740.h
 *  @brief  OmniVision camera chip driver header
 *  $Id: ov7740.h,v 1.1.1.1 2013/12/18 03:43:49 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/09/24  hugo  New file.
 *
 */

#ifndef __OV7740_H
#define __OV7740_H

struct ovc {
    unsigned char sat;          /* Saturation value : 0 ~ 255 */
    int hue;                    /* Hue value : -180 ~ 180 */
};

/*
 * Synopsis     int ov7740_set_reg (struct ovc *pov, unsigned char reg, unsigned char val);
 * Description  write value to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_reg (struct ovc *pov, unsigned char reg, unsigned char val);

/*
 * Synopsis     int ov7740_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk);
 * Description  write bits at positions specified by mask to a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to write
 *              msk - enable bit mask
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_reg_mask (struct ovc *pov, unsigned char reg, unsigned char val, unsigned char msk);

/*
 * Synopsis     int ov7740_set_sat (struct ovc *pov, unsigned char val);
 * Description  set saturation
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_sat (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_hue (struct ovc *pov, int val);
 * Description  set hue
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_hue (struct ovc *pov, int val);

/*
 * Synopsis     int ov7740_set_bright (struct ovc *pov, unsigned char val);
 * Description  set brightness
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_bright (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_contrast (struct ovc *pov, unsigned char val);
 * Description  set contrast
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_contrast (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_vflip (struct ovc *pov, unsigned char val);
 * Description  set virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_vflip (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_hflip (struct ovc *pov, unsigned char val);
 * Description  set horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_hflip (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_gain (struct ovc *pov, unsigned char val);
 * Description  set gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_gain (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_autogain (struct ovc *pov, unsigned char val);
 * Description  set auto gain
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_autogain (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_set_exp (struct ovc *pov, unsigned short val);
 * Description  set exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_exp (struct ovc *pov, unsigned short val);

/*
 * Synopsis     int ov7740_set_autoexp (struct ovc *pov, unsigned char val);
 * Description  set auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int ov7740_set_autoexp (struct ovc *pov, unsigned char val);

/*
 * Synopsis     int ov7740_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val);
 * Description  read value from a register
 * Parameters   pov - point to ov private data
 *              reg - OV camera chip control register address
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_reg (struct ovc *pov, unsigned char reg, unsigned char *val);

/*
 * Synopsis     int ov7740_get_sat (struct ovc *pov, unsigned char *val);
 * Description  get saturation
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_sat (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_hue (struct ovc *pov, int *val);
 * Description  get hue
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_hue (struct ovc *pov, int *val);

/*
 * Synopsis     int ov7740_get_bright (struct ovc *pov, unsigned char *val);
 * Description  get brightness
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_bright (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_contrast (struct ovc *pov, unsigned char *val);
 * Description  get contrast
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_contrast (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_vflip (struct ovc *pov, unsigned char *val);
 * Description  get virtical flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_vflip (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_hflip (struct ovc *pov, unsigned char *val);
 * Description  get horizontal flip
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_hflip (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_gain (struct ovc *pov, unsigned char *val);
 * Description  get gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_gain (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_autogain (struct ovc *pov, unsigned char *val);
 * Description  get auto gain
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_autogain (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     int ov7740_get_exp (struct ovc *pov, unsigned char *val);
 * Description  get exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_exp (struct ovc *pov, unsigned short *val);

/*
 * Synopsis     int ov7740_get_autoexp (struct ovc *pov, unsigned char *val);
 * Description  get auto exposure
 * Parameters   pov - point to ov private data
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int ov7740_get_autoexp (struct ovc *pov, unsigned char *val);

/*
 * Synopsis     void ov7740_dump (struct ovc *pov);
 * Description  dump all registers
 * Parameters   pov - point to ov private data
 * Return       none
 */
void ov7740_dump (struct ovc *pov);

/*
 * Synopsis     int ov7740_init (struct ovc *pov);
 * Description  init ov camera chip
 *                - reset chip
 *                - check manufacturer ID, product ID, Version
 *                - setup hardware window (HREF, VREF)
 *                - setup registers (scaleing, gamma, AGC, AEC, color matrix)
 * Parameters   pov - point to ov private data
 * Return       0 for success, or -1 for any error
 */
int ov7740_init (struct ovc *pov);

#endif /* __OV7740_H */
