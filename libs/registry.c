/*
 *  @file   registry.c
 *  @brief  registry for nor 
 *  $Id $
 *  $Author dora $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30  New file.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define DBG_LEVEL   0
#include <debug.h>
#include <errno.h>
#include <unistd.h>
#include <registry.h>
#include <nor.h>
#include <config.h>
#include <sys.h>
#include <cache.h>

#define     reg_ver     "v1.00"
#define     reg_date    "2011/05/26"

#define m_file_size 64*1024
#define m_reg_size 80
#define m_items 8

#ifdef CONFIG_REG_STORE_BLOCK
struct reg_conf_t g_conf;
struct registry_t g_reg;

int reg_nor_read(struct registry_t *reg, char *buf, int len, int pu, int off)
{   
    int i;
    /* read back */
    memset(buf, 0, len);
    for (i = 0; i < len; i++) {
        buf[i] = readb((pu|NOR_DATA_BASE) + off +i);   
    }
    
    return len;
}


int reg_nor_write(struct registry_t *reg, char *buf, int len, int pu, int off)
{
    int rc = 0;  
    
    if(off == 0) {
        rc = nor_block_erase(pu);
        assert(rc == 0);
    }
    nor_data_write(pu + off, len, (char *)buf);
    
    return len;
}

void *reg_config_nor(int ba)
{
    struct registry_t *r = &g_reg;
    struct reg_conf_t *c = &g_conf;
    c->max_items = m_items; 
    c->ba = ba;
    c->max_reg_size = m_reg_size;
    c->max_file_size = m_file_size;
    c->st_type = ST_TYPE_NOR;
    r->conf = c;
    return (void *)c;   
}


int reg_parse_item(struct registry_t *reg, char *p, int *reg_len)
{
    struct registry_t *r = reg;
    struct reg_conf_t *c = r->conf;
    int name_len, val_len;
    int lrc = -1, nitems;
    char *q, *pval, *pname, *ptimestamp;
    
    name_len = val_len = 0;
    nitems = r->nitems;  //0

    if (strncmp(p, ">>", 2) == 0) {                                                 
        q = strchr(p, '\n');
        *q = '\0';                  
        ptimestamp = p + 2; 
        r->timestamp = strtol(ptimestamp, NULL, 0); 
    }else{
        goto NEXT_LINE; 
    }
        
        p = q + 1;  //point to first name   

    //while((*p != 0) || (*p != '<'))
    while(*p != 0)
    {
        pname = p;
        
        q = strchr(p, '=');
        *q = '\0';  
        
        name_len = strlen(p);
        
        strncpy(r->item[nitems].name, pname, name_len);
        
        pval = q + 1;
        
        q = strchr(pval, '\n');
        
        r->item[nitems].val = (void *)(strtol(pval, NULL, 0));
        r->item[nitems].flag = RF_INIT_SCAN;
        val_len = 10; 
            
        nitems++;
        if (nitems > c->max_items) {
            lrc = -ERR_RESOURCE_LIMIT;
            goto EXIT;
        }
        p = q + 1;
    }


    r->nitems = nitems;

NEXT_LINE:
    return 0;
    
EXIT:
    return lrc; 
}

static int reg_read(char *buf, int len, int pu, int off)
{
    struct registry_t *r = &g_reg;
    //struct reg_conf_t *c = r->conf;
    int rc = -1;
    
    rc = reg_nor_read(r, buf, len, pu, off);
    
    if (rc < 0) {
        dbg(0, "Failed to read, file %d offset 0x%x \n", pu, off);
        return -1;
    }
        
    return rc;
}

static int reg_write(char *buf, int len, int pu, int off)
{
    struct registry_t *r = &g_reg;
    int rc = -1;
    
    rc = reg_nor_write(r, buf, len, pu, off);   
    if (rc < 0) {
        dbg(0, "Failed to write, file %d offset 0x%x \n", pu, off);
        return -1;
    }
        
    return rc;
}
/*
 * registry file format
 *    file format
 *    >>timestamp1\n
 *    name1=value1\n
 *    name2=value2\n
 *    <<timestamp1\n
 *    ...
 *    >>timestampX\n
 *    nameX=valueX\n
 *    <<timestampX\n
 */

/* NOTE:
 * 1. the type defined by reg_get functon is stronger than item type in registry file.
 *    If type of item is defined by registry file, the item should be mark RF_INIT flag. 
 * 2. timestamp can not be zero 
 */


/*
 * reg_init : init registry data struct and rebuild registry elements from specified
 *            registry files
 * @reg_conf - input configuration for nor
 * Return < 0 error, = 0 success
 *   rc = -EINVAL - invalid input argument
 */
int reg_init(void *reg_conf)
{
    struct registry_t *r = &g_reg;
    struct reg_conf_t *c = r->conf;
    int reg_len, ba;
    int rc, lrc;
    //int i; //test
    int dlen;
    int file_size, reg_size, off_reg = 0, off_test = 0;
    //char rbuf[c->max_reg_size*4]; //test 
    char line[c->max_reg_size], *p;
    uint16_t t1 = 0, t2 = 0, t3 = 0;
    
   
    dbg(5, "init>>>> max_items %d ba %x max_reg_size 0x%x max_file_size 0x%x \n", c->max_items, 
            c->ba, c->max_reg_size, c->max_file_size);

    lrc = -1;
    
    if(c->ba <= 0)    	
    	return -EINVAL;
    
    if (c->max_items <= 0)
        return -EINVAL;

    if (c->max_items > MAX_ITEMS)
        return -ERR_RESOURCE_LIMIT;
    
    r->state = ST_APPEND_WRITE;
    r->flush_len = 0;
    r->nitems = 0;
    reg_len = 0;
    ba = c->ba;

    
    file_size = c->max_file_size;
    if (file_size == 0) {
        dbg(0, "ERR_INVALID_VALUE \n");
        lrc = -ERR_INVALID_VALUE;
        goto EXIT;
    }
    
    reg_size = c->max_reg_size;
    if (reg_size == 0) {
            dbg(0, "ERR_INVALID_VALUE \n");
            lrc = -ERR_INVALID_VALUE;
            goto EXIT;
    }
    
    //file_size = reg_size*4; //test
    
//start scan nor: test
    /*
    memset(rbuf, 0, reg_size*4); //test
    dlen = reg_read(rbuf, reg_size*4, ba, 0);
    if (dlen < reg_size*4) {
        lrc = -ERR_FAILED_IO;
        dbg(0, "Failed to write, dlen 0x%x \n", dlen);
        goto EXIT;  
    }
    memdump(rbuf, reg_size*4); //test
    */
    
    while(file_size > 0){
        t1 = readb((ba|NOR_DATA_BASE) + 2 + off_reg); 
        t2 = readb((ba|NOR_DATA_BASE) + reg_size + off_reg - 2);
        t3 = readb((ba|NOR_DATA_BASE) + reg_size + off_reg + 2);
       
        if(t1 == 255 || t2 == 255){ //ff
            off_test = off_reg - reg_size;
            dlen = reg_read(line, reg_size, ba, off_test);
            if (dlen < reg_size) {
                lrc = -ERR_FAILED_IO;
                dbg(0, "Failed to write, dlen 0x%x \n", dlen);
                goto EXIT;  
            }
            break;
        }else if(t1 != 255 && t2 == 255 && t3 == 255){ //incompleted nor write
        	off_test = off_reg - reg_size;
            dlen = reg_read(line, reg_size, ba, off_test);
            if (dlen < reg_size) {
                lrc = -ERR_FAILED_IO;
                dbg(0, "Failed to write, dlen 0x%x \n", dlen);
                goto EXIT;  
            }
            break;
        }else{
            //The last registry file     
            off_test = off_reg;
            dlen = reg_read(line, reg_size, ba, off_test);
            if (dlen < reg_size) {
                lrc = -ERR_FAILED_IO;
                dbg(0, "Failed to write, dlen 0x%x \n", dlen);
                goto EXIT;  
            }
        }           
        off_reg += reg_size;
        file_size -= reg_size;
    }
    
   
    //memdump(line, reg_size);  //test
    
    p = line;
    
    if(line[0] != 255){
        rc = reg_parse_item(r, p, &reg_len);
        if (rc < 0)
            goto EXIT;
    }
//end scan
    
    if(r->active_off > file_size){
    //if(r->active_off >= 80*4){ //test  
        r->active_off = 0;
        r->timestamp = 0;
    }
    r->active_file = ba;
    r->magic = MAGIC_REGISTRY;
    
dbg(5, "nitems %d active_file %x active_off 0x%x ts %d \n", r->nitems, r->active_file, r->active_off, r->timestamp);

    return 0;

EXIT:   

    return lrc;
}


int reg_get(const char *name, void *val)
{
    int i;
    struct registry_t *r = &g_reg;

    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL;

    
    for (i = 0; i < r->nitems; i++) {
        if (strncmp(r->item[i].name, name, 4) == 0) {
            goto FOUND;         
        }
    }
    
    *(void **)val = (void **)-1;
    
    return 0;
    
FOUND:  
    
    *(void **)val = r->item[i].val;
    
    return 0;
}

int reg_items(int *nitems)
{
    struct registry_t *r = &g_reg;
    
    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL; 
    
    *nitems = r->nitems;
    
    return 0;
}

int reg_idx_get(int idx, char *name, void *val)
{
    struct registry_t *r = &g_reg;
    
    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL; 
        
    if (idx >= r->nitems)
        return -1;
        
    strncpy(name, r->item[idx].name, 4);
        
    *(void **)val = r->item[idx].val;
        
    return 0;
    
}

int reg_put(const char *name, void *val)
{
    int lrc, name_len, val_len;
    int i, found = 0;
    struct registry_t *r = &g_reg;
    struct reg_conf_t *c = r->conf;
    char *fname;
   
    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL;

    name_len = val_len = 0;
    
    for (i = 0; i < r->nitems; i++) {
        if (strncmp(r->item[i].name, name, 4) == 0) {           
            found = 1;
            goto FOUND;
        }
    }
    
    r->nitems++;
 
    if (r->nitems > c->max_items) {
    	r->nitems = r->nitems-1;
        lrc = -ERR_RESOURCE_LIMIT;
        goto EXIT;
    }
    
    strncpy(r->item[i].name, name, 4);
    
    
    
FOUND:
    fname = r->item[i].name;
    name_len = strlen(fname);
   
    r->item[i].val = val;        
    val_len = 10;
    
    if (!found) {
        if (c->st_type != ST_TYPE_NAND) {
            r->item[i].flag ^= RF_INIT_SCAN;
            r->item[i].flag |= RF_DIRTY;
        }
        r->reg_len += name_len + val_len + 2; /* 2 means '=' and '\n' characters */     
    }

dbg(0, "reg_len: %d \n", r->reg_len);

    if (r->reg_len > c->max_reg_size) {
        dbg(0, "ERROR: registry size is beyond max registry size \n");
        lrc = ERR_RESOURCE_LIMIT;
        goto EXIT;
    }   
    
    return 0;

EXIT:
    return lrc;
}

#if 1
int reg_flush(void)
{
    struct registry_t *r = &g_reg;
    struct reg_conf_t *c = r->conf;
    uint16_t timestamp;
    char buf[c->max_reg_size];
    int p, i, flush_write = 0;
    int lrc, dlen;
    int file_size;
    //int flush_size =0; 
    //int char_count = 0;
    //char *char_p = NULL;
 

    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL;

	flush_dcache_all();  /*prevent catch old cached memory*/

    if (r->active_off == 0)
        r->state = ST_APPEND_WRITE;
    
    /* create reg buffer */
    memset(buf, 0, c->max_reg_size);
    
    if (r->state == ST_APPEND_WRITE)
        flush_write = 1; 

    p = 0;
    timestamp = r->timestamp + 1;
    r->timestamp++;
    if (flush_write) {        
        p += sprintf((char *)(buf + p), ">>%d\n", timestamp);   
    dbg(2, "WWWWWW :>>%d\\n\n", timestamp);
    }
    
    for (i = 0;i < r->nitems; i++) {
        char name[4];
        void *val;

        if (!flush_write && ((r->item[i].flag & RF_DIRTY) == 0))
            continue;
        //name = r->item[i].name;
        strncpy(name, r->item[i].name, 4);
        val = r->item[i].val;        
        p += sprintf((char *)(buf + p), "%s=%d\n", name, (int)val);
        dbg(2, "WWWWWW :%s=%d\\n\n", name, (int)r->item[i].val);
        r->item[i].flag &= ~(RF_DIRTY); 
    }
    
    // Calculate number of characters
    /*char_p = buf;
    while(*char_p != '\0'){
		char_count++;
		char_p++;
	}	
	char_count = char_count/2;*/

	p += sprintf((char *)(buf + c->max_reg_size - 4), "<<%d\n", timestamp);
    //p += sprintf((char *)(buf + c->max_reg_size - (char_count+3)), "<<%d\n", timestamp); 
    dbg(2, "WWWWWW :<<%d\\n\n", timestamp);
    
    //flush_size = get_flush_size(p);
    
    //dump buf
	/*
	dbg(2, "dump buf: \n"); 
    memdump(buf, 80);
    */
    
    file_size = c->max_file_size;
    if (file_size == 0) {
        dbg(0, "ERR_INVALID_VALUE \n");
        lrc = -ERR_INVALID_VALUE;
        goto EXIT;
    }
    
    //test
    if(r->active_off >= file_size){
    //if(r->active_off >= 80*4){  
        r->active_off = 0;
    }
   
    dlen = reg_write(buf, c->max_reg_size, r->active_file, r->active_off);
    if (dlen < c->max_reg_size) {
        lrc = -ERR_FAILED_IO;
        //dbg(0, "Failed to write, flush_size 0x%x dlen 0x%x \n", flush_size, dlen);
        goto EXIT;  
    }

    r->flush_len = 0;
    r->reg_len = 0;
    r->active_off += c->max_reg_size;
    r->nitems = 0;
	
	
    
dbg(0, "r->active_off 0x%x \n", r->active_off);

	//add by dos
	reg_config_nor(CONFIG_REG_STORE_BLOCK*64*1024);
	lrc = reg_init(&g_conf);
	if(lrc !=0)
		goto EXIT;
	//
    return 0;

EXIT:
    
    return lrc;
}

#endif

int is_configured(void)
{
    struct registry_t *r = &g_reg;
    
    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL;
        
    return 0;
}


int reg_dump(void)
{
	int i, nitems, rc;
    char name[4];
    void *val;
 
    rc = reg_items(&nitems);
    if (rc < 0)
        return -1;
    
    i = 0;
    printf("Name              Value       \n");
    printf("----------------  ----------\n");
    for (i = 0; i < nitems; i++) {
        rc = reg_idx_get(i, name, &val);
        if (rc < 0)
            return -1;
        printf("%-16s  %d \n", name, (int)val);
    }
        
    return 0;	
}


int reg_flush2(void)
{
    struct registry_t *r = &g_reg;
    struct reg_conf_t *c = r->conf;
    uint16_t timestamp;
    char buf[c->max_reg_size];
    int p, i, flush_write = 0;
    int lrc, dlen;
    int file_size;
    //int flush_size =0;
 

    if (r == NULL || r->magic != MAGIC_REGISTRY)
        return -EINVAL;

	flush_dcache_all();  /*prevent catch old cached memory*/

    if (r->active_off == 0)
        r->state = ST_APPEND_WRITE;
    
    /* create reg buffer */
    memset(buf, 0, c->max_reg_size);
    
    if (r->state == ST_APPEND_WRITE)
        flush_write = 1; 

    p = 0;
    timestamp = r->timestamp + 1;
    r->timestamp++;
    if (flush_write) {        
        p += sprintf((char *)(buf + p), ">>%d\n", timestamp);   
    dbg(2, "WWWWWW :>>%d\\n\n", timestamp);
    }
    
    for (i = 0;i < r->nitems; i++) {
        char name[4];
        void *val;

        if (!flush_write && ((r->item[i].flag & RF_DIRTY) == 0))
            continue;
        //name = r->item[i].name;
        strncpy(name, r->item[i].name, 4);
        val = r->item[i].val;        
        p += sprintf((char *)(buf + p), "%s=%d\n", name, (int)val);
        dbg(2, "WWWWWW :%s=%d\\n\n", name, (int)r->item[i].val);
        r->item[i].flag &= ~(RF_DIRTY); 
    }
    
    //p += sprintf((char *)(buf + c->max_reg_size - 4), "<<%d\n", timestamp); 
    //dbg(2, "WWWWWW :<<%d\\n\n", timestamp);
    
    //flush_size = get_flush_size(p);
    
    //dump buf
	/*
	dbg(2, "dump buf: \n"); 
    memdump(buf, 80);
    */
    
    file_size = c->max_file_size;
    if (file_size == 0) {
        dbg(0, "ERR_INVALID_VALUE \n");
        lrc = -ERR_INVALID_VALUE;
        goto EXIT;
    }
    
    //test
    if(r->active_off >= file_size){
    //if(r->active_off >= 80*4){  
        r->active_off = 0;
    }
   
    dlen = reg_write(buf, c->max_reg_size, r->active_file, r->active_off);
    if (dlen < c->max_reg_size) {
        lrc = -ERR_FAILED_IO;
        //dbg(0, "Failed to write, flush_size 0x%x dlen 0x%x \n", flush_size, dlen);
        goto EXIT;  
    }

    r->flush_len = 0;
    r->reg_len = 0;
    r->active_off += c->max_reg_size;
    r->nitems = 0;
	

    
dbg(2, "r->active_off 0x%x \n", r->active_off);
    return 0;

EXIT:
    
    return lrc;
}
#endif

int registry_write(const unsigned char *name,int value)
{
	 int rc =0;
	 rc = reg_put(name,(int *)value);
	 return rc;
}

int registry_read(const unsigned char *name,int *value)
{
	int rc =0;
	void *val = NULL;
	rc = reg_get(name,&val);
	*value = (int)val;
	return rc;
}

int registry_flush()
{
	int rc =0;
	rc =reg_flush();
	return rc;
}
