/**
 *  @file   display.h
 *  @brief  Display setting header file
 *  $Id: display.h,v 1.9 2014/01/22 07:29:11 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/21  C.N.Yu  New file.
 *
 */
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define SCALEBITS   10
#define ONE_HALF    ((int) 1L << (SCALEBITS-1))
#define FIX(x)      ((int)((x)*(1L << SCALEBITS) + 0.5))

#define YR_C        FIX(0.257)
#define YG_C        FIX(0.504)
#define YB_C        FIX(0.098)

#define UR_C        (-FIX(0.148))
#define UG_C        (-FIX(0.291))
#define UB_C        FIX(0.439)

#define VR_C        FIX(0.439)
#define VG_C        (-FIX(0.368))
#define VB_C        (-FIX(0.071))
static inline int clip255(int x)
{
	return (x > 255) ? 255 : ((x < 0) ? 0 : x);
}
static inline unsigned long RGB_YUV(int R, int G, int B)
{
    int Y, U, V;
    unsigned long yuv;  
    Y = clip255(((YR_C*R + YG_C*G + YB_C*B)>>SCALEBITS)+16);
    U = clip255(((UR_C*R + UG_C*G + UB_C*B)>>SCALEBITS) + 128);
    V = clip255(((VR_C*R + VG_C*G + VB_C*B)>>SCALEBITS) + 128);
    yuv = (V  | (U << 16) | (Y << 24));
    return yuv;
}
enum CVBS_CHENNLE {
    CH0,
    CH1,
    CH2,
    CH3
};

/**
 * @func    display_init
 * @brief   initialize panel driver
 * @param   none
 * @return  0 success, or return negative error code.
 */
int display_init(void);

/**
 * @func    display_release
 * @brief   release panel driver
 * @param   none
 * @return  0 success, or return negative error code.
 */
int display_release(void);

/**
 * @func    display_bklight_on
 * @brief   set backlight of panel on
 * @param   none
 * @return  0 success, or return negative error code.
 */
int display_bklight_on(void);

/**
 * @func    display_bklight_off
 * @brief   set backlight of panel off
 * @param   none
 * @return  0 success, or return negative error code.
 */
int display_bklight_off(void);

#define VIDEO_STD_NTSC  1
#define VIDEO_STD_PAL   2
/**
 * @func    display_set_cvbs_lmod
 * @brief   set scaler parameter for liveview display from cvbs input
 * @param   standard         input signal standard(NTSC, PAL)
 * @return  0 success, or return negative error code.
 */
int display_set_cvbs_lmod(int standard, int channel);

/**
 * @func    display_set_cvbs_full
 * @brief   set scaler parameter for liveview display little mode from cvbs input
 * @param   standard         input signal standard(NTSC, PAL)
 * @return  0 success, or return negative error code.
 */

int display_set_cvbs_full(int standard, int channel);

/**
 * @func    display_set_play_lmod
 * @brief   set scaler parameter for decoding a recorded file to display
 * @param   standard         input signal standard(NTSC, PAL)
 * @return  0 success, or return negative error code.
 */
int display_set_play_lmod(int standard);
/**
 * @func    display_set_play_full
 * @brief   set scaler parameter for decoding a recorded file to display
 * @param   standard         input signal standard(NTSC, PAL)
 * @return  0 success, or return negative error code.
 */
int display_set_play_full(int standard);

/**
 * @func    display_set_blackscreen
 * @brief   set single color output
 * @param   color       yuv color
 * @return  0 sucess, or return negative error code 
 */
int display_set_blackscreen(unsigned long color);
int display_set_liveview_screen(void);
int display_set_blackscreen_effect(void);


#define BLACK_YUV 0x008080
#define BLUE_YUV  0x00FF80



#endif /* _DISPLAY_H_ */
