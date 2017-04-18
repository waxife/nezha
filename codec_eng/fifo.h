/**
 *  @file   fifo.h
 *  @brief  fifo
 *  $Id: fifo.h,v 1.1 2014/07/15 09:54:59 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/07/15  hugo    new file
 *
 */
#ifndef __FIFO_H
#define __FIFO_H

/* FIFO Operations
 *
 *  +--+        +------------+        +--+
 *  |  |  <---  |    FIFO    |  <---  |  |
 *  +--+   get  +------------+   put  +--+
 */
#define list_append(HEAD, TAIL, CNT, ENT) do { if (TAIL) { TAIL->next = ENT; } TAIL = ENT; if (!HEAD) { HEAD = ENT; } CNT++; } while (0)
#define list_remove(HEAD, TAIL, CNT, ENT) do { ENT = HEAD; if (ENT) { HEAD = (ENT)->next; (ENT)->next = NULL; CNT--; } if (!HEAD) {TAIL = NULL; } } while (0)

#define ATOM_PUT_FREE(DEC, ATOM) do { list_append((DEC)->atom_free_head, (DEC)->atom_free_tail, (DEC)->atom_free_cnt, (ATOM)); } while (0)
#define ATOM_GET_FREE(DEC, ATOM) do { list_remove((DEC)->atom_free_head, (DEC)->atom_free_tail, (DEC)->atom_free_cnt, (ATOM)); } while (0)
#define ATOM_PEEK_FREE(DEC)      ((DEC)->atom_free_head)

#define ATOM_PUT_DMA(DEC, ATOM)  do { list_append((DEC)->atom_dma_head, (DEC)->atom_dma_tail, (DEC)->atom_dma_cnt, (ATOM)); } while (0)
#define ATOM_GET_DMA(DEC, ATOM)  do { list_remove((DEC)->atom_dma_head, (DEC)->atom_dma_tail, (DEC)->atom_dma_cnt, (ATOM)); } while (0)
#define ATOM_PEEK_DMA(DEC)       ((DEC)->atom_dma_head)

#define ATOM_PUT_FRM(DEC, ATOM)  do { list_append((DEC)->atom_frm_head, (DEC)->atom_frm_tail, (DEC)->atom_frm_cnt, (ATOM)); } while (0)
#define ATOM_GET_FRM(DEC, ATOM)  do { list_remove((DEC)->atom_frm_head, (DEC)->atom_frm_tail, (DEC)->atom_frm_cnt, (ATOM)); } while (0)
#define ATOM_PEEK_FRM(DEC)       ((DEC)->atom_frm_head)

#define PART_PUT_FREE(DEC, PART) do { list_append((DEC)->part_free_head, (DEC)->part_free_tail, (DEC)->part_free_cnt, (PART)); } while (0)
#define PART_GET_FREE(DEC, PART) do { list_remove((DEC)->part_free_head, (DEC)->part_free_tail, (DEC)->part_free_cnt, (PART)); } while (0)
#define PART_PEEK_FREE(DEC)      ((DEC)->part_free_head)

#define PART_PUT_VDEC(DEC, PART) do { list_append((DEC)->part_vdec_head, (DEC)->part_vdec_tail, (DEC)->part_vdec_cnt, (PART)); } while (0)
#define PART_GET_VDEC(DEC, PART) do { list_remove((DEC)->part_vdec_head, (DEC)->part_vdec_tail, (DEC)->part_vdec_cnt, (PART)); } while (0)
#define PART_PEEK_VDEC(DEC)      ((DEC)->part_vdec_head)

#define PART_PUT_ADEC(DEC, PART) do { list_append((DEC)->part_adec_head, (DEC)->part_adec_tail, (DEC)->part_adec_cnt, (PART)); } while (0)
#define PART_GET_ADEC(DEC, PART) do { list_remove((DEC)->part_adec_head, (DEC)->part_adec_tail, (DEC)->part_adec_cnt, (PART)); } while (0)
#define PART_PEEK_ADEC(DEC)      ((DEC)->part_adec_head)

#define PART_PUT_VREP(DEC, PART) do { list_append((DEC)->part_vrep_head, (DEC)->part_vrep_tail, (DEC)->part_vrep_cnt, (PART)); } while (0)
#define PART_GET_VREP(DEC, PART) do { list_remove((DEC)->part_vrep_head, (DEC)->part_vrep_tail, (DEC)->part_vrep_cnt, (PART)); } while (0)
#define PART_PEEK_VREP(DEC)      ((DEC)->part_vrep_head)

#define VIDEO_PUT_DEC(DEC, PART)  do {} while (0)
#define VIDEO_GET_DEC(DEC, PART)  do {} while (0)
#define AUDIO_PUT_DEC(DEC, PART)  do {} while (0)
#define AUDIO_GET_DEC(DEC, PART)  do {} while (0)

#endif /* __FIFO_H */
