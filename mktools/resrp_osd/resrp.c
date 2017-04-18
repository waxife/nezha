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
      printf("ERROR: OSD2 ONLY can't use IMG\n");
      break;
    case TYPE_SPRITE :
      update_address(item.spr.lut_addr,res_offset);
      update_address(item.spr.index_addr,res_offset);
      printf("ERROR: OSD2 ONLY can't use SPRITE\n");
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
      printf("ERROR: OSD2 ONLY can't use TWBC\n");
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
    change_item_offset(fp_rd, fp_wr, res_offset+itemHsize,        //update item address
                     fp_wr_offset, &resHeader.desc[0]);

    endian_inv(resHeader.desc[0].offset);

    fwrite(&resHeader, sizeof(struct res_t), 1, fp_wr); //write to file

/*//////////////////////////update all item header address////////////////////////////////////////*/
	for(i=1;i<n;i++){
		fread(&itemHeader, sizeof(struct res_item_t), 1, fp_rd); //read item header

		endian_inv(itemHeader.offset);                           //read &add offset with item header
		itemHeader.offset += resHsize;                           //add res header size!!!
		change_item_offset(fp_rd, fp_wr, res_offset+itemHsize,
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

    int binsize   = strtol(argv[1], NULL, 16);//offset
	char *res_in  = argv[2]; //spi osd res
	char *rp_out  = argv[3]; //.rp
    char *spi_out = argv[4]; //.spi
	
    f2 = fopen(res_in/*spi_out*/, "rb+"); // resource file with rc file remapped
    if(f2 == NULL){
        printf("3 open %s fail\n", spi_out);
		return -1;
	}

    f3 = fopen(rp_out, "wb+"); // new text bin file with rc info, lut, tile, oregs, twf parts (code, offset)
	if(f3 == NULL){
		printf("4 open %s fail\n", rp_out);
		return -1;
	}

    change_header_offset(f2, f3, binsize+FLASH_BASE_ADDR);

    fclose(f2);
    fclose(f3);

    //printf("Make tarzan_addrc.bin successfully.\n");
    return 0;
}
