/**
 *  @file   ili9327_init.c
 *  @brief	ili9327 init setting
 *  $Id: ili9163_init.c,v 1.1 2016/07/22 10:17:30 joec Exp $
 *  $Author: joec $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/03/13  Kevin 	New file.
 *
 */

#include <stdio.h>
#include <display.h>
#include <unistd.h>
#include <sys.h>
#include <io.h>
#include "./drivers/i80/i80.h"


int ili9163_init(void)
{
   
    printf ("init ILI9163\n");
#if 0
    /* exit sleep */
    unsigned char data_11[] = {0x00,0x00};
 
    i80_write_regv (0x0011, data_11, 0);

    usleep (100);

    /* init ili9163 */
    unsigned char data_B1[] = {0x05, 0x3A, 0x3A};
    
    i80_write_regv (0x00B1, data_B1, 3);

    unsigned char data_B2[] = {0x05, 0x3A, 0x3A};
    i80_write_regv (0x00B2, data_B2, 3);
    
    unsigned char data_B3[] = {0x05, 0x3A, 0x3A, 0x05, 0x3A, 0x3A};
    i80_write_regv (0x00B3, data_B3, 6);
    
    unsigned char data_B4[] = {0x03};
    i80_write_regv (0x00B4, data_B4, 1);

    unsigned char data_c0[] = {0x62, 0x02, 0x04};
    i80_write_regv (0x00c0, data_c0, 3);

    unsigned char data_c1[] = {0xC0};
    i80_write_regv (0x00c1, data_c1, 1);

    unsigned char data_c2[] = {0x0D, 0x00};
    i80_write_regv (0x00c2, data_c2, 2);

    unsigned char data_c3[] = {0x8D, 0xEA};
    i80_write_regv (0x00c3, data_c3, 2);
    
    unsigned char data_c4[] = {0x8D, 0xEE};
    i80_write_regv (0x00c4, data_c4, 2);
    
    unsigned char data_c5[] = {0x0D};
    i80_write_regv (0x00c5, data_c5, 1);
    
    unsigned char data_36[] = {0x88};
    i80_write_regv (0x0036, data_36, 1);
    
    unsigned char data_3A[] = {0x06};
    i80_write_regv (0x003A, data_3A, 1);
    
    unsigned char data_30[] = {0x00, 0x00, 0x00, 0x7F};
    i80_write_regv (0x0030, data_30, 4);
#else
/* exit sleep */

		/* ST7789S Frame rate setting */
		unsigned char data_B2[] = {0x0C, 0x0C, 0x00 ,0x33,0x33};
    	i80_write_regv (0x00B2, data_B2, 5);
    	
   	unsigned char data_B7[] = {0x35};
    	i80_write_regv (0x00B7, data_B7, 1);
    	
    unsigned char data_36[] = {0xa0};	 // bit7 上下 bit6 左右
    	i80_write_regv (0x0036, data_36, 1); 
    	
   	/* ST7789S Power setting */
   	unsigned char data_BB[] = {0x30};	//vcom
    	i80_write_regv (0x00BB, data_BB, 1);
    	
   	unsigned char data_C3[] = {0x1C};
    	i80_write_regv (0x00C3, data_C3, 1);
    	
    unsigned char data_C6[] = {0x0F};	 
    	i80_write_regv (0x00C6, data_C6, 1);
    	 
    unsigned char data_D0[] = {0xA4,0xA2};	 
    	i80_write_regv (0x00D0, data_D0, 2);  
    		
		/* ST7789S gamma setting */
		unsigned char data_E0[] = {0xF0,0x00,0x0A,0x10,0x12,0x1B,0x39,0x44,0x47,0x28,0x12,0x10,0x16,0x1B};	 
    	i80_write_regv (0x00E0, data_E0, 14);  
    	
		unsigned char data_E1[] = {0xF0,0x00,0x0A,0x10,0x11,0x1A,0x3B,0x34,0x4E,0x3A,0x17,0x16,0x21,0x22};	 
    	i80_write_regv (0x00E1, data_E1, 14); 
    	
    unsigned char data_3A[] = {0x55};	 //18 RGB ,55-16BIT RGB
    	i80_write_regv (0x003A, data_3A, 1);	 
    	
		/* ST7789S  setting */
		unsigned char data_2A[] = {0x00,0x00,0x00,0xEF};	//Frame rate control 
    	i80_write_regv (0x002A, data_2A, 4); 
   
   	unsigned char data_2B[] = {0x00,0x00,0x01,0x3F};	//Display function control
    	i80_write_regv (0x002B, data_2B, 4); 
    
    unsigned char data_11[] = {};	
    	i80_write_regv (0x0011, data_11, 0);  		 			
		
		usleep (120);
		
		unsigned char data_29[] = {};		//display on
    i80_write_regv (0x0029, data_29, 0);
    
    unsigned char data_2c[] = {};
    i80_write_regv (0x002c, data_2c, 0);   
/*
    unsigned char data_36[] = {0xa0};	 // bit7 上下 bit6 左右
    i80_write_regv (0x0036, data_36, 1);

    unsigned char data_3a[] = {0x05};
    i80_write_regv (0x003a, data_3a, 1);
    
    unsigned char data_c5[] = {0x0D};
    i80_write_regv (0x00c5, data_c5, 1);
    unsigned char data_30[] = {0x00, 0x00, 0x00, 0x7F};
    i80_write_regv (0x0030, data_30, 4);

    unsigned char data_E0[] = {0x0A, 0x1F, 0x0E, 0x17, 0x37, 0x31, 0x2B, 0x2E, 0x2C, 0x29, 0x31, 0x3C, 0x00, 0x05, 0x03, 0x0D};
    i80_write_regv (0x00E0, data_E0, 16);
    
    unsigned char data_E1[] = {0x0B, 0x1F, 0x0E, 0x12, 0x28, 0x24, 0x1F, 0x25, 0x25, 0x26, 0x30, 0x3C, 0x00, 0x05, 0x03, 0x0D};
    i80_write_regv (0x00E1, data_E1, 16);
    
    unsigned char data_29[] = {};
    i80_write_regv (0x0029, data_29, 0);
    
  
    unsigned char data_2c[] = {0x00, 0x00};
    i80_write_regv (0x002c, data_2c, 2);   

*/    
#endif
	return 0;
}




