/*
 *  @file   registry2.c
 *  @brief  registry2 for nor 
 *  $Id $
 *  $Author dos $
 *  $Revision: 1.6 $
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
#include <debug.h>
#include <errno.h>
#include <unistd.h>
#include <registry2.h>
#include <nor.h>
#include <config.h>
#include <sys.h>
#include <cache.h>
#include <config.h>
#include <heap.h>

struct registry2 reg_t;

extern unsigned int _fregistry;

int nor2_read(char *buf, int len, int pu, int off)
{   
    int i;
	
    /* read back */
    memset(buf, 0, len);
    for (i = 0; i < len; i++) {
        buf[i] = readb((pu|NOR_DATA_BASE) + off +i);   
    }
	
    return len;
}

int nor2_write(char *buf, int len, int pu, int off)
{
    int rc = 0;  
    flush_dcache_all();
    if(reg_t.offset == 0) {
        rc = nor_block_erase(pu);
		dbg(0,"block erase\n");
        assert(rc == 0);
    }
    nor_data_write(pu + off, len, (char *)buf);
    
    return len;
}

int registry2_init(void)
{
	char buff[reg_info_size];//id(1)+length(4)+makr(1)
	int tag = 0x00;
	char mark = 0;
	int length = 0;
	if(reg_t.configured == 1)
		return -1;
	memset(&reg_t,0xff,sizeof(struct registry2));
	reg_t.offset=0;
	
	reg_t.pu = (unsigned int)&_fregistry;
	nor2_read(buff,reg_info_size,reg_t.pu,reg_t.offset);
	tag = (int)buff[0];
	length = (int)(buff[1]<<0|buff[2]<<8|buff[3]<<16|buff[4]<<24);
	if(tag != 0xff){
		nor2_read(&mark,1,reg_t.pu,reg_t.offset+reg_info_size+length-1);
	}else{
		dbg(0,"Registry2 : No Data\n");
		mark = 0xff;
	}
	reg_t.configured = 1;
	while(mark == mark_value){  //mark value
		reg_t.buff[tag] = reg_t.offset;//(int)(buff[1]<<24|buff[2]<<16|buff[3]<<8|buff[4]);
		reg_t.bitmark[(tag/32)] &= ~(1<<((tag)%32));
		reg_t.offset += (length + reg_info_size);
		nor2_read(buff,reg_info_size,reg_t.pu,reg_t.offset);
		tag = (int)buff[0];
		length = (int)(buff[1]<<0|buff[2]<<8|buff[3]<<16|buff[4]<<24);
		nor2_read(&mark, 1, reg_t.pu, reg_t.offset+reg_info_size+length-1);
	}
	return 0;
}

int registry2_set_single(int tag,int value)
{
	int rc =0;
	char buff[10]={0};
	int length = 4;
	buff[0]=tag&0xff;  //[id:1][length:4][data:4][mark:1]

	buff[1]=(length>>0)&0xff;
	buff[2]=(length>>8)&0xff;
	buff[3]=(length>>16)&0xff;
	buff[4]=(length>>24)&0xff;
	
	buff[5]=(value>>0)&0xff;
	buff[6]=(value>>8)&0xff;
	buff[7]=(value>>16)&0xff;
	buff[8]=(value>>24)&0xff;
	
	buff[9]=0;//mark

	nor_writeprotect(0);
	
	reg_t.buff[tag] = reg_t.offset;
	reg_t.bitmark[(tag/32)] &= ~(1<<((tag)%32));
	rc = nor2_write(buff,reg_info_size+sizeof(int), reg_t.pu, reg_t.offset);
	reg_t.offset += (reg_info_size + sizeof(int));//4 = data size
	
	nor_writeprotect(1);
	return 0;
}

int registry2_set_multi(int tag,char *value,int length)
{
	int rc =0;
	char buff[5]={0};
	char mark = 0;
	buff[0]=tag&0xff;

	buff[1]=(length>>0)&0xff;
	buff[2]=(length>>8)&0xff;
	buff[3]=(length>>16)&0xff;
	buff[4]=(length>>24)&0xff;

	nor_writeprotect(0);
	
	reg_t.buff[tag] = reg_t.offset;//physical address
	reg_t.bitmark[(tag/32)] &= ~(1<<((tag)%32));
	rc = nor2_write(buff,reg_info_size-1, reg_t.pu, reg_t.offset);
	
	reg_t.offset += (reg_info_size -1);
	rc = nor2_write(value,length, reg_t.pu, reg_t.offset);
	
	reg_t.offset += length;
	rc = nor2_write(&mark,sizeof(char), reg_t.pu, reg_t.offset);
	
	reg_t.offset += 1;
	
	nor_writeprotect(1);
	return 0;
}

int registry2_get_multi(int tag,char *value,int length)
{
	int rc =0;
	char buff[reg_info_size] = {0};
	int size = 0;
	if(tag>=reg_size)
		return -1;
	
	if((reg_t.bitmark[(tag/32)] & (1<<tag%32)) == 0){
		nor2_read(buff,reg_info_size,reg_t.pu,reg_t.buff[tag]);
		size = (int)(buff[1]<<0|buff[2]<<8|buff[3]<<16|buff[4]<<24);
		if((size != length) && (length != 0x12345678))
			rc = -1;
		else{
			nor2_read(value,size,reg_t.pu,reg_t.buff[tag] + reg_info_size -1);
			rc = size;
		}
	}else{
		rc = -1;
	}
	return rc;
}

int registry_read_back(char *p)//read back only get data
{
	int i = 0;
	int length = 0;
	int total_length = 0;
	char buff[6] = {0};
	for(i=0;i<reg_size;i++){
		if((reg_t.bitmark[(i/32)] & (1<<i%32)) == 0){
			nor2_read(buff,reg_info_size,reg_t.pu,reg_t.buff[i]);
			length = (int)(buff[1]<<0|buff[2]<<8|buff[3]<<16|buff[4]<<24);
			length = reg_info_size + length;
			nor2_read(p,length,reg_t.pu,reg_t.buff[i]);
			p += length;
			total_length +=length;
		}
	}
	return total_length;
}

int registry2_set_all(void)
{
	int rc = 0;
	char *p = heap_alloc(64*1024);
	int total_size = 0;
	
	nor_writeprotect(0);
	
	reg_t.offset = 0;
	memset(p,0,64*1024);
	total_size = registry_read_back(p);
	rc = nor2_write(p,total_size, reg_t.pu, reg_t.offset);
	heap_release(p);
	
	reg_t.configured = 0;
	registry2_init();
	
	nor_writeprotect(1);
	return rc;
}

int _registry2_set(int tag,int value)
{
	int rc =0;
	if(reg_t.configured != 1){
		registry2_init();
	}
	if(tag>=reg_size)
		goto EXIT;
	
	if(reg_t.offset >= reg_limit_size){
		registry2_set_single(tag,value);//save value first
		registry2_set_all();//read back and write back
	}else{
		registry2_set_single(tag,value);
	}
	
	return rc;
EXIT:
	return -1;
}

int _registry2_get(int tag,int *value,int default_value)
{
	int rc =0;
	char buff[10] = {0};
	
	if(tag>=reg_size)
		return -1;
	
	if((reg_t.bitmark[(tag/32)] & (1<<tag%32)) == 0){
		nor2_read(buff,reg_info_size + sizeof(int),reg_t.pu,reg_t.buff[tag]);
		*value = (int)(buff[5]<<0|buff[6]<<8|buff[7]<<16|buff[8]<<24);
	}else{
		*value = default_value;
		rc = -1;
	}
	return rc;
}

int registry2_get(int tag,int *value,int default_value)
{
	int tag_id = 0;
	tag_id =tag +sys_num;
	if(tag_id>=reg_size)
		return -1;
	else
		return _registry2_get(tag_id,value,default_value);
}

int registry2_set(int tag,int value)
{
	int tag_id = 0;
	
	tag_id =tag +sys_num;
	if(tag_id>=reg_size)
		return -1;
	else
		return _registry2_set(tag_id,value);
}

int registry2_burst_set(int tag, int *value, int cnt)
{
	int rc = 0;
	int i = 0;
	int tag_id = 0;

	nor_writeprotect(0);

	for(i = 0; i < cnt; i++) {
		tag_id = tag +sys_num;
		if(tag_id >= reg_size) {
			rc = -1;
			goto EXIT;
		} else {
			rc = _registry2_set(tag_id, value[i]);
		}
		tag++;
	}

EXIT:
	nor_writeprotect(1);
	return rc;
}

void reg_hexdump(char *buff,int length)
{
	int i = 0;
	for(i = 0; i < length; i++){
		if(i%16 == 0)
			printf("\n");
		printf("0x%02x ",buff[i]);
	}
	printf("\n");
}

void registry2_dump(void)
{
	int i=0 ;
	char buff[262] = {0};
	int length = 0;
	printf("offset = %x\n",reg_t.offset);
	printf("flash address = %x\n",reg_t.pu);
	for(i=0;i<reg_size;i++){
		if((reg_t.bitmark[(i/32)] & (1<<i%32)) == 0){
			printf("ID = %x\n",i);
			memset(buff,0,sizeof(buff));
			length = registry2_get_multi(i,buff,0x12345678);
			reg_hexdump(buff,length);
		}
	}
}

int registry2_getn(int tag,void *buf,int length)
{
	char *buf_in = (char *)buf;
	int tag_id = 0;
	tag_id =tag +sys_num;
	if(tag_id >= reg_size)
		return -1;
	else{
		if( registry2_get_multi(tag_id,buf_in,length) >= 0)
			return 0;
		else
			return -1;
	}
}

int registry2_setn(int tag,void *buf,int length)
{
	char *buf_in = (char *)buf;
	int tag_id = 0;
	tag_id =tag +sys_num;
	if(length > 256){
		dbg(0,"Error: Exceed 256 bytes\n");
		return -1;
	}
	if(tag_id >= reg_size)
		return -1;
	else
		return registry2_set_multi(tag_id,buf_in,length);
}
