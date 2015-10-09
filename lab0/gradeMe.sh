#!/bin/bash

REFOUTPUT=./ref.txt

function doReap(){
	rm -f _tmp.txt .tmp.txt
}

doReap
make clean >& /dev/null
make > /dev/null
#echo $?
if [[ $? != 0 ]]; then
	echo "[-_-]: Compile Error"
	popd >& /dev/null
	exit 123
fi

./a.out >& _tmp.txt
diff -b _tmp.txt $REFOUTPUT >& .tmp.txt
if [ -s .tmp.txt ]; then
	echo "[*_*]: Output Mismatch"
	cat .tmp.txt
	doReap
	popd >& /dev/null
	exit 234
fi

doReap
echo "[^_^]: Pass"
