#! /bin/bash

input_file=$1
ring_size=$2

__RING_MAGIC=0x21872187
__RING_TYPE=0x00000004

mk4bytes() {
    printf "0: %.8x" $1 | sed -e 's/0\: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g4 >> $input_file.hh
}


mkoldring() {
    rm -f $input_file.hh
    mk4bytes $__RING_MAGIC
    size=`stat $input_file -c "%s"`
    mk4bytes $size
    mk4bytes $__RING_TYPE; rtype=$retval
    mk4bytes 0
    cat $input_file >> $input_file.hh
    size=`stat $input_file.hh -c "%s"`
    if [ $size -gt $ring_size ]; then
        echo -e "$input_file with header size=$size is > $ring_size"
        exit -1
    fi
    let size=$ring_size-$size;
    dd if=/dev/zero of=$input_file.pad count=$size bs=1 > /dev/null 2>&1
    cat $input_file.hh $input_file.pad > $input_file.64k
}

mkheadring() {
    size=`stat $input_file -c "%s"`
    if [ $size -gt $ring_size ]; then
        echo -e "$input_file size=$size is > $ring_size"
        exit -1
    fi
    let size=$ring_size-$size;
    echo size=$size   $ring_size
    dd if=/dev/zero of=$input_file.pad count=$size bs=1 > /dev/null 2>&1
    cat $input_file $input_file.pad > $input_file.64k
}

print_usage() {
    echo "./mkringtong.sh <input_file> <expected ring size>"
    exit -1
}

if [ $# -ne 2 ]; then
    print_usage
fi 

tag=0x`od -N4 -A n -t x4 $input_file | tr -d ' '`

if [ "$tag" == "$__RING_MAGIC" ]; then
    echo "make ring: input file '$input_file' with header"
    mkheadring
else
    echo "make ring: input file '$input_file' without header"
    mkoldring
fi

rm -f $input_file.hh $input_file.pad



