#! /bin/bash

echo "/*"
echo " * @" $1
echo " * Automatically generated C config: don't edit"
echo " */"
while read line
do 
   ## skip comment 
   if [ `expr index "$line" \#` -gt 0 ]; then
	echo "/* $line */"	
	continue
   fi

   ## check config
   if [ `expr index "$line" =` -gt 0 ]; then
	var=${line%%=*}
    var=$(echo $var | tr '[:upper:]' '[:lower:]')
	val=${line#*=}

	if [[ "$val" =~ ^[yY]$ ]]; then
	    echo "const unsigned char $var=1;"
	elif [[ "$val" =~ ^[nN]$ ]]; then
	    echo "const unsigned char $var=0;"
    elif [[ "$val" =~ ^[[:space:]]*[[:digit:]]*[[:space:]]*$ ]]; then
        echo "const unsigned long $var=$val;"
    elif [[ "$val" =~ ^[[:space:]]*[[:xdigit:]]*[[:space:]]*$ ]]; then
        echo "const unsigned long $var=0x$val;"
	else
	    echo "const char $var[]=$val;"
	fi 
   fi 	
	
done

echo "/* end "$1 " */"
echo


