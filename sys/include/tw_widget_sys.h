/**
 *  @file   tw_widget_sys.h
 *  @brief  head flie for tw_widget_sys
 *  $Id: tw_widget_sys.h,v 1.13 2014/08/21 04:06:14 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.13 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2013/11/15  Ken		New file.
 *
 */

#ifndef __TW_WIDGET_SYS_H__
#define __TW_WIDGET_SYS_H__

/* Event type */
#define TW_EVENT_TYPE_NONE              0
#define TW_EVENT_TYPE_EXPOSURE          1
#define TW_EVENT_TYPE_ENTER_SYSTEM      2
#define TW_EVENT_TYPE_QUIT_SYSTEM       3
#define TW_EVENT_TYPE_TOUCH_DOWN        4
#define TW_EVENT_TYPE_TOUCH_RELEASE     5
#define TW_EVENT_TYPE_TOUCH_MOTION      6
#define TW_EVENT_TYPE_KEY_DOWN          7
#define TW_EVENT_TYPE_KEY_UP            8
#define TW_EVENT_TYPE_KEY_REPEAT        9
#define TW_EVENT_TYPE_IR_DOWN           10
#define TW_EVENT_TYPE_IR_UP             11
#define TW_EVENT_TYPE_IR_REPEAT         12
#define TW_EVENT_TYPE_TIMEOUT           13
#define TW_EVENT_TYPE_UART              14
#define TW_EVENT_TYPE_GPIO              15
#define TW_EVENT_TYPE_OTHER             16
#define TW_EVENT_TYPE_SIGNAL			17
#define TW_EVENT_TYPE_BT_PB             18
#define TW_EVENT_TYPE_TOUCH_REPEAT      19
#define TW_EVENT_TYPE_BELL				20

/* Event type - Service */
#define TW_EVENT_TYPE_QUIT_TASK   		0x80
#define TW_EVENT_TYPE_SERVICE_CREATE   	0x81
#define TW_EVENT_TYPE_SERVICE_DESTROY   0x82

/* UI type */
#define TW_UI_TYPE_IMAGE             	1
#define TW_UI_TYPE_SPRITE             	2
#define TW_UI_TYPE_OSD2             	3

/* Return code */
#define TW_RETURN_NONE              	0
#define TW_RETURN_NOT_PAGE          	1
#define TW_RETURN_NO_ACTIVE      		2
#define TW_RETURN_NO_PROCESS       		3
#define TW_RETURN_NO_PAGE       		4
#define TW_RETURN_NO_SRV				5

/* Error code */
#define TW_ERROR_NO_PAGE				6
#define TW_ERROR_NO_SRV					7
#define TW_ERROR_NO_PAGE_HANDLE         8
#define TW_ERROR_NO_SRV_HANDLE          9

/* Return code - Service */
#define TW_RETURN_SUCCESS				0
#define TW_RETURN_CREATE           		0x80

/* Error code - Service */
#define TW_ERROR_SRV_CREATE_FAILE		0x81


/* TIMER ID */
#define TW_TIMER_TOTAL_NUM				4

#define TW_NO_SERVICE_REQ               0

#define TW_SERVICE_REQ_FAIL             0
#define TW_SERVICE_REQ_SUCCESS          1
#define TW_SERVICE_REQ_CREATE           2

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */

/**
 * Event for a keystroke typed for the window with has focus.
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char ch;			/**< 8-bit key value */
	unsigned char no;			/**< keypad no. */
} TW_EVENT_KEYSTROKE;

/**
 * Event for a IR code typed for the window with has focus.
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char ch;			/**< 16-bit key value */
} TW_EVENT_IRCODE;

/**
 * TW_EVENT_TYPE_TIMER
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char id;			/**< ID of expired timer */
//	unsigned char flags;		/**< for tw_timer, one source expand to Nth */
} TW_EVENT_TIMER;

/**
 * TW_EVENT_TYPE_GPIO
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char gid;			/**< ID of GPIO, the T530 only one group */
	unsigned short flags;		/**< 16bits of interrupt flag */
} TW_EVENT_GPIO;

/**
 * Events for touch motion or touch position.
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	int x;						/**< window x coordinate of touch */
	int y;						/**< window y coordinate of touch */
	unsigned int cpu_count;
} TW_EVENT_TOUCH;

/**
 * Events for UART
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char ch;			/**< 8-bit key value */
} TW_EVENT_UART;

/**
 * Events for page
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char id;			/**< page id */
	unsigned char mode;			/**< 0: normal, 1: slide */
} TW_EVENT_PAGE;

/**
 * Events for video signal status
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char active;		/* active status */
	unsigned char src;			/* source status */
	unsigned char std;			/* signal standard */
} TW_EVENT_SIGNAL;

/**
 * Events for ring bell status
 */
typedef struct __attribute__((packed)) {
	unsigned char type;			/**< event type */
	unsigned char status;		/* ring bell status */
	unsigned char idx;			/* bell index */
	unsigned char cnt;			/* repeat count */
} TW_EVENT_BELL;

/**
* Events for other
*/
typedef struct __attribute__((packed)) {
  unsigned char type;                   /**< event type */
  void	*p;             				/**< other define point */
  unsigned char addr;
  unsigned char len;
} TW_EVENT_OTHER;

/**
* Events for service
*/
typedef struct __attribute__((packed)) {
  unsigned char type;                   /**< event type */
  unsigned char srv_id;             	/**< service id */
  unsigned char cus_id;                 /**< custom id */
  int re_code;                          /**< retrun code */
} TW_EVENT_SRV;

/**
 * Union of all possible event structures.
 * This is the structure returned by get_event() and similar routines.
 */
typedef union __attribute__((packed)) {
	unsigned char 		type;			/**< event type */
	TW_EVENT_KEYSTROKE 	keystroke;      /**< keystroke events */
	TW_EVENT_IRCODE   	ircode;         /**< IR code events */
	TW_EVENT_TOUCH 		touch_panel;    /**< mouse motion events */
	TW_EVENT_TIMER 		timer;          /**< timer events */
	TW_EVENT_GPIO		gpios;			/**< GPIO events */
	TW_EVENT_UART 		uart;           /**< UART event */
	TW_EVENT_PAGE 		page;           /**< page event */
	TW_EVENT_SIGNAL 	signal;			/**< signal event */
	TW_EVENT_BELL		rbell;			/**< ring bell event */
	TW_EVENT_SRV  		service;        /**<Service event */
	TW_EVENT_OTHER 		other;          /**< Other definition event */
} TW_EVENT;

/*
 * tw task status structure
 */
typedef struct __attribute__((packed)) {
	int status;                         /* task current status */
	unsigned int page_id;               /* current page ID */
	unsigned int srv_id;                /* current service ID */
} tw_task_status_t;

/**
 * Service Type ID
 */
typedef enum {
	TW_TASK_STA_UNKNOW = 0,
	TW_TASK_STA_WIDGET,
	TW_TASK_STA_SERVICE,
	TW_TASK_STA_DESTROY,
	TW_TASK_STA_TOTAL_NUM
} tw_task_sta_id;

/**
 * Service Type ID
 */
typedef enum {
	TW_SRV_PLAYBACK_ID = 1,
	TW_SRV_RECODER_ID,
	TW_SRV_TEST_ID,
	TW_SRV_TOTAL_NUM
} tw_srv_type_id;

/**
 * OSD type order
 */
enum OBJECT_TYPE {
	IMAGE_TYPE = 1,
	SPRITE_TYPE,
	MENU_TYPE,
	ICON_TYPE,
	TEXT_TYPE
};

/**
 * sOSD Sprite structure
 */
struct tw_spr {
    unsigned short          sprID;
    unsigned short          x;          // unit is pixel
    unsigned short          y;          // unit is pixel
    unsigned char          	width;      // unit is pixel
    unsigned short          height;     // unit is pixel
	unsigned char 			cnt;
} __attribute__((packed));

/**
 * sOSD Image structure
 */
struct tw_img {
    unsigned short          imgID;
    unsigned short          x;          // unit is pixel
    unsigned short          y;          // unit is pixel
    unsigned short          width;      // unit is pixel
    unsigned short          height;     // unit is pixel
} __attribute__((packed));

/**
 * sOSD TD structure
 */
struct tw_td {
    unsigned short          tdID;
    unsigned short          x;          // unit is pixel
    unsigned short          y;          // unit is pixel
    unsigned short          width;      // unit is pixel
    unsigned short          height;     // unit is pixel
} __attribute__((packed));

/**
 * OSD2 Icon structure
 */
struct tw_icon {
    unsigned short          icon_id;
    unsigned char          	color;		// lut index | 2nd lut index
    unsigned char           x;          // unit is Character
    unsigned char           y;          // unit is Character
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is Character
    unsigned short         	index;
} __attribute__((packed));

/**
 * OSD2 Icon structure
 */
struct tw_icons {
	unsigned short          icon_id;
    unsigned char          	color;		// lut index | 2nd lut index
	unsigned char           x;          // unit is Character
    unsigned char           y;          // unit is Character
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is Character
    unsigned char          *table;
    unsigned short          offset;
} __attribute__((packed));

/**
 * OSD2 Icon structure
 */
struct tw_gicon {
    unsigned short          gicon_id;
    unsigned char          	color;		// lut index | 2nd lut index
    unsigned char           x;          // unit is Character
    unsigned char           y;          // unit is Character
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is Character
	unsigned short          index;
    unsigned short          oram_addr;
} __attribute__((packed));

/**
 * OSD2 Icon structure
 */
struct tw_gicons {
	unsigned short          gicons_id;
    unsigned char          	color;		// lut index | 2nd lut index
	unsigned char           x;          // unit is Character
    unsigned char           y;          // unit is Character
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is Character
    unsigned char          *table;
	unsigned short			offset;
	unsigned short          oram_addr;
} __attribute__((packed));

/**
 * OSD2 text (twfont) structure
 */
struct tw_txt {
    unsigned short          icon_id;   
    unsigned char           fg_color;      
    unsigned char           bg_color; 
    unsigned char           x;          // unit is Character
    unsigned char           y;          // unit is Character
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is Character
    unsigned short          index;  
} __attribute__((packed));

/**
 * OSD2 Canvas structure
 */
struct tw_canvas {
    unsigned char          	font_w;		// font width
    unsigned char           font_h;		// font height
    unsigned char			bpp;		// bits per pixel
    unsigned char           fg_color;	// foreground color index
    unsigned char           bg_color;	// background color index
    unsigned char			chkey;		// chrome key on canvas
    unsigned char           x;          // MENU X position, unit is Character
    unsigned char           y;          // MENU Y position, unit is Character
    unsigned short			index;		// font index
    unsigned short          addr;		// ORAM address
    int           			width;      // canvas width (pixel, alignment font_w)
    int           			height;     // canvas height (pixel, alignment font_h)
    unsigned short			size;		// canvas size (byte)
    char					*dbuf;		// buffer point (dynamic)
} __attribute__((packed));

/**
 * OSD2 Page structure
 */
struct tw_menu {
    unsigned short          menuID;
    unsigned short          tilesID;
	unsigned short			lutsID;
	unsigned short          regsID;
	unsigned short          tofID;
    unsigned char           x;          // start x
    unsigned char           y;          // start y
    unsigned char           width;      // unit is Character
    unsigned char           height;     // unit is ROW
	unsigned short          addr;
    unsigned char           font_w;
	unsigned char           font_h;
} __attribute__((packed));

struct tw_layout_item {
    void*               tw_object;
    void*               up;
    void*               down;
    void*               left;
    void*               right;
    void*               jump;
} __attribute__((packed));

struct tw_layout_group {
    unsigned char           width;
    unsigned char           height;
    struct tw_layout_item*  item;
} __attribute__((packed));

/**
 * sOSD Layout
 */
struct tw_layout {
    unsigned char           type;       // 0: image, 1: twbc
    unsigned char           cnt;        // group count
    struct tw_layout_group* group;  
} __attribute__((packed));

/**
 * OSD2 Scrolling text
 */
struct tw_scl_txt {
    unsigned short  total_len;
    unsigned char   direct;  
    unsigned char   tail_blank;  
    unsigned short  cur_index;   
    unsigned char   size_id;
    struct tw_txt*  ptxt;
} __attribute__((packed));

/**
 * TW timer
 */
struct tw_timer {
	unsigned int en;
	unsigned long long count;
	unsigned int msec;
	int	auto_set;
};

#pragma pack(pop)

#define DO_ISSUE	1
#define DONT_ISSU	0

#define PAGE_EVENT_NORMAL   0
#define PAGE_EVENT_SLIDE    1

#define DIR_LEFT        		0
#define DIR_RIGHT       		1
#define DIR_UP          		2
#define DIR_DOWN        		3

#define SLIDE_NONE      		0
#define SLIDE_H_DIR     		1
#define SLIDE_V_DIR     		2

#define IMG_SLIDE       		0
#define TD_SLIDE        		1

#define SLIDE_MOTION    		0xFE
#define SLIDE_HV_ERROR  		0xFF

#define CHANGEABLE_SET_MAX		16

typedef unsigned char (* tw_page_handler_t) (unsigned char id, TW_EVENT* event);
typedef unsigned char (* tw_srv_handler_t) (unsigned char id, void* srv_ctx);

extern unsigned char tw_page_handler(unsigned char id, TW_EVENT* event);

/*
 * tw widget system of check GPIO function
 */
typedef int (* tw_chk_gpio_event_t)(void);


/**
 * @func    region_check
 * @brief   check touch event of location whether or not in UI of region.
 * @param   TW_EVENT* event		tw widget system of event
 * 			x		region of x start
 * 			y		region of y start
 * 			w		region of width
 * 			h		region of height
 * 			type	UI of type
 * @return  0 if not in region, or 1 if in region
 */
extern unsigned char
region_check (TW_EVENT* event, unsigned int x, unsigned int y,
			  unsigned int w, unsigned int h, unsigned char type);
/**
 * @func    next_iconset
 * @brief   change to next icon set(changeable, multi-language).
 * @param   none
 * @return  none
 * @note	should call tw_draw_menu for change of effect
 */
extern void
next_iconset (void);
/**
 * @func    set_iconset_num
 * @brief   set icon set no.(changeable, multi-language).
 * @param   num		set no.
 * @return  none
 * @note	should call tw_draw_menu for change of effect
 */
extern void
set_iconset_num (unsigned char num);
/**
 * @func    next_iconset
 * @brief   getting current icon set of no.(changeable, multi-language).
 * @param   none
 * @return  icon set of no.
 */
extern unsigned char
get_iconset_num (void);
/**
 * @func    find_ch_icon
 * @brief   icon ID of mapping function with ICONSET (changeable).
 * @param   id		dynamic of icon ID
 *			pmenu	icon ID belongs to the this menu
 * @return  0 if not changeable menu or other errors
 */
extern unsigned short
find_ch_icon(unsigned short id, struct tw_menu* pmenu);
/**
 * @func    tw_load_oregs
 * @brief   loading a OSD2 registers table with specified ID.
 * @param   id	resource of ID
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_load_oregs (unsigned short id);
/**
 * @func    _tw_load_menu_at
 * @brief   loading a OSD2 MENU with specified ID and address.
 * @param   id		resource of ID
 * 			addr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
_tw_load_menu_at (unsigned short id, unsigned short addr);
/**
 * @func    tw_load_menu
 * @brief   loading a OSD2 MENU with specified struct MENU.
 * @param   *pmenu	struct MENU of point
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_load_menu (struct tw_menu* pmenu);
/**
 * @func    tw_load_icon
 * @brief   loading a OSD2 ICON with specified ID and address.
 * @param   id		resource of ID
 * 			*oaddr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_load_icon (unsigned short id, unsigned short *oaddr);
/**
 * @func    _tw_load_tiles_at
 * @brief   loading a OSD2 tiles(fonts) with specified ID and address.
 * @param   id		resource of ID
 * 			*oaddr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
_tw_load_tiles_at (unsigned short id, unsigned short oaddr);
/**
 * @func    tw_load_tiles
 * @brief   loading a OSD2 tiles(fonts) with specified ID.
 * @param   id		resource of ID
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_load_tiles (unsigned short id);
/**
 * @func    tw_load_luts
 * @brief   loading a OSD2 tiles(fonts) with specified ID.
 * @param   id		resource of ID
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_load_luts (unsigned short id);
/**
 * @func    tw_img_draw
 * @brief   drawing a SPIOSD IMAGE with specified struct IMAGE.
 * @param   *pimg		struct tw_img of point
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_img_draw (struct tw_img* pimg);
/**
 * @func    tw_spr_draw
 * @brief   loading a SPIOSD SPRITE with specified struct SPRITE.
 * @param   *pspr		struct tw_spr of point
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_spr_draw (struct tw_spr* pspr);
/**
 * @func    tw_get_spr_idx
 * @brief   getting a SPIOSD SPRITE of index address with specified ID.
 * @param   id		resource of ID
 * 			addr	sprite of index address
 * @return  0 if successfully, or 1 if fail
 */
extern void
tw_get_spr_idx (unsigned short id, unsigned long *addr);
/**
 * @func    tw_spr_action
 * @brief   action a SPIOSD SPRITE with specified struct SPRITE.
 * @param   struct tw_spr* pspr		struct tw_spr of point
 * @return  none
 */
extern void
tw_spr_action (struct tw_spr* pspr);
/**
 * @func    tw_td_draw
 * @brief   drawing a SPIOSD TWBC with specified ID.
 * @param   *ptd	sturct tw_td of point
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_td_draw (struct tw_td* ptd);
/**
 * @func    tw_emu_trig
 * @brief   trigger SPIOSD of emulation
 * @param   none
 * @return  none
 */
extern void
tw_emu_trig (void);
/**
 * @func    tw_load_emu
 * @brief   load SPIOSD of emulation and trigger
 * @param   id	emulation of ID
 * @return  none
 */
extern void
tw_load_emu (unsigned short id);
/**
 * @func    tw_wait_emu_done
 * @brief   waiting emulation action done
 * @param   none
 * @return  none
 */
extern void
tw_wait_emu_done (void);
/**
 * @func    tw_emu_exit
 * @brief   exit SPIOSD of emulation action
 * @param   none
 * @return  none
 */
extern void
tw_emu_exit (void);
/**
 * @func    tw_img_force_alpha
 * @brief   control SPIOSD image of force alpha
 * @param   en		0: disable 1: enable
 * @return  none
 */
extern void
tw_img_force_alpha (unsigned char en);
/**
 * @func    tw_menu_draw
 * @brief   drawing a SPIOSD MENU with specified struct MENU.
 * @param   *pmenu	sturct tw_menu of point
 * @return  0 if successfully, or 1 if fail
 */
extern unsigned char
tw_menu_draw(struct tw_menu* pmenu);
/**
 * @func    check_color_config
 * @brief   change to color attribute.
 * @param   index	font of index
 * 			offset	font of offset
 * 			color	color of index
 * @return  color of attribute
 */
extern unsigned char
check_color_config (unsigned short index, unsigned char offset, unsigned char color);
/**
 * @func    _icon_draw_at
 * @brief   draw(put) character attribute in MENU with parameters.
 * @param   index		font of index
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_icon_draw_at (unsigned short index, unsigned char color, unsigned char x, unsigned char y,
			   unsigned char width, unsigned char height, unsigned short menu_addr,
			   unsigned char menu_width, unsigned char menu_row, unsigned short offset,
			   unsigned char blinking);
/**
 * @func    _tw_icon_draw_at
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			offset, menu of parameters.
 * @param   *picon		icon structure of point
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_tw_icon_draw_at (struct tw_icon* picon, unsigned short menu_addr, unsigned char menu_width,
				  unsigned char menu_row, unsigned short offset, unsigned char blinking);
/**
 * @func    _icon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			index, location of parameters.
 * @param   index		font of index
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_icon_draw (unsigned short index, unsigned char color, unsigned char x, unsigned char y,
		    unsigned char width, unsigned char height, unsigned char blinking);
/**
 * @func    _n_tw_icon_draw
 * @brief   no update, draw(put) character attribute in MENU with ICON structure.
 * @param   *picon		icon structure of point
 * @return  none
 */
extern void
_n_tw_icon_draw (struct tw_icon* picon);
/**
 * @func    tw_icon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure.
 * @param   *picon		icon structure of point
 * @return  none
 */
extern void
tw_icon_draw (struct tw_icon* picon);
/**
 * @func    _n_tw_icon_loc
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
extern void
_n_tw_icon_loc (struct tw_icon* picon, unsigned char x, unsigned char y);
/**
 * @func    tw_icon_loc
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
extern void
tw_icon_loc (struct tw_icon* picon, unsigned char x, unsigned char y);
/**
 * @func    _n_tw_icon_color_change
 * @brief   no update, draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_n_tw_icon_color_change (struct tw_icon* picon, unsigned char color, unsigned char blanking);
/**
 * @func    _n_tw_icon_loc_color
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font*
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_n_tw_icon_loc_color (struct tw_icon* picon, unsigned char x, unsigned char y, unsigned char color, unsigned char blanking);
/**
 * @func    tw_icon_loc_color
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font*
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
tw_icon_loc_color (struct tw_icon* picon, unsigned char x, unsigned char y, unsigned char color, unsigned char blanking);
/**
 * @func    tw_icon_color_change
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
tw_icon_color_change (struct tw_icon* picon, unsigned char color, unsigned char blanking);
/**
 * @func    _n_tw_icon_1st_color
 * @brief   no update, change to 1st color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
_n_tw_icon_1st_color (struct tw_icon* picon);
/**
 * @func    tw_icon_1st_color
 * @brief   change to 1st color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
tw_icon_1st_color (struct tw_icon* picon);
/**
 * @func    _n_tw_icon_2nd_color
 * @brief   no update, change to 2nd color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
_n_tw_icon_2nd_color (struct tw_icon* picon);
/**
 * @func    tw_icon_1st_color
 * @brief   change to 2nd color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
tw_icon_2nd_color (struct tw_icon* picon);
/**
 * @func    _icons_draw_at
 * @brief   draw(put) character attribute in MENU with parameters (table).
 * @param   *table      index table
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_icons_draw_at(unsigned char *table, unsigned char color, unsigned char x, unsigned char y,
               unsigned char width, unsigned char height, unsigned short menu_addr,
               unsigned char menu_width, unsigned char menu_row, unsigned short offset,
               unsigned char blinking);
/**
 * @func    _tw_icons_draw_at
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			offset, menu of parameters (table).
 * @param   *picon		ICONS structure of point
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_tw_icons_draw_at (struct tw_icons* picon, unsigned short menu_addr, unsigned char menu_width,
                   unsigned char menu_row, unsigned short offset, unsigned char blinking);
/**
 * @func    _icons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			index, location of parameters (table).
 * @param   *table      index table
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 *          offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_icons_draw (unsigned char *table, unsigned char color, unsigned char x, unsigned char y,
             unsigned char width, unsigned char height, unsigned short offset, unsigned char blinking);
/**
 * @func    _n_tw_icons_draw
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure.
 * @param   *picon	ICONS structure of point
 * @return  none
 */
extern void
_n_tw_icons_draw (struct tw_icons* picon);
/**
 * @func    tw_icons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure.
 * @param   *picon	ICONS structure of point
 * @return  none
 */
extern void
tw_icons_draw (struct tw_icons* picon);
/**
 * @func    _n_tw_icons_loc
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
extern void
_n_tw_icons_loc (struct tw_icons* picon, unsigned char x, unsigned char y);
/**
 * @func    tw_icons_loc
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
extern void
tw_icons_loc (struct tw_icons* picon, unsigned char x, unsigned char y);
/**
 * @func    _n_tw_icons_color_change
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			color index and blinking.
 * @param   *picon		icons structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
_n_tw_icons_color_change (struct tw_icons* picon, unsigned char color, unsigned char blanking);
/**
 * @func    tw_icons_color_change
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			color index and blinking.
 * @param   *picon		icons structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
extern void
tw_icons_color_change (struct tw_icons* picon, unsigned char color, unsigned char blanking);
/**
 * @func    _n_tw_icons_1st_color
 * @brief   no update, change to 1st color of ICONS.
 * @param   *picon	ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
_n_tw_icons_1st_color (struct tw_icons* picon);
/**
 * @func    tw_icons_1st_color
 * @brief   change to 1st color of ICONS.
 * @param   *picon	ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
tw_icons_1st_color (struct tw_icons* picon);
/**
 * @func    _n_tw_icons_2nd_color
 * @brief   no update, change to 2nd color of ICONS.
 * @param   *picon		ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
_n_tw_icons_2nd_color (struct tw_icons* picon);
/**
 * @func    tw_icons_1st_color
 * @brief   change to 2nd color of ICONS.
 * @param   *picon		ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
extern void
tw_icons_2nd_color (struct tw_icons* picon);
/**
 * @func    _n_tw_gicon_load
 * @brief   no update, load character in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
extern void
_n_tw_gicon_load(struct tw_gicon* pgicon);
/**
 * @func    _n_tw_gicon_draw
 * @brief   no update, draw(put) character attribute in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
extern void
_n_tw_gicon_draw(struct tw_gicon* pgicon);
/**
 * @func    tw_gicon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */

/**
 * @func    _n_tw_gicon_loc
 * @brief   no update, draw(put) character attribute in MENU 
 *          with ICON structure and location parameters.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
extern void
_n_tw_gicon_loc(struct tw_gicon* pgicon, 
                unsigned char x, unsigned char y, unsigned char blank);
extern void
tw_gicon_draw(struct tw_gicon* pgicon);
/**
 * @func    _n_tw_gicons_draw
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure.
 * @param   *gpicon	Group ICONS structure of point
 * @return  none
 */
extern void
_n_tw_gicons_draw (struct tw_gicons* pgicon);
/**
 * @func    tw_gicons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure.
 * @param   *gpicon	Group ICONS structure of point
 * @return  none
 */
extern void
tw_gicons_draw (struct tw_gicons* pgicon);
/**
 * @func    tw_osd2_menu_write
 * @brief   draw(put) a character attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * @return  none
 */
extern void
tw_osd2_menu_write (unsigned char x, unsigned char y, unsigned short index, unsigned char color);
/**
 * @func    tw_osd2_menu_write_line
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * @return  none
 */
extern void
tw_osd2_menu_write_line (unsigned char y, unsigned short index, unsigned char color);
/**
 * @func    tw_osd2_menu_write_block
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * 			width	icon of width, unit is font
 * 			height	icon of height, unit is font
 * @return  none
 */
extern void
tw_osd2_menu_write_block (unsigned char x, unsigned char y, unsigned char width,
	unsigned char height, unsigned short index, unsigned char color);
/**
 * @func    tw_osd2_clear_char
 * @brief   clear(put blank font) a character attribute in MENU.
 * @param  	x	x coord. of the MENU, unit is font
 * 			y	y coord. of the MENU, unit is font
 * @return  none
 */
extern void
tw_osd2_clear_char (unsigned char x, unsigned char y);
/**
 * @func    tw_osd2_clear_line
 * @brief   clear(put blank font) a ROW of characters attribute in MENU.
 * @param  	y	y coord. of the MENU, unit is font
 * @return  none
 */
void
tw_osd2_clear_line (unsigned char y);
/**
 * @func    tw_osd2_menu_write_block
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * 			width	icon of width, unit is font
 * 			height	icon of height, unit is font
 * @return  none
 */
void
tw_osd2_clear_block (unsigned char x, unsigned char y, unsigned char width, unsigned char height);
/**
 * @func    tw_clear_icon
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICON structure of point
 * @return  none
 */
void
tw_clear_icon (struct tw_icon* picon);
/**
 * @func    tw_clear_icons
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICONS structure of point
 * @return  none
 */
void
tw_clear_icons (struct tw_icons* picon);
/**
 * @func    tw_clear_txt
 * @brief   clear a block in MENU with ICON.
 * @param  	*ptxt	TXT structure of point
 * @return  none
 */
void
tw_clear_txt (struct tw_txt* ptxt);
/**
 * @func    _tw_clear_icon_at
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICON structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_icon_at (struct tw_icon* picon, unsigned short menu_addr, unsigned char menu_width);
/**
 * @func    _tw_clear_icons_at
 * @brief   clear a block in MENU with ICONS.
 * @param  	*picon	ICONS structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_icons_at (struct tw_icons* picon, unsigned short menu_addr, unsigned char menu_width);
/**
 * @func    _tw_clear_txt_at
 * @brief   clear a block in MENU with ICON.
 * @param  	*ptxt	TXT structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_txt_at (struct tw_txt* ptxt, unsigned short menu_addr, unsigned char menu_width);
/**
 * @func    tw_osd2_clear_menu
 * @brief   clear(put blank index) OSD2 MENU.
 * @param  	none
 * @return  none
 */
void
tw_osd2_clear_menu (void);
/**
 * @func    tw_osd2_menu_start_h
 * @brief	set a specified ROW of H start position of the MENU.
 * @param   x		x coord. of the display, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			row		specified ROW of the MENU
 * @return  none
 */
extern void
tw_osd2_menu_start_h (unsigned short x);
/**
 * @func    tw_osd2_menu_location
 * @brief	set OSD2 MENU of location.
 * @param   x		x coord. of the MENU, unit is pixel
 * 			y		y coord. of the MENU, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			height	MENU of height (ROW)
 * @return  none
 */
extern void
tw_osd2_menu_location (unsigned short x, unsigned short y);



/**
 * @func    _tw_is_half_alphabet
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_is_half_alphabet();
/**
 * @func    _tw_half_alphabet_num
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_half_alphabet_num();
/**
 * @func    _tw_get_font_size_id
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_get_font_size_id();
/**
 * @func    _tw_get_file_type
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_get_file_type();
/**
 * @func    _Unicode_len
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned short _Unicode_len(const unsigned short *ucs);
/**
 * @func    _GB_code_len
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned short _GB_code_len(const unsigned short *ucs);
/**
 * @func    _get_offset_GBS
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned short _get_offset_GBS(unsigned short *ucs, unsigned short n);
/**
 * @func    _get_code_from_GBS
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned short _get_code_from_GBS(const unsigned short *ucs, unsigned short n);
/**
 * @func    _tw_get_font_quality
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_get_font_quality();
/**
 * @func    _tw_get_draw_len
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_get_draw_len(unsigned short *ucs, unsigned char width, unsigned char size_id);
/**
 * @func    _romfont_draw_at
 * @brief   .
 * @param   none
 * @return  none
 */
extern void _romfont_draw_at(struct tw_txt* ptxt, unsigned char x, unsigned char y, const unsigned char *ucs, unsigned short menu_addr, unsigned char menu_width, unsigned char color, unsigned char blink);
/**
 * @func    _romfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
extern void _romfont_draw(struct tw_txt* ptxt, unsigned char x, unsigned char y, const unsigned char *ucs, unsigned char color, unsigned char blink);
/**
 * @func    tw_romfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_romfont_draw(struct tw_txt* ptxt, const unsigned char *ucs);
/**
 * @func    _twfont_draw_at
 * @brief   .
 * @param   none
 * @return  none
 */
extern void _twfont_draw_at(struct tw_txt* ptxt, unsigned char x, unsigned char y, unsigned short *ucs, unsigned short menu_addr, unsigned char menu_width, unsigned char color, unsigned char blink);
/**
 * @func    _twfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
extern void _twfont_draw(struct tw_txt* ptxt, unsigned char x, unsigned char y, unsigned short *ucs, unsigned char color, unsigned char blink);
/**
 * @func    tw_twfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_twfont_draw(struct tw_txt* ptxt, unsigned short *ucs);
/**
 * @func    tw_text_draw
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_text_draw(struct tw_txt* ptxt, unsigned short *ucs);
/**
 * @func    tw_text_color_change
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_text_color_change(struct tw_txt* ptxt, unsigned short *ucs, unsigned char color);
/**
 * @func    _tw_draw_char_at
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char _tw_draw_char_at(unsigned char x, unsigned char y, unsigned char w, unsigned h,
				          unsigned short idx, unsigned char color, unsigned short menu_addr, unsigned char menu_width);
/**
 * @func    tw_menu_draw_nstring
 * @brief   .
 * @param   none
 * @return  none
 */
extern unsigned char tw_menu_draw_nstring(unsigned char x, unsigned char y, unsigned char max_w, const unsigned short *ucs, unsigned short menu_addr, unsigned char menu_width,
                                       unsigned char color, unsigned char size_id, unsigned char is2BP);
/**
 * @func    tw_menu_row_moveV
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_menu_row_moveV(unsigned char row, unsigned short y);
/**
 * @func    tw_menu_reset_index
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_menu_reset_index();
/**
 * @func    tw_menu_reset_index_n
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_menu_reset_index_n(unsigned char idx);
/**
 * @func    tw_menu_reset_index_rev_nums
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_menu_fill(unsigned char x, unsigned char y, unsigned char dat ,unsigned char color, unsigned char length);
/**
 * @func    tw_init_scrol
 * @brief   .
 * @param   none
 * @return  none
 */
extern struct tw_scl_txt tw_init_scrol(struct tw_txt* ptxt, unsigned short *ucs, unsigned char direct, unsigned char tail_blank);
/**
 * @func    tw_scrolling
 * @brief   .
 * @param   none
 * @return  none
 */
extern void tw_scrolling(struct tw_scl_txt* scl_txt);
/**
 * @func    get_cur_page_id
 * @brief   get current of page ID.
 * @param   none
 * @return  page ID if has active of page
 */
extern int get_cur_page_id (void);
/**
 * @func    put_enter_page_event
 * @brief   put event for enter page with specified page ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
extern void put_enter_page_event (unsigned char id);
/**
 * @func    put_quit_page_event
 * @brief   put event for quit page with specified page ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
extern void put_quit_page_event (unsigned char id);
/**
 * @func    put_enter_slide_page_event
 * @brief   put event for enter page by slide.
 * @param   unsigned char id	page ID
 * @return  none
 */
extern void put_enter_slide_page_event (unsigned char id);
/**
 * @func    put_quit_slide_page_event
 * @brief   put event for quit page by slide.
 * @param   unsigned char id	page ID
 * @return  none
 */
extern void put_quit_slide_page_event (unsigned char id);
/**
 * @func    put_enter_srv_event
 * @brief   put event for enter service with specified service ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
extern void put_enter_srv_event (unsigned char id);

extern unsigned char SARscans(unsigned char* sar);

extern void* tw_get_sys_ctx (void);

extern void* tw_get_au_ctx (void);
/**
 * @func    tw_check_event
 * @brief   check all event and put to event queue, call this function in main loop,
 * 			and anybody can get event. add check function in here
 * 			if you want receiver of event (definition in TW_EVENT).
 * @param   none
 * @return  none
 */
extern void
tw_check_event (void);

extern TW_EVENT*
tw_get_sys_evt (void);
/**
 * @func    tw_srv_process
 * @brief   service of process function.
 * @param   srv_handler			App of service handler
 * 			TW_EVENT* event		sys event of point
 * @return  0 if noting, or 1 if some thing
 */
extern unsigned char
tw_srv_process (tw_srv_handler_t srv_handler, TW_EVENT* event);
/**
 * @func    tw_service_handler
 * @brief   service of handler entry function.
 * @param   srv_handler		App of service handler
 * @return  0 if noting, or 1 if some error
 */
extern int
tw_service_handler (tw_srv_handler_t srv_handler);
/**
 * @func    tw_service_init
 * @brief   initialize for service of task with specified service ID.
 * @param   unsigned char service_id	service ID
 * @return  none
 */
extern void
tw_service_init (unsigned char service_id);
/**
 * @func    tw_page_process
 * @brief   widget page of process function.
 * @param   page_handler		App of page handler
 * 			TW_EVENT* event		sys event of point
 * @return  0 if noting, or 1 if some thing
 */
extern unsigned char
tw_page_process (tw_page_handler_t page_handler, TW_EVENT* event);
/**
 * @func    tw_task_handler
 * @brief   widget page of handler entry function.
 * @param   page_handler	App of page handler
 * @return  0 if noting, or 1 if some error
 */
extern int
tw_widget_handler (tw_page_handler_t page_handler);
/**
 * @func    tw_widget_init
 * @brief   initialize for widget page of task with specified page ID.
 * @param   unsigned char page_id		page ID
 * @return  none
 */
extern void
tw_widget_init (int page_id);
/**
 * @func    tw_task_handler
 * @brief   task of handler entry function, include page handler and service handler.
 * @param   void *sys_ctx				system context of point
 * 			void *au_ctx				audio context of point
 * 			unsigned char page_id		page ID
 * 			page_handler				App of page handler
 * @return  0 if noting, or 1 if some error
 */
extern int
tw_task_handler (int page_id, tw_page_handler_t page_handler);

/**
 * @func    put_event
 * @brief   put event to event queue.
 * @param   TW_EVENT* event		event (type for tw widget system) of point.
 * @return  none
 */
extern void put_event (TW_EVENT* event);
/**
 * @func    get_event
 * @brief   get event from event queue.
 * @param   none
 * @return  none type if not event, or return at event queue of point if has event
 */
extern TW_EVENT* get_event (void);
/**
 * @func    clear_event
 * @brief   clear all events at event queue.
 * @param   none
 * @return  none
 */
extern void clear_event(void);
/**
 * @func    srv_put_event
 * @brief   put event to event queue.
 * @param   TW_EVENT* event		event (type for tw widget system) of point.
 * @return  none
 */
extern void srv_put_event (TW_EVENT* event);
/**
 * @func    srv_get_event
 * @brief   get event from event queue.
 * @param   none
 * @return  none type if not event, or return at event queue of point if has event
 */
extern TW_EVENT* srv_get_event (void);
/**
 * @func    srv_clear_event
 * @brief   clear all events at event queue.
 * @param   none
 * @return  none
 */
extern void srv_clear_event(void);
/**
 * @func    tw_set_timer
 * @brief   set the msec and count of individual timer
 * @param   id		ID of timer
 * 			msec	millisecond
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_set_timer (int id, int msec);
/**
 * @func    tw_clear_timer
 * @brief   clear the state of individual timer
 * @param   id		ID of timer
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_clear_timer (int id);
/**
 * @func    tw_check_timer
 * @brief   check the count of individual timer
 * @param   id		ID of timer
 * @return  1 if timeout, 0 if noting
 */
extern int tw_check_timer (int id);
/**
 * @func    _get_point
 * @brief   get point from canvas
 * @param   x		x position
 * 			y		y position
 * 			dbuf	data buffer of point
 * 			width	buffer of width
 * 			height	buffer of height
 * 			bpp		bits per pixel
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
extern int
_get_point (int x, int y, char *dbuf, int w, int h, int bpp);
/**
 * @func    _canvas_get_point
 * @brief   get point from canvas
 * @param   x		x position
 * 			y		y position
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
extern int
_canvas_get_point (int x, int y);
/**
 * @func    _set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * 			dbuf	data buffer of point
 * 			width	buffer of width
 * 			height	buffer of height
 * 			bpp		bits per pixel
 * 			chkey	chrome key
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
extern void
_set_point (int x, int y, int idx, char *dbuf, int w, int h, int bpp, int chkey);
/**
 * @func    _canvas_set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
extern void
_canvas_set_point (int x, int y, int idx);
/**
 * @func    tw_canvas_set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_set_point (int x, int y, int idx);
/**
 * @func    _canvas_line
 * @brief   draw a line at canvas
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_line (int x0, int y0, int x1, int y1, int idx);
/**
 * @func    tw_canvas_line
 * @brief   draw a line at canvas and update
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_line (int x0, int y0, int x1, int y1, int idx);
/**
 * @func    _canvas_circle
 * @brief   draw a circle at canvas
 * @param   c_x		x0 position
 * 			c_y		y0 position
 * 			r		radius
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_circle (int c_x, int c_y, int r, int idx, int fill);
/**
 * @func    tw_canvas_circle
 * @brief   draw a circle at canvas and update
 * @param   c_x		x0 position
 * 			c_y		y0 position
 * 			r		radius
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_circle (int c_x, int c_y, int r, int idx, int fill);
/**
 * @func    _canvas_rectangle
 * @brief   draw a rectangle at canvas
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_rectangle (int x0, int y0, int x1, int y1, int idx, int fill);
/**
 * @func    tw_canvas_rectangle
 * @brief   draw a rectangle at canvas and update
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_rectangle (int x0, int y0, int x1, int y1, int idx, int fill);
/**
 * @func    _canvas_square
 * @brief   draw a square at canvas
 * @param   x		x position
 * 			y		y position
 * 			within	width
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_square (int x, int y, int within, int idx, int fill);
/**
 * @func    tw_canvas_square
 * @brief   draw a square at canvas and update
 * @param   x		x position
 * 			y		y position
 * 			within	width
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_square (int x, int y, int within, int idx, int fill);
/**
 * @func    _canvas_bmp
 * @brief   load a bmp at canvas
 * @param   x		x position
 * 			y		y position
 * 			w		BMP of width
 * 			h		BMP of height
 * 			bpp		BMP of bits per pixel*
 * 			src		BMP of source point
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_bmp (int x, int y, int w, int h, int bpp, unsigned char *src);
/**
 * @func    tw_canvas_bmp
 * @brief   load a bmp at canvas and update
 * @param   x		x position
 * 			y		y position
 * 			w		BMP of width
 * 			h		BMP of height
 * 			bpp		BMP of bits per pixel
 * 			src		BMP of source point
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_bmp (int x, int y, int w, int h, int bpp, unsigned char *src);
/**
 * @func    tw_canvas_update
 * @brief   canvas update
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_update (void);
/**
 * @func    _canvas_clear
 * @brief   canvas clear
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_clear (void);
/**
 * @func    tw_canvas_clear
 * @brief   canvas clear and update
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_clear (void);
/**
 * @func    _canvas_fill
 * @brief   canvas fill
 * @param   idx		fill index
 * @return  0 for successful, minus if any error occurred
 */
extern int
_canvas_fill (int idx);
/**
 * @func    tw_canvas_fill
 * @brief   canvas fill and update
 * @param   idx		fill index (color)
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_fill (int idx);
/**
 * @func    tw_canvas_config
 * @brief   configure a canvas (system) base on a canvas (app)
 * @param   canvas		canvas structure
 * @return  none
 */
extern void tw_canvas_config (struct tw_canvas *canvas);
/**
 * @func    tw_canvas_init
 * @brief   initialize canvas for OSD2 (memory allocation)
 * @param   canvas		canvas structure of point
 * @return  0 for successful, minus if any error occurred
 * @note	must release when unused
 */
extern int tw_canvas_init (struct tw_canvas *canvas);
/**
 * @func    tw_canvas_release
 * @brief   release canvas (release sequence memory)
 * @param   canvas		canvas structure of point
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_release (struct tw_canvas *canvas);
/**
 * @func    tw_canvas_enable
 * @brief   enable (display) a canvas on OSD2 MENU
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_enable (void);
/**
 * @func    tw_canvas_disable
 * @brief   disable a canvas on OSD2 MENU
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_disable (void);
/**
 * @func    tw_canvas_loc
 * @brief   setting canvas of location on OSD2 MENU
 * @param   x	x position
 * 			y	y position
 * @return  0 for successful, minus if any error occurred
 */
extern int tw_canvas_loc (int x, int y);
/**
 * @func    tw_canvas_dbuf
 * @brief   setting canvas of data buffer
 * @param   dbuf	data buffer of point
 * @return  none
 */
extern void tw_canvas_dbuf (void *dbuf);

extern void tw_sys_timer_clear (void);
extern void tw_sys_timer_start (void);
extern void tw_sys_timer_stop (void);

extern void tw_set_chk_rbell (int en);

extern void
tw_set_gpio_chk (tw_chk_gpio_event_t chk_func);

extern void
tw_set_uart_comm_chk (tw_chk_gpio_event_t chk_func);

extern void
tw_set_shutdown_time (unsigned int st);
extern unsigned int
tw_get_shutdown_time (void);
extern unsigned char mSarkeyStatus;
extern int mSarkeyCount ;

#endif	/* __TW_WIDGET_SYS_H__ */
