#! /bin/bash


input_file=$1

get_sym() {
    retval="0x`grep -w $1 $input_file | cut -d' ' -f1`"
}

get_sym _bootsector_start;  bootsector_start=$retval
get_sym _bootsector_size;   bootsector_size=$retval
get_sym _ftboot;            ftboot=$retval

get_sym _code_start;        code_start=$retval
get_sym _code_size;         code_size=$retval
get_sym _fcode;             fcode=$retval

get_sym _data_start;        data_start=$retval
get_sym _data_size;         data_size=$retval
get_sym _fdata;             fdata=$retval
get_sym _upgrade_start;     upgrade_start=$retval
get_sym _upgrade_size;      upgrade_size=$retval
get_sym _fupg;              fupg=$retval

echo $fupgrade
get_sym _dfuins_start;      dfuins_start=$retval
get_sym _dfuins_size;       dfuins_size=$retval
get_sym _fdfui;             fdfui=$retval
get_sym _res_start;         res_start=$retval
get_sym _res_size;          ret_size=$retval
get_sym _fresource;         fresource=$retval
get_sym _res_rp_start;      res_rp_start=$retval
get_sym _res_rp_size;       res_rp_size=$retval
get_sym _fres_rp;           fres_rp=$retval
get_sym _febin;             febin=$retval
get_sym _ebin_start;        ebin_start=$retval
get_sym _ebin_size;         ebin_size=$retval
get_sym _info_start;        info_start=$retval
get_sym _info_size;         info_size=$retval
get_sym _finfo;             finfo=$retval
get_sym _ringtong_start;    ringtong_start=$retval
get_sym _ringtong_size;     ringtong_size=$retval
get_sym _fringtong;         fringtong=$retval
get_sym _registry_start;    registry_start=$retval
get_sym _registry_size;     registry_size=$retval
get_sym _fregistry;         fregistry=$retval
get_sym _medium_start;      medium_start=$retval
get_sym _fmedium;           fmedium=$retval
get_sym _ringtong_num;      ringtong_num=$retval
get_sym _ringtong_unitsize; ringtong_unitsize=$retval


echo -e "**  NOR (ROM) Layout Summary *************************"
echo -e "    PA        LA          Size         Segment"
echo -e "---------- ----------   ---------- -------------------"
printf "0x%08x [%08x] + %'10d bootsector\n" $bootsector_start $ftboot $bootsector_size
printf "0x%08x [%08x] + %'10d code\n" $code_start $fcode $code_size
printf "0x%08x [%08x] + %'10d data (shadow)\n" $data_start $fdata $data_size

printf "0x%08x [%08x] + %'10d upgrade (shadow)\n" $upgrade_start $fupg $upgrade_size
printf "0x%08x [%08x] + %'10d dfuins (shadow)\n" $dfuins_start $fdfui $dfuins_size
printf "0x%08x [%08x] + %'10d resource\n" $res_start $fresource $res_size
printf "0x%08x [%08x] + %'10d res header\n" $res_rp_start $fres_rp $res_rp_size
printf "0x%08x [%08x] + %'10d ebin\n" $ebin_start $febin $ebin_size
printf "0x%08x [%08x] + %'10d info\n" $info_start $finfo $info_size
printf "0x%08x [%08x] + %'10d ringtong (%d x %'d)\n" $ringtong_start $fringtong $ringtong_size $ringtong_num $ringtong_unitsize
printf "0x%08x [%08x] + %'10d registry\n" $registry_start $fregistry $registry_size
printf "0x%08x [%08x] +      > end medium\n" $medium_start $fmedium 
echo -e "------------------------------------------------------\n"


get_sym _fdata;         fdata=$retval
get_sym _data_size;     data_size=$retval
get_sym _fbss;          fbss=$retval
get_sym _bss_size;      bss_size=$retval
get_sym _fstack_barrier; fstack_barrier=$retval
get_sym _fstack;        fstack=$retval
get_sym _stack_size;    stack_size=$retval
get_sym _fheap;         fheap=$retval
get_sym _heap_size;     heap_size=$retval

echo -e "**  RAM Layout Summary *******************"
echo -e "   LA           Size       Segment"
echo -e "----------   ---------- ------------------"
printf "[%08x] + %'10d data\n" $fdata $data_size
printf "[%08x] + %'10d bss\n" $fbss $bss_size
printf "[%08x]              stack_barrier\n" $fstack_barrier 
printf "[%08x] - %'10d stack\n" $fstack $stack_size
printf "[%08x] + %'10d heap\n" $fheap $heap_size
echo -e "------------------------------------------"
