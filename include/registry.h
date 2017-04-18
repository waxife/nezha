/*
 *  @file   registry.h
 *  @brief  header file of registry
 *  $Id $
 *  $Author $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2009/07/27  New file.(Sherman Chen)
 *
 */
#ifndef __REGISTRY_H
#define __REGISTRY_H

//#define ST_TYPE_FILE        1
#define ST_TYPE_NAND        2
#define ST_TYPE_NOR         3

#define MAGIC_REGISTRY      0x34a81823
#define MAX_ITEMS           8

#define RF_DIRTY            (1 << 0)
#define RF_INIT_SCAN        (1 << 1)

#define ST_FLUSH_WRITE      0
#define ST_APPEND_WRITE     1

struct registry_item_t {
    uint32_t    flag;
    char        name[4];
    void        *val;
};

struct p_nand_t {
    int         bsize;
    int         psize;
    void        *ctx;
};

struct reg_conf_t {
    int             max_items;
    int             ba;
    int             max_reg_size;
    int             max_file_size;
    int             st_type;   
};

struct registry_t {
    int             magic;
    int16_t         state;
    uint16_t        timestamp;
    int             active_file;    /* For nand, it means pu. For nor, it means ba */ 
    int             active_off;     /* For nand, it means page. For nor, it means offset */
    int             flush_len;
    int             nitems;
    int             reg_len;
    struct registry_item_t  item[8];   
    struct reg_conf_t       *conf;    
};


extern void *reg_config_nor(int ba);
extern int reg_init(void *reg_conf);
extern int reg_get(const char *name, void *val);
extern int reg_put(const char *name, void *val);
extern int reg_flush(void);
extern int reg_release(void);
extern int reg_items(int *nitems);
extern int reg_idx_get(int idx, char *name, void *val);
extern int is_configured(void);
extern int reg_dump(void);

extern int registry_write(const unsigned char *name,int value);

extern int registry_read(const unsigned char *name,int *value);

extern int registry_flush();

enum {
    ERR_INVALID_TYPE    = 2100,
    ERR_INVALID_VALUE   = 2101,
    ERR_FAILED_IO       = 2102,
    ERR_RESOURCE_LIMIT  = 2103,
    ERR_NO_MEMORY       = 2104,
};



#endif

