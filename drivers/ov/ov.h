/**
 *  @file   ov.h
 *  @brief  OmniVision camera chip driver header
 *  $Id: ov.h,v 1.1.1.1 2013/12/18 03:43:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/06  hugo  New file.
 *
 */

#ifndef __OV_H
#define __OV_H

#include <config.h>

#ifdef CONFIG_OV7670
#include "ov7670.h"

#define ov_set_reg_mask(pov, reg, val, msk) ov7670_set_reg_mask(pov, reg, val, msk)

#define ov_set_reg(pov, reg, val)   ov7670_set_reg(pov, reg, val)
#define ov_set_sat(pov, val)        ov7670_set_sat(pov, val)
#define ov_set_hue(pov, val)        ov7670_set_hue(pov, val)
#define ov_set_bright(pov, val)     ov7670_set_bright(pov, val)
#define ov_set_contrast(pov, val)   ov7670_set_contrast(pov, val)
#define ov_set_vflip(pov, val)      ov7670_set_vflip(pov, val)
#define ov_set_hflip(pov, val)      ov7670_set_hflip(pov, val)
#define ov_set_gain(pov, val)       ov7670_set_gain(pov, val)
#define ov_set_autogain(pov, val)   ov7670_set_autogain(pov, val)
#define ov_set_exp(pov, val)        ov7670_set_exp(pov, val)
#define ov_set_autoexp(pov, val)    ov7670_set_autoexp(pov, val)

#define ov_get_reg(pov, reg, val)   ov7670_get_reg(pov, reg, val)
#define ov_get_sat(pov, val)        ov7670_get_sat(pov, val)
#define ov_get_hue(pov, val)        ov7670_get_hue(pov, val)
#define ov_get_bright(pov, val)     ov7670_get_bright(pov, val)
#define ov_get_contrast(pov, val)   ov7670_get_contrast(pov, val)
#define ov_get_vflip(pov, val)      ov7670_get_vflip(pov, val)
#define ov_get_hflip(pov, val)      ov7670_get_hflip(pov, val)
#define ov_get_gain(pov, val)       ov7670_get_gain(pov, val)
#define ov_get_autogain(pov, val)   ov7670_get_autogain(pov, val)
#define ov_get_exp(pov, val)        ov7670_get_exp(pov, val)
#define ov_get_autoexp(pov, val)    ov7670_get_autoexp(pov, val)

#define ov_dump(pov)                ov7670_dump(pov)
#define ov_init(pov)                ov7670_init(pov)

#endif /* CONFIG_OV7670 */

#ifdef CONFIG_OV7740
#include "ov7740.h"

#define ov_set_reg_mask(pov, reg, val, msk) ov7740_set_reg_mask(pov, reg, val, msk)

#define ov_set_reg(pov, reg, val)   ov7740_set_reg(pov, reg, val)
#define ov_set_sat(pov, val)        ov7740_set_sat(pov, val)
#define ov_set_hue(pov, val)        ov7740_set_hue(pov, val)
#define ov_set_bright(pov, val)     ov7740_set_bright(pov, val)
#define ov_set_contrast(pov, val)   ov7740_set_contrast(pov, val)
#define ov_set_vflip(pov, val)      ov7740_set_vflip(pov, val)
#define ov_set_hflip(pov, val)      ov7740_set_hflip(pov, val)
#define ov_set_gain(pov, val)       ov7740_set_gain(pov, val)
#define ov_set_autogain(pov, val)   ov7740_set_autogain(pov, val)
#define ov_set_exp(pov, val)        ov7740_set_exp(pov, val)
#define ov_set_autoexp(pov, val)    ov7740_set_autoexp(pov, val)

#define ov_get_reg(pov, reg, val)   ov7740_get_reg(pov, reg, val)
#define ov_get_sat(pov, val)        ov7740_get_sat(pov, val)
#define ov_get_hue(pov, val)        ov7740_get_hue(pov, val)
#define ov_get_bright(pov, val)     ov7740_get_bright(pov, val)
#define ov_get_contrast(pov, val)   ov7740_get_contrast(pov, val)
#define ov_get_vflip(pov, val)      ov7740_get_vflip(pov, val)
#define ov_get_hflip(pov, val)      ov7740_get_hflip(pov, val)
#define ov_get_gain(pov, val)       ov7740_get_gain(pov, val)
#define ov_get_autogain(pov, val)   ov7740_get_autogain(pov, val)
#define ov_get_exp(pov, val)        ov7740_get_exp(pov, val)
#define ov_get_autoexp(pov, val)    ov7740_get_autoexp(pov, val)

#define ov_dump(pov)                ov7740_dump(pov)
#define ov_init(pov)                ov7740_init(pov)

#endif /* CONFIG_OV7740 */

#ifdef CONFIG_OV7725
#include "ov7725.h"

#define ov_set_reg_mask(pov, reg, val, msk) ov7725_set_reg_mask(pov, reg, val, msk)

#define ov_set_reg(pov, reg, val)   ov7725_set_reg(pov, reg, val)
#define ov_set_sat(pov, val)        ov7725_set_sat(pov, val)
#define ov_set_hue(pov, val)        ov7725_set_hue(pov, val)
#define ov_set_bright(pov, val)     ov7725_set_bright(pov, val)
#define ov_set_contrast(pov, val)   ov7725_set_contrast(pov, val)
#define ov_set_vflip(pov, val)      ov7725_set_vflip(pov, val)
#define ov_set_hflip(pov, val)      ov7725_set_hflip(pov, val)
#define ov_set_gain(pov, val)       ov7725_set_gain(pov, val)
#define ov_set_autogain(pov, val)   ov7725_set_autogain(pov, val)
#define ov_set_exp(pov, val)        ov7725_set_exp(pov, val)
#define ov_set_autoexp(pov, val)    ov7725_set_autoexp(pov, val)

#define ov_get_reg(pov, reg, val)   ov7725_get_reg(pov, reg, val)
#define ov_get_sat(pov, val)        ov7725_get_sat(pov, val)
#define ov_get_hue(pov, val)        ov7725_get_hue(pov, val)
#define ov_get_bright(pov, val)     ov7725_get_bright(pov, val)
#define ov_get_contrast(pov, val)   ov7725_get_contrast(pov, val)
#define ov_get_vflip(pov, val)      ov7725_get_vflip(pov, val)
#define ov_get_hflip(pov, val)      ov7725_get_hflip(pov, val)
#define ov_get_gain(pov, val)       ov7725_get_gain(pov, val)
#define ov_get_autogain(pov, val)   ov7725_get_autogain(pov, val)
#define ov_get_exp(pov, val)        ov7725_get_exp(pov, val)
#define ov_get_autoexp(pov, val)    ov7725_get_autoexp(pov, val)

#define ov_dump(pov)                ov7725_dump(pov)
#define ov_init(pov)                ov7725_init(pov)

#endif /* CONFIG_OV7725 */

#endif /* __OV_H */
