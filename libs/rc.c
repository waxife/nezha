/**
 *  @file   resource.c
 *  @brief  this header file put system interface initial function prototypeing
 *  $Id: rc.c,v 1.3 2016/04/15 08:38:42 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc New file.
 *
 */

#include "./drivers/spirw/spirw.h"
#include "rc.h"
#include "debug.h"
#include "string.h"

//#define DBG_RC
//#define BINERY_VERSION
static struct res_item_t desc_b;

extern unsigned int _fresource, _eresource, _finfo, _einfo; 

struct res_t res_s;

void endian_change(int size,char *data){
  int i, half_size;
  char tmp;

  half_size = size/2;
  size--;

  for(i=0;i<half_size;i++){
    tmp = data[i];
    data[i]=data[size-i];
    data[size-i]=tmp;
  }
}
#define endian_inv(data) endian_change(sizeof(data),(char*)&(data))

void endian_inv_struct(int type, char* data)
{

    switch(type) {
	case TYPE_RES_T:
		endian_inv(((struct res_t*)data)->resource_magic);
		endian_inv(((struct res_t*)data)->nelements);
		endian_inv(((struct res_t*)data)->desc[0].id);
		endian_inv(((struct res_t*)data)->desc[0].offset);
		break;
	case TYPE_RES_ITEM_T:
		endian_inv(((struct res_item_t*)data)->id);
		endian_inv(((struct res_item_t*)data)->offset);
		break;
	case TYPE_IMG:
		endian_inv(((struct res_img_t*)data)->head);
		endian_inv(((struct res_img_t*)data)->lut_addr);
		endian_inv(((struct res_img_t*)data)->index_addr);
		endian_inv(((struct res_img_t*)data)->line_jump);
		endian_inv(((struct res_img_t*)data)->width);
		endian_inv(((struct res_img_t*)data)->height);
		break;
	case TYPE_SPRITE:
		endian_inv(((struct res_spr_t*)data)->head);
		endian_inv(((struct res_spr_t*)data)->lut_addr);
		endian_inv(((struct res_spr_t*)data)->index_addr);
		endian_inv(((struct res_spr_t*)data)->line_jump);
		endian_inv(((struct res_spr_t*)data)->height);

		break;
	case TYPE_EMU:
		endian_inv(((struct res_emu_t*)data)->head);
		endian_inv(((struct res_emu_t*)data)->base_addr);
		endian_inv(((struct res_emu_t*)data)->length);
		break;
	case TYPE_MENU:
		endian_inv(((struct res_menu_t*)data)->head);
		endian_inv(((struct res_menu_t*)data)->base_addr);
		endian_inv(((struct res_menu_t*)data)->oram_addr);
		endian_inv(((struct res_menu_t*)data)->length);
		break;
	case TYPE_TILE:
		endian_inv(((struct res_tile_t*)data)->head);
		endian_inv(((struct res_tile_t*)data)->base_addr);
		//endian_inv(((struct res_tile_t*)data)->oram_addr);
		endian_inv(((struct res_tile_t*)data)->length);
		break;
	case TYPE_LUT:
		endian_inv(((struct res_lut_t*)data)->head);
		endian_inv(((struct res_lut_t*)data)->lut_addr);
		endian_inv(((struct res_lut_t*)data)->main_len);
		break;
	case TYPE_OREG:
		endian_inv(((struct res_oreg_t*)data)->head);
		endian_inv(((struct res_oreg_t*)data)->base_addr);
		break;
	case TYPE_ICON:
		endian_inv(((struct res_icon_t*)data)->head);
		endian_inv(((struct res_icon_t*)data)->base_addr);
		break;
		break;
	case TYPE_TD:
		endian_inv(((struct res_td_t*)data)->head);
		endian_inv(((struct res_td_t*)data)->color_addr);
		endian_inv(((struct res_td_t*)data)->index_addr);
		endian_inv(((struct res_td_t*)data)->line_jump);
		endian_inv(((struct res_td_t*)data)->width);
		endian_inv(((struct res_td_t*)data)->height);
		break;
	case TYPE_TOFONT:
		endian_inv(((struct res_tof_t*)data)->head);
		endian_inv(((struct res_tof_t*)data)->base_addr);
		endian_inv(((struct res_tof_t*)data)->font_width);
		endian_inv(((struct res_tof_t*)data)->font_height);
		endian_inv(((struct res_tof_t*)data)->flash_addr);
		break;
	default:
    	break;
    }
}

/**
 * rc_init: init resource struct
 * return > 0 number of elements
 *        -1 invalid magic 
 *        -2 invalid number of elements
 *        -3 invalid pack struct
 *        -4 invalid element struct
 *        -5 invalid dma 2 xram
 */

int
rc_init (unsigned short *n)
{
	struct res_t *res = (struct res_t *) (&_eresource );
	unsigned long p_magic= 0;

	dbg(0, " resource point = %p \r\n", res);

    memcpy(&res_s, res, RES_T_SIZE);
    endian_inv_struct(TYPE_RES_T, (char*)&res_s);
    dbg(0, " resource magic = 0x%lX \r\n", res_s.resource_magic);

	if (res_s.resource_magic != RESOURCE_MAGIC)
        return -1;
	
	/* resource validation */
    if (res_s.nelements <= 0)
        return -2;

    *n = res_s.nelements;

    memcpy(&p_magic, &res->desc[*n], sizeof(unsigned long));
    endian_inv(p_magic);

	dbg(2, " resource end magic = 0x%lX \r\n", p_magic);

    if (p_magic != RESOURCE_END_MAGIC) {
            return -3;
    }

    return 0;
}

/** 
 * rc_data(ID)
 */
unsigned long
rc_data (unsigned short id)
{
#if 1    
    struct res_t *res = (struct res_t *) (&_eresource  );

	if (id == desc_b.id)
        return (unsigned long)((char *)res + desc_b.offset);
        
    return NULL;
#else    
	struct res_t *res = (struct res_t *) (&_eresource  );
	struct res_item_t desc_s;
    int i, n, cur_n;

	if (res_s.resource_magic != RESOURCE_MAGIC) {
		ERROR("invalid rc init!\r\n");
		return NULL;
	}
		
    n = res_s.nelements;

    for (i = 0; i < n; i++) {
        memcpy(&desc_s, &res->desc[cur_n], RES_ITEM_T_SIZE);
        endian_inv_struct(TYPE_RES_ITEM_T, (char*)&desc_s);
        cur_n++;
        if(cur_n >= n)
            cur_n = 0;
		if (id == desc_s.id)
            return (unsigned long)((char *)res + desc_s.offset);
    }

	return NULL;
#endif	
}

#ifdef BINERY_VERSION
static struct res_item_t desc_b;
unsigned short get_id_key(unsigned short index)
{
    struct res_t *res = (struct res_t *) (&_eresource  );
	
	memcpy(&desc_b, &res->desc[index], RES_ITEM_T_SIZE);
    endian_inv_struct(TYPE_RES_ITEM_T, (char*)&desc_b);

	return desc_b.id;
}
/**
 * Binary Searching
 */
int b_searching(int low, int high, unsigned short id)
{
	int i;

	if(id< get_id_key(low) || id> get_id_key(high))
		return 0;
	while(low<=high) {
		i= (low+high)/2;
		if(id< get_id_key(i))
			high = i-1;
		else if(id> get_id_key(i))
				low= i+1;
		else
			return i;
	}
	
	return 0;	
}
#endif
/**
 * rc_type(ID)
 * return SUB_TYPE | TYPE
 *        < 0 id not found
 */
unsigned short
rc_type (unsigned short id)
{
	struct res_t *res = (struct res_t *) (&_eresource  );	
    int i, n;

    if (res_s.resource_magic != RESOURCE_MAGIC) {
		ERROR("invalid rc init!\r\n");
		return -1;
	}

    n = res_s.nelements;

#ifdef BINERY_VERSION
    /* Binery searching */
	if(b_searching(0, n-1, id) > 0) {
	    if (id == desc_b.id)
            return desc_b.type | (desc_b.subtype << 16);
    }
#else
    for (i = 0;i < n; i++) {
        memcpy(&desc_b, &res->desc[i], RES_ITEM_T_SIZE);
        endian_inv_struct(TYPE_RES_ITEM_T, (char*)&desc_b);
		if (id == desc_b.id)
            return desc_b.type | (desc_b.subtype << 16);
    }
#endif
	ERROR("NO ID!\r\n");
    return -1;
}

/**
 * rc_len(ID)
 * return length
 */
unsigned long
rc_len (unsigned short id)
{
    struct res_t *res = (struct res_t *) (&_eresource  );
	struct res_item_t desc_s, desc_s1;
    int i, n;

    if (res_s.resource_magic != RESOURCE_MAGIC) {
		ERROR("invalid rc init!\r\n");
		return -1;
	}

    n = res_s.nelements;

    for (i = 0;i < n; i++) {
        memcpy(&desc_s, &res->desc[i], RES_ITEM_T_SIZE);
        endian_inv_struct(TYPE_RES_ITEM_T, (char*)&desc_s);
		if (id == desc_s.id) {
            memcpy(&desc_s1, &res->desc[i+1], RES_ITEM_T_SIZE);
            endian_inv_struct(TYPE_RES_ITEM_T, (char*)&desc_s1);
			if(i==(n-1))
				return 16;
			else
				return (unsigned long)(desc_s1.offset - desc_s.offset);
        }
    }

	ERROR("NO ID!\r\n");
    return NULL;
}

unsigned char
get_res_img (unsigned short id, struct res_img_t *img_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_IMG) {
		ERROR("Resource %d not is TYPE_IMG (1 != %u)!\n", id, type);
		return 1;
	}
	if(img_s == NULL) {
		ERROR("struct res_img_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(img_s, (unsigned char*)rc_data(id), RES_IMG_T_SIZE);
    endian_inv_struct(TYPE_IMG, (char*)img_s);

    return 0;
}

unsigned char
get_res_spr (unsigned short id, struct res_spr_t *spr_s)
{
	unsigned char type= rc_type(id);

	if(type!=TYPE_SPRITE) {
		ERROR("Resource %d not is TYPE_SPRITE (2 != %u)!\n", id, type);
		return 1;
	}
	if(spr_s == NULL) {
		ERROR("struct res_spr_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(spr_s, (unsigned char*)rc_data(id), RES_SPR_T_SIZE);
    endian_inv_struct(TYPE_SPRITE, (char*)spr_s);

    return 0;
}

unsigned char
get_res_emu (unsigned short id, struct res_emu_t *emu_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_EMU) {
		ERROR("Resource %d not is TYPE_EMU (3 != %u)!\n", id, type);
		return 1;
	}
	if(emu_s == NULL) {
		ERROR("struct res_emu_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(emu_s, (unsigned char*)rc_data(id), RES_EMU_T_SIZE);
    endian_inv_struct(TYPE_EMU, (char*)emu_s);

    return 0;
}

unsigned char
get_res_menu (unsigned short id, struct res_menu_t *menu_s)
{
	unsigned char type= rc_type(id);
	
	if(type != TYPE_MENU) {
		ERROR("Resource %d not is TYPE_MENU (4 != %u)!\n", id, type);
		return 1;
	}
	if(menu_s == NULL) {
		ERROR("struct res_menu_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(menu_s, (unsigned char*)rc_data(id), RES_MENU_T_SIZE);
    endian_inv_struct(TYPE_MENU, (char*)menu_s);

    return 0;
}

unsigned char
get_res_tile (unsigned short id, struct res_tile_t *tile_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_TILE) {
		ERROR("Resource %d not is TYPE_TILE (5 != %u)!\n", id, type);
		return 1;
	}
	if(tile_s == NULL) {
		ERROR("struct res_tile_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(tile_s, (unsigned char*)rc_data(id), RES_TILE_T_SIZE);
    endian_inv_struct(TYPE_TILE, (char*)tile_s);

    return 0;
}

unsigned char
get_res_lut (unsigned short id, struct res_lut_t *lut_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_LUT) {
		ERROR("Resource %d not is TYPE_LUT (6 != %u)!\n", id, type);
		return 1;
	}
	if(lut_s == NULL) {
		ERROR("struct res_lut_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(lut_s, (unsigned char*)rc_data(id), RES_LUT_T_SIZE);
    endian_inv_struct(TYPE_LUT, (char*)lut_s);

    return 0;
}

unsigned char
get_res_oreg (unsigned short id, struct res_oreg_t *oreg_s)
{
	unsigned char type= rc_type(id);
	
	if(type != TYPE_OREG) {
		ERROR("Resource %d not is TYPE_OREG (7 != %u)!\n", id, type);
		return 1;
	}
	if(oreg_s == NULL) {
		ERROR("struct res_oreg_t can't is NULL!\n");
		return 1;
	}

    memcpy(oreg_s, (unsigned char*)rc_data(id), RES_OREG_T_SIZE);
    endian_inv_struct(TYPE_OREG, (char*)oreg_s);

    return 0;
}

unsigned char
get_res_icon (unsigned short id, struct res_icon_t *icon_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_ICON) {
		ERROR("Resource %d not is RES_ICON_T_SIZE (8 != %u)!\n", id, type);
		return 1;
	}
	if(icon_s == NULL) {
		ERROR("struct res_icon_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(icon_s, (unsigned char*)rc_data(id), RES_ICON_T_SIZE);
    endian_inv_struct(TYPE_ICON, (char*)icon_s);

    return 0;
}

unsigned char
get_res_td (unsigned short id, struct res_td_t *td_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_TD) {
		ERROR("Resource %d not is TYPE_TD (9 != %u)!\n", id, type);
		return 1;
	}
	if(td_s == NULL) {
		ERROR("struct res_td_t can't is NULL!\n");
		return 1;
	}
	
    memcpy(td_s, (unsigned char*)rc_data(id), RES_TD_T_SIZE);
    endian_inv_struct(TYPE_TD, (char*)td_s);

    return 0;
}

unsigned char
get_res_tof (unsigned short id, struct res_tof_t *tof_s)
{
	unsigned char type= rc_type(id);
	
	if(type!=TYPE_TOFONT) {
		ERROR("Resource %d is not TYPE_TOFONT (8 != %u)!\n", id, type);
		return 1;
	}
	if(tof_s == NULL) {
		ERROR("struct res_tof_t can't is NULL!\n");
		return 1;
	}
																		  
    memcpy(tof_s, (unsigned char*)rc_data(id), RES_TOF_T_SIZE);
    endian_inv_struct(TYPE_TOFONT, (char*)tof_s);

    return 0;
}


#ifdef DBG_RC
void show_img_info (unsigned long fp)
{
	struct res_img_t img_s;

    memcpy(&img_s, (unsigned char*)fp, RES_IMG_T_SIZE);
    endian_inv_struct(TYPE_IMG, &img_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : SOSD IMAGE\r\n");
	DBG_PRINT("lut_addr  : 0x%lX\r\n", img_s.lut_addr);
	DBG_PRINT("index_addr: 0x%lX\r\n", img_s.index_addr);
	DBG_PRINT("line jump : %u\r\n", img_s.line_jump);
	DBG_PRINT("width     : %u\r\n", img_s.width);
	DBG_PRINT("height    : %u\r\n", img_s.height);
	DBG_PRINT("\r\n");
}

void show_spr_info (unsigned long fp)
{
	struct res_spr_t spr_s;

    memcpy(&spr_s, (unsigned char*)fp, RES_SPR_T_SIZE);
    endian_inv_struct(TYPE_SPRITE, &spr_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : SOSD SPRITE\r\n");
	DBG_PRINT("lut_addr  : 0x%lX\r\n", spr_s.lut_addr);
	DBG_PRINT("index_addr: 0x%lX\r\n", spr_s.index_addr);
	DBG_PRINT("line jump : %u\r\n", spr_s.line_jump);
	DBG_PRINT("width     : %u\r\n", spr_s.width);
	DBG_PRINT("height    : %u\r\n", spr_s.height);
	DBG_PRINT("count     : %u\r\n", spr_s.count);
	DBG_PRINT("\r\n");
}

void show_emu_info (unsigned long fp)
{
	struct res_emu_t emu_s;

    memcpy(&emu_s, (unsigned char*)fp, RES_EMU_T_SIZE);
    endian_inv_struct(TYPE_EMU, &emu_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : SOSD EMULATION\r\n");
	DBG_PRINT("base_addr : 0x%lX\r\n", emu_s.base_addr);
	DBG_PRINT("length    : %u\r\n", emu_s.length);
	DBG_PRINT("repeat    : %u\r\n", emu_s.repeat);
	DBG_PRINT("\r\n");
}

void show_menu_info (unsigned long fp)
{
	struct res_menu_t menu_s;

    memcpy(&menu_s, (unsigned char*)fp, RES_MENU_T_SIZE);
    endian_inv_struct(TYPE_MENU, &menu_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : OSD2 MENU\r\n");
	DBG_PRINT("base_addr : 0x%lX\r\n", menu_s.base_addr);
	DBG_PRINT("length    : %u\r\n", menu_s.length);
	DBG_PRINT("\r\n");
}

void show_tile_info (unsigned long fp)
{
	struct res_tile_t tile_s;

    memcpy(&tile_s, (unsigned char*)fp, RES_TILE_T_SIZE);
    endian_inv_struct(TYPE_TILE, &tile_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : OSD2 TILE\r\n");
	DBG_PRINT("base_addr : 0x%lX\r\n", tile_s.base_addr);
	DBG_PRINT("length    : %u\r\n", tile_s.length);
	DBG_PRINT("\r\n");
}

void show_lut_info (unsigned long fp)
{
	struct res_lut_t lut_s;

    memcpy(&lut_s, (unsigned char*)fp, RES_LUT_T_SIZE);
    endian_inv_struct(TYPE_LUT, &lut_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : OSD2 LUT\r\n");
	DBG_PRINT("lut_addr  : 0x%lX\r\n", lut_s.lut_addr);
	DBG_PRINT("main_len  : %u\r\n", lut_s.main_len);
	DBG_PRINT("sec_len   : %u\r\n", lut_s.sec_len);
	DBG_PRINT("re_2bp_len: %u\r\n", lut_s.re_2bp_len);
	DBG_PRINT("re_bg_len : %u\r\n", lut_s.re_bg_len);
	DBG_PRINT("\r\n");
}

void show_oreg_info (unsigned long fp)
{
	struct res_oreg_t oreg_s;

    memcpy(&oreg_s, (unsigned char*)fp, RES_OREG_T_SIZE);
    endian_inv_struct(TYPE_OREG, &oreg_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : OSD2 REG TABLE\r\n");
	DBG_PRINT("base_addr : 0x%lX\r\n", oreg_s.base_addr);
	DBG_PRINT("length    : %u\r\n", oreg_s.length);
	DBG_PRINT("\r\n");
}

void show_icon_info (unsigned long fp)
{
	struct res_icon_t icon_s;

    memcpy(&icon_s, (unsigned char*)fp, RES_ICON_T_SIZE);
    endian_inv_struct(TYPE_ICON, &icon_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : OSD2 ICON TABLE\r\n");
	DBG_PRINT("base_addr : 0x%lX\r\n", icon_s.base_addr);
	DBG_PRINT("font_w	 : %u\r\n", icon_s.font_w);
	DBG_PRINT("font_h    : %u\r\n", icon_s.font_h);
	DBG_PRINT("width     : %u\r\n", icon_s.width);
	DBG_PRINT("height    : %u\r\n", icon_s.height);
	DBG_PRINT("bp    	 : %u\r\n", icon_s.bp);
	DBG_PRINT("count     : %u\r\n", icon_s.count);
	DBG_PRINT("\r\n");
}

void show_td_info (unsigned long fp)
{
	struct res_td_t td_s;

    memcpy(&td_s, (unsigned char*)fp, RES_TD_T_SIZE);
    endian_inv_struct(TYPE_TD, &td_s);
	DBG_PRINT("[resource header]\n");
	DBG_PRINT("type      : SOSD TD\r\n");
	DBG_PRINT("color_addr: 0x%lX\r\n", td_s.color_addr);
	DBG_PRINT("index_addr: 0x%lX\r\n", td_s.index_addr);
	DBG_PRINT("line jump : %u\r\n", td_s.line_jump);
	DBG_PRINT("width     : %u\r\n", td_s.width);
	DBG_PRINT("height    : %u\r\n", td_s.height);
	DBG_PRINT("\r\n");
}
#endif

void show_resource_header (unsigned short res_id)
{
	unsigned long fp;
	unsigned short type= 0;
	
	type = rc_type(res_id);

    DBG_PRINT("rc_id   : %u\n", res_id);
	DBG_PRINT("rc_type   : %d\n", type);
	DBG_PRINT("rc_len    : %ld bytes\n", rc_len(res_id));

	fp = rc_data(res_id);

#ifdef DBG_RC
	switch(type) {
		case TYPE_IMG:
			show_img_info(fp);
			break;
		
		case TYPE_SPRITE:
			show_spr_info(fp);
			break;
		
		case TYPE_EMU:
			show_emu_info(fp);
			break;
		
		case TYPE_MENU:
			show_menu_info(fp);
			break;

		case TYPE_TILE:
			show_tile_info(fp);
			break;
		
		case TYPE_LUT:
			show_lut_info(fp);
			break;

		case TYPE_OREG:
			show_oreg_info(fp);
			break;
		
		case TYPE_ICON:
			show_icon_info(fp);
			break;

		case TYPE_TD:
			show_td_info(fp);
			break;	
		
		default:
			ERROR("type error!\r\n");
			break;
	}
#endif

}

int resource_init (void)
{
	unsigned short n = 0;
    int rc= 0;

	rc = rc_init(&n);

	if (rc < 0) {
		if (rc == -1)
			ERROR("resource: invalid magic!\r\n");
		else if (rc == -2)
			ERROR("resource: invalid number of elements!\r\n");
		else if (rc == -3)
			ERROR("resource: invalid pack struct!\r\n");
		else if (rc == -4)
			ERROR("resource: invalid element struct!\r\n");
		else if (rc == -5)
			ERROR("resource: invalid dma 2 xram!\r\n");

		goto EXIT;
	}
	
	if (n>0) {
		DBG_PRINT("rc init success!\r\n");
		DBG_PRINT("number of elements is %d \r\n", n);

#ifdef DBG_RES_INFO
	#ifdef OLD_TOOL_COMBINE
		for(i=0; i<n; i++)
			show_resource_header(i);
	#else
		for(i=1; i<=n; i++)
			show_resource_header(get_res_id(i));
	#endif
#endif
	} else
		ERROR("no resource elements!\r\n");

EXIT:
	return rc;
}
