/**
 *  @file   sh_registry2.c
 *  @brief  configure registry for nand or nor flash
 *  $Id: 
 *  $Author: dos1236 $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30    dora    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <cache.h>
#include <ctype.h>
#include <unistd.h>
#include <debug.h>
#include <io.h>
#include <registry2.h>
#include <serial.h>
#include <nor.h>

command_init (sh_reg2_init, "reg2_init", "reg2_init");
command_init (sh_reg2_set, "reg2_set", "reg2_set [tag] [value]");
command_init (sh_reg2_get, "reg2_get", "reg2_get [tag]");
command_init (sh_reg2_dump, "reg2_dump", "reg2_dump");
command_init (sh_reg2_test, "reg2_test", "reg2_test");

command_init (sh_reg2_setn, "reg2_setn", "reg2_setn");
command_init (sh_reg2_getn, "reg2_getn", "reg2_getn");

command_init (sh_reg2_reset, "reg2_reset", "reg2_reset");

static int sh_reg2_init(int argc, char **argv, void **result, void *sys_ctx)
{ 
	int rc =0;
	rc = registry2_init();
	if(rc == 0)
		printf("registry2 initial successfully\n");
	else
		printf("registry2 is already configured\n");
    return 0;
}

static int sh_reg2_set(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int tag = 0;
    int rc = 0;
	int value;

    if (argc < 3) {
		printf("Need three argument\n");
        rc = -1;
        goto EXIT;
    }

    tag = atoi(argv[1]);
	value = atoi(argv[2]);
	if(tag > (reg_size-1)){
		printf("the size is too big\n");
		goto EXIT;
	}
	rc = registry2_set(tag,value);
	if(rc == 0)
		printf("get setting tag = %d value = %d\n",tag,value);
	else
		printf("get default tag = %d value = %d\n",tag,value);
EXIT:
    return rc;
}

static int sh_reg2_get(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int tag = 0;
    int rc = 0;
	int value = 0;

    if (argc < 2) {
		printf("Need two argument\n");
        rc = -1;
        goto EXIT;
    }
	
    tag = atoi(argv[1]);
	if(tag > (reg_size-1)){
		printf("the size is too big\n");
		goto EXIT;
	}
	rc = registry2_get(tag,&value, 0xff);
	if(rc == 0)
		printf("get setting tag = %d value = %d\n",tag,value);
	else
		printf("get default tag = %d value = %d\n",tag,value);
EXIT:
    return rc;
}

static int sh_reg2_dump(int argc, char **argv, void **result, void *sys_ctx)
{   
	registry2_dump();
    return 0;
}

static int sh_reg2_reset(int argc, char **argv, void **result, void *sys_ctx)
{   
	registry2_set_all();
    return 0;
}

static int sh_reg2_test(int argc, char **argv, void **result, void *sys_ctx)
{
	int i = 0;
	int value = 0;
	for(i = 5;i<reg_size;i++){
		registry2_set(i,i);
	}
	for(i = 0;i<reg_size;i++){
		registry2_get(i,&value, 0xff);
		printf("i=%d valur=%d\n",i,value);
	}
	return 0;
}

struct reg_2_set{
	char a[12];
};
struct reg_2_set_int{
	int a;
	int b;
	int c;
};

static int sh_reg2_setn(int argc, char **argv, void **result, void *sys_ctx)
{   
	struct reg_2_set test;
	struct reg_2_set_int test2;
	int rc = 0;
	memset(&test,0,sizeof(struct reg_2_set));
	memset(&test2,0,sizeof(struct reg_2_set_int));
	sprintf(test.a,"%s","abcdefg");
	test2.a = 1;
	test2.b = 2;
	test2.c = 3;
	printf("test.a = %s\n",test.a);
	rc = registry2_setn(10,&test,sizeof(struct reg_2_set));
	printf("rc = %d\n",rc);
	printf("test.a = %d .b = %d .c =%d\n",test2.a,test2.b,test2.c);
	rc = registry2_setn(11,&test2,sizeof(struct reg_2_set_int));
	printf("rc = %d\n",rc);
    return 0;
}

static int sh_reg2_getn(int argc, char **argv, void **result, void *sys_ctx)
{   
	int rc = 0;
	struct reg_2_set test;
	struct reg_2_set_int test2;
	
	memset(&test,0,sizeof(struct reg_2_set));
	memset(&test2,0,sizeof(struct reg_2_set_int));
	
	rc = registry2_getn(10,&test,sizeof(struct reg_2_set));
	
	printf("rc = %d\n",rc);
	printf("test.a = %s\n",test.a);
	
	rc = registry2_getn(11,&test2,sizeof(struct reg_2_set_int));

	printf("test.a = %d .b = %d .c =%d\n",test2.a,test2.b,test2.c);
	printf("rc = %d\n",rc);
    return 0;
}
