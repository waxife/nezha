/**
 *  @file   cache.h
 *  @brief  export cache functions
 *  $Id $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/10  New file.
 *
 */
#ifndef __CACHE_H
#define __CACHE_H


extern void flush_cache_all(void);
extern void flush_icache_all(void);
extern void flush_dcache_all(void);

extern void flush_icache_range(unsigned long start, unsigned long end);
extern void cache_init(void);

/* should move to io.h */
extern void dma_cache_wback_inv(unsigned long start, int size);
extern void dma_cache_wback(unsigned long start, int size);
extern void dma_cache_inv(unsigned long start, int size);

#endif /* __CACHE_H */
