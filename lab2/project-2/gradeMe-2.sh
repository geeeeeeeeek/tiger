#!/bin/bash

BIN=parsetest
#PROJDIR=project-2
REFOUTDIR=../refs-2
TESTCASEDIR=../testcases
#DIFFOPTION="-w -B"

rm -f _tmp.txt .tmp.txt	
#echo $PROJDIR/$ite
#pwd
make clean >& /dev/null
make > /dev/null
#echo $?
if [[ $? != 0 ]]; then
	echo "[-_-]$ite: Compile Error"		
	exit 123
fi	

for tcase in `ls $TESTCASEDIR`
do		
	if [ ${tcase##*.} = "tig" ]; then
		tfileName=${tcase##*/}
		#echo $tfileName
		#echo "$PROJDIR/$ite/$BIN $TESTCASEDIR/$tfileName >& $REFOUTDIR/${tfileName%.*}.out"
		#read
		#$PROJDIR/$ite/$BIN $TESTCASEDIR/$tfileName >& $REFOUTDIR/${tfileName%.*}.out
		
		#pwd
		#echo "./$BIN $TESTCASEDIR/$tfileName >& _tmp.txt"
		#echo "diff $DIFFOPTION _tmp.txt $REFOUTDIR/${tfileName%.*}.out >& .tmp.txt"
		#read

		./$BIN $TESTCASEDIR/$tfileName >& _tmp.txt
		# fix bison version diff
		if [ $tfileName = "test49.tig" ]; then
			#echo $tfileName
			#cat _tmp.txt
			grep 'test49.tig:5.18: syntax error' _tmp.txt >& .tmp.txt
			if [ -s .tmp.txt ]; then
				grep 'test49.tig:5.18: parse error' _tmp.txt >& .tmp.txt
				if [ -s .tmp.txt ]; then
					cat _tmp.txt
					echo "[*_*]$ite: Output Mismatch [$tfileName]"				
					rm -f _tmp.txt .tmp.txt	
					exit 234
				fi
			fi
			continue
		fi

		# normal case
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

