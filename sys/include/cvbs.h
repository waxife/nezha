/**
 *  @file   cvbs.h
 *  @brief  Cheetah CVBS header
 *  $Id: cvbs.h,v 1.7 2014/03/31 12:53:25 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/07  New file.
 *
 */
 
#ifndef _CVBS_H_
#define _CVBS_H_


/*
 * CVBS signal: avin signal type
 */
enum AVIN_CVBS_TYPE{
    AVIN_CVBS_CLOSE,
    AVIN_CVBS_NTSC,
    AVIN_CVBS_PAL,
    AVIN_CVBS_NTSC443,
    AVIN_CVBS_PALCN,
    AVIN_CVBS_UNKNOW,
    AVIN_END
};	

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */
/*
 * CVBS information data structure
 */
struct cvbs_state {
	unsigned char std_mode;                 /*avin type*/   
	unsigned char auto_freerun_mode;        /*auto free run mode*/
	unsigned char chromapll_lockcb;         /*chromapall_lock P2_5A[3]*/
	unsigned char chromapll_lockcb_times;   /*record ChromaPLL lock times*/
	unsigned char one_line_loss_times;      /*record one line loss times*/
	unsigned char cvd_h_loss_times;         /*record CVD h line loss times.*/
	unsigned char cvd_v_loss_times;         /*record CVD v line loss times.*/
	unsigned char reset_times;              /*record CVD reset times.*/
	unsigned char en_auto_detect;           /*Enable process detect cvbs.*/
} __attribute__((packed));

#pragma pack(pop)

/**
 * @func      cvbs_detect_init
 * @brief      configure cvbs initial state
 * @param   cvbs_st        cvbs state data structure         
 * @return  0 success, or return negative error code.
 */
int cvbs_detect_init(struct cvbs_state *cvbs_st);
int cvbs2_detect_init(struct cvbs_state *cvbs_st);

/**
 * @func      cvbs_detect_close
 * @brief      close cvbs detect 
 * @param   cvbs_st        cvbs state data structure
 * @return  0 success, or return negative error code.
 */
int cvbs_detect_close(struct cvbs_state *cvbs_st);
int cvbs2_detect_close(struct cvbs_state *cvbs_st);

/**
 * @func     cvbs_detect_reset
 * @brief     software reset 
 * @param           
 * @return  0 success, or return negative error code.
 */
int cvbs_detect_reset(struct cvbs_state *cvbs_st);
int cvbs2_detect_reset(void);

/**
 * @func      cvbs_detect
 * @brief     detect cvbs signal type
 * @param   cvbs_st        cvbs state data structure       
 * @return  0: no signal, 1: NTSC, 2: PAL
 */
int cvbs_detect(struct cvbs_state *cvbs_st);
int cvbs2_detect(struct cvbs_state *cvbs_st);
int cvbs2_detect_wait(struct cvbs_state *cvbs_st);
/**
 * @func     cvbs_detect_without_init
 * @brief     detect cvbs signal type without cvbs detect init
 * @param  cvbs_st        
 * @return  0: no signal, 1: NTSC, 2: PAL 
 */
int cvbs_detect_without_init(struct cvbs_state *cvbs_st);

/**
 * @func     cvbs_detect_freerun_open
 * @brief     open free run  
 * @param   cvbs_st        cvbs state data structure       
 * @return  0 success, or return negative error code.
 */
int cvbs_detect_freerun_open(struct cvbs_state *cvbs_st);
void cvbs2_detect_freerun_open(struct cvbs_state *cvbs_st);

/**
 * @func       cvbs_detect_freerun_close
 * @brief       close free run
 * @param    cvbs_st        cvbs state data structure     
 * @return  0 success, or return negative error code.
 */
int cvbs_detect_freerun_close(struct cvbs_state *cvbs_st);
int cvbs2_detect_freerun_close(struct cvbs_state *cvbs_st);

/**
 * @func       cvbs_detect_return_chromalock_times
 * @brief      return chromapll lock times
 * @param   cvbs_st        cvbs state data structure        
 * @return   cvbs_st->chromapll_lockcb_times
 */
int cvbs_detect_return_chromalock_times(struct cvbs_state *cvbs_st);
int cvbs2_detect_return_chromalock_times(struct cvbs_state *cvbs_st);

/**
 * @func       cvbs_bluescreen_on
 * @brief      blue screen on/off
 * @param      0: off blue screen. 1:on blue screen
 * @return   
 */
void cvbs_bluescreen_on(unsigned char on);
void cvbs2_bluescreen_on(unsigned char on, unsigned char y, unsigned char u, unsigned char v);

void cvbs2_resync_fpll(void);
/**
 * @func       cvbs2_fpll_init
 * @brief      init fpll
 * @param     
 * @return   
 */
void cvbs2_fpll_init(void);

void cvbs2_fine_tune_dto(void);

int cvbs2_detect656(struct cvbs_state *cvbs_st);
int cvbs2_detect656_wait(struct cvbs_state *cvbs_st);
#endif /*_CVBS_H_*/ 
