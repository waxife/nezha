/**
 *  @file   sdm.h
 *  @brief  header file of sigma delta manager
 *  $Id: sdm.h,v 1.1.1.1 2013/12/18 03:43:42 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/28  Jonathan New file.
 *
 */

#ifndef __SDM_H_
#define __SDM_H_

#define SDM_BASE                            0xB1000000

#define SDM_ADC_CONF_0                      (SDM_BASE + 0x00000000)
#define SDM_ADC_CONF_1                      (SDM_BASE + 0x00000004)
#define SDM_ADC_STATUS                      (SDM_BASE + 0x00000008)
#define SDM_DAC_CONF_0                      (SDM_BASE + 0x00000010)
#define SDM_DAC_CONF_1                      (SDM_BASE + 0x00000014)

/* SDM_ADC_CONF_0 */
#define SDM_ADC_RESET                       (0x1 << 1)
#define SDM_ADC_EN                          (0x1)

/* SDM_ADC_CONF_1 */
#define ADC_AMP_SEL                         (4)
#define ADC_ODR_SEL                         (3)
#define ADC_FDIN_SEL                        (2)
#define ADC_DCRM_SEL                        (1)
#define ADC_OSR_SEL                         (0)
#define ADC_AMP_1                           (0x0 << 4)
#define ADC_AMP_2                           (0x1 << 4)
#define ADC_AMP_3                           (0x2 << 4)
#define ADC_AMP_4                           (0x3 << 4)
#define ADC_AMP_5                           (0x4 << 4)
#define ADC_AMP_6                           (0x5 << 4)
#define ADC_AMP_7                           (0x6 << 4)
#define ADC_AMP_8                           (0x7 << 4)
#define ADC_ORD_2ND                         (0x0 << 3)
#define ADC_ORD_4TH                         (0x1 << 3)
#define ADC_FDIN_EN                         (0x1 << 2)
#define ADC_DCRM_EN                         (0x1 << 1)
#define ADC_OSR_128                         (0x0)
#define ADC_OSR_256                         (0x1)

/* SDM_ADC_STATUS */
#define SDM_ADC_STAT(x)                     ((x) & 0x1)

/* SDM_DAC_CONF_0 */
#define DAC_AMP_SEL                         (2)
#define DAC_OSR_SEL                         (0)
#define DAC_AMP_1_8                         (0x0 << 2)
#define DAC_AMP_1_4                         (0x1 << 2)
#define DAC_AMP_1_2                         (0x2 << 2)
#define DAC_AMP_1                           (0x3 << 2)
#define DAC_OSR_256                         (0x0)
#define DAC_OSR_128                         (0x1)
#define DAC_OSR_64                          (0x2)

/* SDM_DAC_CONF_1 */
#define SDM_DAC_PWR_OFF                     (0x0)
#define SDM_DAC_PWR_ON                      (0x1)


#endif /* __SDM_H_ */
