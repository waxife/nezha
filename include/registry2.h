/*
 *  @file   registry2.c
 *  @brief  registry2 for nor 
 *  $Id $
 *  $Author dos $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30  New file.
 *
 */
#ifndef __REGISTRY2_H
#define __REGISTRY2_H
#define reg_size 96
#define reg_mark (reg_size/32)
#define block_size (64*1024)
#define sys_num  10
#define reg_limit_size (56*1024)
#define reg_info_size 6 //[id][length][mark] 1 4 1
#define MAX_REG_ID (reg_size-sys_num-1)
/*
format : <tag> <value> <mark>
0~0:tag : id num
1~4:value :saved value 
5~5:mark(0x00) : make sure the data is saved
Please modify the t_* name to your id_name sequently 
*/

//must small than sys_num
#define RS_TOUCH_CALI_LEFT  0
#define RS_TOUCH_CALI_RIGHT 1
#define RS_TOUCH_CALI_UP    2
#define RS_TOUCH_CALI_DOWN  3
#define RS_TOUCH_CALI_MODE  4


/*
	Registry2 struct
	attribute:
		buff      ->the saved buffer
		offset    ->the offset of the flash address
		bitmark   ->mark the used value, the bit value = 0,the value is saved value
                                      the bit value = 1,the value is unsaved value
		pu        ->the block address of the nor flash
		configured->registry2 is already initialled
*/

struct registry2{
	int buff[reg_size];//offset
	int offset;//
	int bitmark[reg_mark];
	int pu;//flash physical address
	int configured;
};



#define mark_value 0

/**
 * @func      registry2_get
 * @brief     get tag value from registry 
 * @param     tag = id ,value = pointer value        
 * @return    0  pointer to saved value 
 *           -1  pointer to default value  
 */
extern int registry2_get(int tag,int *value,int default_value);
/**
 * @func      registry2_set
 * @brief     setting the tag value
 * @param     tag = saving id , value = saving value, default_value = if fail return def_value       
 * @return    0 successful 
 *           -1 the value already existed 
 */
extern int registry2_set(int tag,int value);
/**
 * @func      registry2_burst_set
 * @brief     setting the tag values
 * @param     tag = saving id , value = saving values, cnt = count, default_value = if fail return def_value
 * @return    0 successful
 *           -1 the value already existed
 */
extern int registry2_burst_set (int tag, int *value, int cnt);
/**
 * @func      registry2_init
 * @brief     registry2 initial function
 * @param     none       
 * @return    0: initial successful
 *           -1: already configured
 */
extern int registry2_init(void);
/**
 * @func      registry2_dump
 * @brief     the information of the registry2
 * @param     none       
 * @return    none
 */
void registry2_dump (void);

/**
 * @func      registry2_getn
 * @brief     get tag value from registry 
 * @param     tag = id ,value = pointer value        
 * @return    0  pointer to saved value 
 *           -1  pointer to default value  
 */
extern int registry2_getn(int tag,void *buf,int length);
/**
 * @func      registry2_setn
 * @brief     setting the tag value
 * @param     tag = saving id , value = saving value, default_value = if fail return def_value       
 * @return    0 successful 
 *           -1 the value already existed 
 */
extern int registry2_setn(int tag,void *buf,int length);

extern int registry2_set_all(void);

extern int _registry2_get(int tag,int *value,int default_value);
extern int _registry2_set(int tag,int value);
#endif
