/**
 *  @file   app0.h
 *  @brief  header file of app0 info
 *  $Id: app0.h,v 1.2 2014/07/18 03:12:44 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2011/06/09  New file.
 *
 */
#ifndef __APP0_H
#define __APP0_H

/*
 * AVI App0 Info
 */
#define JPEG_USER_INFO_LEN      34
#define JAPP_LEN                (JPEG_USER_INFO_LEN + 24)
struct jhdr_app0_t {
    uint16_t    marker;        /* APP0 */
    uint16_t    sz;            /* length */
    uint8_t     data[JAPP_LEN];
    uint8_t     junk;
} __attribute__((packed));


/*
 * Jpeg App0 Info
 */
#define USER_INFO_LEN 32
#define SYS_INFO_SIZE 22 //add 2 tag size
#define USER_INFO_SIZE 32 //add 2 tag size
#pragma pack(push, 1)
struct app0_info{
	unsigned char sys_tag;  
	unsigned char sys_size; 
	unsigned char q;//quality
	unsigned char f;//field
	unsigned short top_size;//top_field_size
	unsigned short bottom_size;//bottom_field_size
	unsigned int time;//time
	unsigned int threshold;//for SNR threshold
	unsigned int total_size;//file total size
	unsigned int temp;
	unsigned char user_tag;
	unsigned char user_size;
	char user_info[USER_INFO_LEN];
};
#pragma pack(pop)

//[tag-sys[S]][size][sys_info][tag-user(U)][size][user_info]
//   1 - 1 - 22 - 1 - 1- 32
//total info size = 58 bytes
//[S][22][sys_info][u][32][user_info]

#endif /* __APP0_H */
