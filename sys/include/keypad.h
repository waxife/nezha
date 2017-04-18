/**
 *  @file   keypad.h
 *  @brief  Cheetah SAR keypad driver header
 *  $Id: keypad.h,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/03/24  C.N.Yu  New file.
 *
 */
#ifndef _KEYPAD_H_
#define _KEYPAD_H_

enum KEY_VAL {
    KEY_VAL_1 = 1,
    KEY_VAL_2 = 2,
    KEY_VAL_3 = 3,
    KEY_VAL_4 = 4,
    KEY_VAL_5 = 5,
    KEY_VAL_6 = 6
};

int keypad_setup(int id);
int keypad_open(void);
int keypad_get_keyvalue(void);
int keypad_close(void);

#define ERR_KEYPAD_BASE     2000
#define ERR_KEYPAD_INV_DEV  (ERR_KEYPAD_BASE + 1)

#endif /* _KEYPAD_H_ */
