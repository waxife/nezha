//#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include "rc.h"
//#include "../include/config.h"
//#define COMBINE_DBG
#define BIG_END
#define FLASH_BASE_ADDR 0x80000000

#ifdef COMBINE_DBG
    #define dbg_printf printf
#else
    #define dbg_printf(fmt, args...) ((void)0)
#endif


/********************************************************************
origin:  01 02 03 04
i=0:     04 02 03 01
i=1:     04 03 02 01
********************************************************************/
void endian_change(int size,char *data){
  int i, half_size;
  char tmp;

  half_size = size/2;
  size--;

  for(i=0;i<half_size;i++){
    tmp = data[i];
    data[i]=data[size-i];
    data[size-i]=tmp;
  }
}
#define endian_inv(data) endian_change(sizeof(data),(char*)&(data))

#define update_address(data,offset) do{                     \
                                      endian_inv(data);     \
                                      data += offset;       \
                                      endian_inv(data);     \
                                    }while(0)

/*******************************
 * update address function start
 *******************************/
void change_item_offset(FILE* frd, FILE* fwr, DWORD res_offset, DWORD res_staddr, struct res_item_t *itemHeader){
  long fp_offset;
  union item_u item;

  fp_offset = ftell(frd);

  fseek(frd,itemHeader->offset,SEEK_SET);//read item to union
  fread(&item, sizeof(item), 1, frd);

  fseek(frd,fp_offset,SEEK_SET); //reset file cursor

  switch(itemHeader->type){//update item address
    case TYPE_IMG:
      update_address(item.img.lut_addr,res_offset);
      update_address(item.img.index_addr,res_offset);
      break;
    case TYPE_SPRITE :
      update_address(item.spr.lut_addr,res_offset);
      update_address(item.spr.index_addr,res_offset);
      break;
    case TYPE_EMU:
      update_address(item.emu.base_addr,res_offset);
      break;
    case TYPE_MENU:
      update_address(item.menu.base_addr,res_offset);
     // update_address(item.menu.oram_addr,res_offset);
      break;
    case TYPE_TILE:
      update_address(item.tile.base_addr,res_offset);
     // update_address(item.tile.oram_addr,res_offset);
      break;
    case TYPE_LUT:
      update_address(item.lut.lut_addr,res_offset);
      break;
    case TYPE_OREG:
      update_address(item.oreg.base_addr,res_offset);
      break;
    case TYPE_ICON:
      update_address(item.icon.base_addr,res_offset);
      break;
    case TYPE_TWBC:
      update_address(item.twbc.color_addr,res_offset);
      update_address(item.twbc.index_addr,res_offset);
      break;
    case TYPE_TOFONT:
      item.tof.flash_addr = item.tof.base_addr;
      update_address(item.tof.base_addr,res_offset);
	  update_address(item.tof.flash_addr,res_offset);
	  break;
    default:
      printf("item read err!!!!\n");
      return;
  }

  fp_offset = ftell(fwr);

  fseek(fwr,itemHeader->offset+res_staddr,SEEK_SET);
  fwrite(&item, sizeof(item), 1, fwr);  //update item to file

  fseek(fwr,fp_offset,SEEK_SET);//reset file cursor
}

void change_header_offset(FILE *fp_rd, FILE *fp_wr, DWORD res_offset){
	struct res_t resHeader;
	struct res_item_t itemHeader;
	short n = 0;
	long i,j;
	long fp_wr_offset, rs_tbl_end;
	DWORD rs_addr, rs_data, rs_tbl_st=0;
	short chkTW;
	DWORD resHsize,itemHsize;

	unsigned char temp = 0;
	unsigned char emu_data[5];

/*///////////////////check resource update table position//////////////////////////*/
    fseek(fp_rd,-6,SEEK_END);     //seek resource end - 4
    rs_tbl_end = ftell(fp_rd);    //update table end = resource end - tbl start address

    fread(&rs_tbl_st, sizeof(rs_tbl_st) ,1, fp_rd);  //read update table start address
    endian_inv(rs_tbl_st);
//  rs_tbl_st-=2;                 //T.W.Addr -> addr-2 for cancel TW 10/24 fix

    fseek(fp_rd,0,SEEK_SET);      //reset file cursor
    fp_wr_offset = ftell(fp_wr);  //save write file resource start position(for update)

/*////write raw resource to combine////*/
    for(i=0; i<rs_tbl_st; i++){
        fread(&temp, 1, 1, fp_rd);
        fwrite(&temp, 1, 1, fp_wr);
    }

/*////////reset file cursor///////////*/
    fseek(fp_rd,0,SEEK_SET);
    fseek(fp_wr,fp_wr_offset,SEEK_SET);
/*//////////////read res header, cal itemsize////////////////////////////////*/
	fread(&resHeader, sizeof(struct res_t), 1, fp_rd);  //read res header

	n = resHeader.nelements;                            //read element number
	endian_change(sizeof(n),(char*)&n);

	resHsize = n*8 + 6 +4;
	itemHsize = n * 16 + resHsize;

/*///////////////////update resource data, like emu address/////////////////////////////*/
    fseek(fp_rd,rs_tbl_st,SEEK_SET);
    fread(&chkTW,sizeof(chkTW),1,fp_rd);

    if(chkTW != 0x5754){
        printf("rs_tbl_st %x (ERR)!!!\n",rs_tbl_st);
        system("pause");
        return;
    }

    for(i=rs_tbl_st+2;i<rs_tbl_end;i+=4){
        fseek(fp_rd,i,SEEK_SET);                        //read update table
        fread(&rs_addr, sizeof(rs_addr), 1, fp_rd);     //get update res addr
        endian_inv(rs_addr);

        fseek(fp_rd,rs_addr,SEEK_SET);                  //go to res data addr
        fread(&emu_data, 1, 5, fp_rd);

        rs_data = (emu_data[4]<<16) + (emu_data[2]<<8) + emu_data[0]; //combine emu address
        rs_data += res_offset;                          //add offsetitemHsize
        rs_data += itemHsize;                           //add itemHsize

        for(j=0;j<=2;j++){  //write back to array
            emu_data[j*2] = rs_data & 0xff;
            rs_data >>= 8;
        }

        fseek(fp_wr,rs_addr+fp_wr_offset,SEEK_SET);     //go to res data addr again
        fwrite(&emu_data, 1, 5, fp_wr);                 //update res data to write file
    }

/*////////reset file cursor///////////*/
    fseek(fp_rd,0,SEEK_SET);
    fseek(fp_wr,fp_wr_offset,SEEK_SET);

/*//////////////read res header, and update first item header address////////////////////////////////*/
	fread(&resHeader, sizeof(struct res_t), 1, fp_rd);  //read res header

	n = resHeader.nelements;                            //read element number
	endian_change(sizeof(n),(char*)&n);

	resHsize = n*8 + 6 + 4;
	itemHsize = n * 16 + resHsize;

    endian_inv(resHeader.desc[0].offset);               //read &add offset with first item header
    resHeader.desc[0].offset += resHsize;               //add res header size!!!
    change_item_offset(fp_rd, fp_wr, /*res_offset+*/itemHsize,        //update item address
                     fp_wr_offset, &resHeader.desc[0]);

    endian_inv(resHeader.desc[0].offset);

    fwrite(&resHeader, sizeof(struct res_t), 1, fp_wr); //write to file

/*//////////////////////////update all item header address////////////////////////////////////////*/
	for(i=1;i<n;i++){
		fread(&itemHeader, sizeof(struct res_item_t), 1, fp_rd); //read item header

		endian_inv(itemHeader.offset);                           //read &add offset with item header
		itemHeader.offset += resHsize;                           //add res header size!!!
		change_item_offset(fp_rd, fp_wr, /*res_offset+*/itemHsize,
                       fp_wr_offset, &itemHeader);           //update item address

		endian_inv(itemHeader.offset);

		fwrite(&itemHeader, sizeof(struct res_item_t), 1, fp_wr);//write to file
	}
//////reset file cursor///////
	fseek(fp_rd,0,SEEK_SET);
	fseek(fp_wr,0,SEEK_END);

	return;
}
/*******************************
 * update address function end
 *******************************/

int main(int argc, char* argv[])
{
	//FILE*	f1;
	FILE*	f2;
	FILE*	f3;
	int		i, j;
	//ressize;
	char	temp[16];
	char	element = 0;

	DWORD resHsize,itemHsize;
	struct res_t resHeader;

	short n = 0;
	unsigned long res_wr_addr = 0;
	unsigned long res_wr_size = 0;
	struct res_item_t item = {0};
	unsigned char rcbuf[16];
    struct res_tile_t* ptile;
    struct res_lut_t* plut;
    struct res_oreg_t* poreg;
    struct res_tof_t* ptof;

    unsigned short font_num = 0;
    unsigned long font_bytes = 0;

    int binsize   = strtol(argv[1], NULL, 16);//offset
	char *res_in  = argv[2]; //spi osd res
	char *rp_out  = argv[3]; //.rp
    char *spi_out = argv[4]; //.spi

    f2 = fopen(res_in, "rb+"); // resource file with rc file remapped
    if(f2 == NULL){
        printf("1 open %s fail\n", res_in);
		return -1;
	}

    f3 = fopen(spi_out, "wb+"); // new text bin file with rc info, lut, tile, oregs, twf parts (code, offset)
	if(f3 == NULL){
		printf("2 open %s fail\n", spi_out);
		return -1;
	}

    change_header_offset(f2, f3, 0);
    fclose(f2);
    fclose(f3);

    f2 = fopen(spi_out, "rb+"); // resource file with rc file remapped
    if(f2 == NULL){
        printf("3 open %s fail\n", spi_out);
		return -1;
	}

    f3 = fopen(rp_out, "wb+"); // new text bin file with rc info, lut, tile, oregs, twf parts (code, offset)
	if(f3 == NULL){
		printf("4 open %s fail\n", rp_out);
		return -1;
	}

	//3rd combin res file (rc info)
	fread(&resHeader, sizeof(struct res_t), 1, f2);  //read res header
	n = resHeader.nelements;                            //read element number
	endian_change(sizeof(n),(char*)&n);
	resHsize = n*8 + 6 +4;
	itemHsize = n * 16 + resHsize;


	fseek(f2, 0, SEEK_SET);

	res_wr_addr = binsize;//ftell(f3);
	res_wr_addr += itemHsize;
	res_wr_addr += FLASH_BASE_ADDR;
	res_wr_size = 0;
	//rc_item_addr = res_wr_addr - itemHsize + resHsize;
	//fseek(f3, rc_item_addr, SEEK_SET);

	for(i = 0; i < resHsize; i++)
	{
		fread(temp, 1, 1, f2);
		fwrite(temp, 1, 1, f3);
	}
	fseek(f2, 6, SEEK_SET);

    dbg_printf("Header Process\r\n");

	for(i = 6; i < resHsize-4; )
	{
		fread(&item, sizeof(item), 1, f2);
		endian_change(sizeof(item.offset), (char*)&(item.offset));
		fseek(f2, item.offset, SEEK_SET);
		if(item.type == TYPE_TILE){
			fread(rcbuf, 16, 1, f2);
			ptile = (struct res_tile_t*)rcbuf;
			endian_change(sizeof(ptile->base_addr), (char*)&(ptile->base_addr));
			endian_change(sizeof(ptile->length), (char*)&(ptile->length));

            dbg_printf("tile old addr = 0x%lx, len = 0x%x\r\n", ptile->base_addr, ptile->length);

			ptile->base_addr = res_wr_addr + res_wr_size;
			res_wr_size += (ptile->length* sizeof(short));

            dbg_printf("tile new addr = 0x%lx, len = 0x%x, end = 0x%lx\r\n", ptile->base_addr, ptile->length, ptile->base_addr + ptile->length);

			endian_change(sizeof(ptile->base_addr), (char*)&(ptile->base_addr));
			endian_change(sizeof(ptile->length), (char*)&(ptile->length));
			fwrite(rcbuf, 16, 1, f3);
		}else if(item.type == TYPE_LUT){
			fread(rcbuf, 16, 1, f2);
			plut = (struct res_lut_t*)rcbuf;
			endian_change(sizeof(plut->lut_addr), (char*)&(plut->lut_addr));
			endian_change(sizeof(plut->main_len), (char*)&(plut->main_len));

			dbg_printf("LUT old addr = 0x%lx, len = 0x%x\r\n", plut->lut_addr, plut->main_len);

            plut->lut_addr = res_wr_addr + res_wr_size;
			res_wr_size += plut->main_len*4 + plut->re_2bp_len + plut->re_bg_len + plut->sec_len;

			dbg_printf("LUT new addr = 0x%lx, len = 0x%x, end = 0x%lx\r\n", plut->lut_addr, plut->main_len, plut->lut_addr + plut->main_len);

            endian_change(sizeof(plut->lut_addr), (char*)&(plut->lut_addr));
			endian_change(sizeof(plut->main_len), (char*)&(plut->main_len));
			fwrite(rcbuf, 16, 1, f3);
		}else if(item.type == TYPE_OREG){
			fread(rcbuf, 16, 1, f2);
			poreg = (struct res_oreg_t*)rcbuf;
			endian_change(sizeof(poreg->base_addr), (char*)&(poreg->base_addr));
            endian_change(sizeof(poreg->length), (char*)&(poreg->length));

			dbg_printf("OREG old addr = 0x%lx, len = 0x%x\r\n", poreg->base_addr, poreg->length);

            poreg->base_addr = res_wr_addr + res_wr_size;
            res_wr_size += poreg->length;

			dbg_printf("OREG new addr = 0x%lx, len = 0x%x, end = 0x%lx\r\n", poreg->base_addr, poreg->length, poreg->base_addr + poreg->length);

            endian_change(sizeof(poreg->base_addr), (char*)&(poreg->base_addr));
            endian_change(sizeof(poreg->length), (char*)&(poreg->length));
			fwrite(rcbuf, 16, 1, f3);
		}else if(item.type == TYPE_TOFONT){
            fread(rcbuf, 16, 1, f2);
            ptof = (struct res_tof_t*)rcbuf;
            endian_change(sizeof(ptof->base_addr), (char*)&(ptof->base_addr));

            dbg_printf("TOF old addr = 0x%lx\r\n", ptof->base_addr);

            fseek(f2, ptof->base_addr + 0x58, SEEK_SET);
            ptof->base_addr = res_wr_addr + res_wr_size;
            fread(&font_num, sizeof(font_num), 1, f2);
            font_bytes = ((font_num * 6) + 93);
            res_wr_size += font_bytes;

            dbg_printf("TOF new addr = 0x%lx, len = 0x%lx, end = 0x%lx\r\n", ptof->base_addr, font_bytes, ptof->base_addr + font_bytes);

            endian_change(sizeof(ptof->base_addr), (char*)&(ptof->base_addr));
            fwrite(rcbuf, 16, 1, f3);
        }else{
            fread(rcbuf, 16, 1, f2);
            fwrite(rcbuf, 16, 1, f3);
        }
        i += sizeof(item);
        fseek(f2, i, SEEK_SET);
    }

    // copy osd2 lut, oreg, tile
    fseek(f2, 6, SEEK_SET);
    for(i = 6; i < resHsize-4;)
    {
        fread(&item, sizeof(item), 1, f2);
        endian_change(sizeof(item.offset), (char*)&(item.offset));
        fseek(f2, item.offset, SEEK_SET);
        if(item.type == TYPE_TILE)
        {
            fread(rcbuf, 16, 1, f2);
            ptile = (struct res_tile_t*)rcbuf;
            endian_change(sizeof(ptile->base_addr), (char*)&(ptile->base_addr));
            endian_change(sizeof(ptile->length), (char*)&(ptile->length));
            fseek(f2, ptile->base_addr, SEEK_SET);
            int tile_len = ptile->length * sizeof(short);

            dbg_printf("TYPE_TILE  tile_len = 0x%x\r\n", tile_len);

            for(j = 0; j < tile_len; j++)
            {
                fread(&element, 1, 1, f2);
                fwrite(&element, 1, 1, f3);
            }
        }
        else if(item.type == TYPE_LUT)
        {
            fread(rcbuf, 16, 1, f2);
            plut = (struct res_lut_t*)rcbuf;
            endian_change(sizeof(plut->lut_addr), (char*)&(plut->lut_addr));
            endian_change(sizeof(plut->main_len), (char*)&(plut->main_len));
            endian_change(sizeof(plut->sec_len), (char*)&(plut->sec_len));
            endian_change(sizeof(plut->re_2bp_len), (char*)&(plut->re_2bp_len));
            endian_change(sizeof(plut->re_bg_len), (char*)&(plut->re_bg_len));
            fseek(f2, plut->lut_addr, SEEK_SET);
            int lut_len = plut->main_len*4 + plut->sec_len + plut->re_2bp_len + plut->re_bg_len;

            dbg_printf("TYPE_LUT  lut_len = 0x%x\r\n", lut_len);

            for(j = 0; j < lut_len; j++)
            {
                fread(&element, 1, 1, f2);
                fwrite(&element, 1, 1, f3);
            }
        }
        else if(item.type == TYPE_OREG)
        {
            fread(rcbuf, 16, 1, f2);
            poreg = (struct res_oreg_t*)rcbuf;
            endian_change(sizeof(poreg->base_addr), (char*)&(poreg->base_addr));
            endian_change(sizeof(poreg->length), (char*)&(poreg->length));
            fseek(f2, poreg->base_addr, SEEK_SET);

            dbg_printf("TYPE_OREG  oreg->length = 0x%x\r\n", poreg->length);

            for(j = 0; j < poreg->length; j++)
            {
                fread(&element, 1, 1, f2);
                fwrite(&element, 1, 1, f3);
            }
        }else if(item.type == TYPE_TOFONT){
            fread(rcbuf, 16, 1, f2);
            ptof = (struct res_tof_t*)rcbuf;
            endian_change(sizeof(ptof->base_addr), (char*)&(ptof->base_addr));
            
            fseek(f2, ptof->base_addr + 0x58, SEEK_SET);
            fread(&font_num, sizeof(font_num), 1, f2);
            
            font_bytes = font_num * 6 + 93; /*font num --> font byte num*/
            fseek(f2, ptof->base_addr, SEEK_SET);

            dbg_printf(".....................................\n");
            dbg_printf("TYPE_TOFONT  font_bytes= 0x%lx\r\n", font_bytes);
            dbg_printf("TYPE_TOFONT FLASH_ADDR = %lx\n",ptof->flash_addr);

            for(j = 0; j < font_bytes; j++)
            {
                fread(&element, 1, 1, f2);
                fwrite(&element, 1, 1, f3);
            }
            //printf("file end = 0x%x\r\n", ftell(f2));

        }
        i += sizeof(item);
        fseek(f2, i, SEEK_SET);
    }

    fclose(f2);
    fclose(f3);

    //printf("Make tarzan_addrc.bin successfully.\n");
    return 0;
}
