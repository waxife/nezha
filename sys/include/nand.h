/**
 *  @file   nand.h
 *  @brief  nand flash id table and nand flash command
 *  $Id: nand.h,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/03/15  New file.
 *          2009/12/07  Hugo    modification for JFTL 2.0
 *
 */

#ifndef __NAND_H
#define __NAND_H

#define NAND_MAGIC	0x646e616e
#define SECTORSIZE	512
#define OOBSIZE		16

struct nand_cfg {
	unsigned int magic;
	unsigned int csize;	/* chip size */
	unsigned int bsize;	/* block size */
	unsigned short psize;	/* page size */
	unsigned char osize;	/* oob size */
	unsigned char erate;	/* error rate */
};

struct nandid {
	int did;		/* device id */
	int bsize;		/* block size */
	int psize;		/* page size */
	int osize;		/* oob size */
	int nblock;		/* number of blocks */
	char *name;		/* device name */
	char *manf;		/* manufactor */
	unsigned char nid[8];	/* nand flash id */
};

/* NAND OOB exported to upper layer */
struct nand_oob {
	unsigned short bad;	/* bad block marker */
	unsigned char data;	/* OOB data status */
	unsigned char jver;	/* journal version */
	unsigned int lba;	/* logical block addressing */
	unsigned char ecc[8];
} __attribute__ ((packed));

/* SGData - internal use struct between jftl and nand */
struct scatterlist;
struct sgdata {
	struct scatterlist *sg;
	unsigned int sgoff;
	int sgnent;
};

#ifdef CONFIG_PROC_FS
struct nand_info {
	u32 wpage_count;
	u32 npage_w;
	u32 page_oob_w;

	u32 rpage_count;
	u32 npage_r;
	u32 page_oob_r;

	u32 eblk;
	u32 page_cpbk;

};
#endif

#define ENAND			(2000)
#define ENAND_PROGRAM   (ENAND+0)
#define ENAND_ERASE     (ENAND+1)
#define ENAND_ECC       (ENAND+2)
#define ENAND_BCH		(ENAND+3)
#define ENAND_FLASH_NG	(ENAND+4)

/* NAND general operations */
#ifndef __KERNEL__
void nand_load_img (char *img_file);
#endif

void nand_alloc (void **ctx, int type);
int nand_init (void *ctx, struct nandid *nid);
int xd_init(void *ctx, struct nandid *nid);
int nand_release (void *ctx);
int nand_erase_block (void *ctx, int pu);
int is_free_sector(char *oob);

int nand_get_oob (void *ctx, const void *oob, struct nand_oob *nand_oob);
int nand_set_oob (void *ctx, void *oob, const struct nand_oob *nand_oob);

int nand_info (void *ctx);
int nand_probe (void *ctx, struct nandid *nid);
int nand_remove (void *ctx);

int nand_get_block_size (void *nctx);
int nand_get_page_size (void *nctx);
int nand_get_oob_size (void *nctx);

void disable_bch(void *ctx);
void enable_bch(void *ctx);

/* NAND page operations - backward compatible */
int nand_read_page_oob (void *ctx, int pu, int page, char *oob);
int nand_write_page_oob (void *ctx, int pu, int page, const char *oob);
int nand_read_pages (void *ctx, int pu, int page, int num, struct sgdata *sgd, void *oob);
int nand_write_pages (void *ctx, int pu, int page, int num, struct sgdata *sgd, void *oob);
int nand_copyback_page (void *ctx, int spu, int spage, int tpu, int tpage, int n, const char *oob);

int nand_read_page (void *ctx, int pu, int page, char *data, char *oob);
int nand_write_page (void *ctx, int pu, int page, const char *data, const char *oob);

/* NAND sector operations */
int nand_read_sector_oob (void *ctx, int pu, int sector, void *oob);
int nand_write_sector_oob (void *ctx, int pu, int sector, void *oob);

#define nand_read_sector(ctx, pu, sec, num, sgd, oob) __nand_read_sector(ctx, pu, sec, num, sgd, oob, __FILE__, __LINE__)
int __nand_read_sector (void *ctx, int pu, int sector, int num, struct sgdata *sgd, void *oob, const char* fname, int lineno);

#define nand_write_sector(ctx, pu, sec, num, sgd, oob) __nand_write_sector(ctx, pu, sec, num, sgd, oob, __FILE__, __LINE__)
int __nand_write_sector (void *ctx, int pu, int sector, int num, struct sgdata *sgd, void *oob, const char* fname, int lineno);
//int nand_write_sector (void *ctx, int pu, int sector, int num, struct sgdata *sgd, void *oob);

#define nand_readv_sector(ctx, pu, sec, data, oob, nsec) __nand_readv_sector(ctx, pu, sec, data, oob, nsec, __FILE__, __LINE__)
int __nand_readv_sector(void *ctx, int pu, int sec, char *data, char *oob, int nsec, const char* fname, int lineno);
//int nand_readv_sector(void *ctx, int pu, int sec, char *data, char *oob, int nsec);
int nand_writev_sector(void *ctx, int pu, int sec, const char *data, const char *oob, int nsec);
int nand_copyback_sector (void *ctx, int spu, int ssector, int dpu, int dsector, int num);
int nand_is_empty_sector(void *ctx, int pu, int sec);

/* Scatter-gather */
#ifndef __KERNEL__
#include <alloca.h>

typedef unsigned long dma_addr_t;
struct page {
	void *virtual;		/* virtual address */
};

#define page_address(page) ((page)->virtual)
#define set_page_address(page, address) do { (page)->virtual = (address); } while(0)

//#define virt_to_bus(addr) 0x00000000 /* faked dma address */
#define virt_to_page(addr) ({ struct page *page = alloca (sizeof (struct page)); page->virtual = (void *)addr; page; })

struct scatterlist {
	struct page *	page;
	unsigned int	offset;
	dma_addr_t	dma_address;
	unsigned int	length;
};

#define sg_dma_address(sg)	((sg)->dma_address)
#define sg_dma_len(sg)		((sg)->length)
#endif /* !__KERNEL__ */

#endif /* __NAND_H */
