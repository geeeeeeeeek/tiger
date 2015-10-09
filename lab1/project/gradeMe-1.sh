#!/bin/bash

BIN=lextest
PROJDIR=project
REFOUTDIR=../refs-1
TESTCASEDIR=../testcases

rm -f _tmp.txt .tmp.txt	
make clean >& /dev/null
make > /dev/null
if [[ $? != 0 ]]; then
	echo "[-_-]$ite: Compile Error"		
	exit 123
fi	

for tcase in `ls $TESTCASEDIR`
do		
	if [ ${tcase##*.} = "tig" ]; then
		tfileName=${tcase##*/}

		./$BIN $TESTCASEDIR/$tfileName >& _tmp.txt
		diff $DIFFOPTION _tmp.txt $REFOUTDIR/${tfileName%.*}.out >& .tmp.txt
		if [ -s .tmp.txt ]; then
			cat .tmp.txt
			echo "[*_*]$ite: Output Mismatch [$tfileName]"				
			rm -f _tmp.txt .tmp.txt	
			exit 234
		fi
	fi	
done

rm -f _tmp.txt .tmp.txt	
echo "[^_^]$ite: Pass"

