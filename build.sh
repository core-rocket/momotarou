#!/bin/bash
echo -ne "$1\t"
if [ ${#1} -lt 8 ]; then
	echo -ne "\t"
fi

cd $1
mbed deploy &> d
mbed compile&> compile.log
code=$?
if [ $code = 0 ]; then
	echo -e "[OK]"
else
	echo -e "[faild]"
	cat compile.log
fi
rm -f d
exit $code
