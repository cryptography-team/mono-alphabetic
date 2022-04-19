#!/bin/sh
C=0; D=0; R=0; T=0; A=0
if [ "${1:0:1}" == "-" ]; then
	i=1
	while [ "${1:${i}:1}" ]; do
		CHAR=${1:${i}:1}
		if [ "$CHAR" == "c" ]; then C=1
		elif [ "$CHAR" == "d" ]; then D=1
		elif [ "$CHAR" == "r" ]; then R=1
		elif [ "$CHAR" == "t" ]; then T=1
		elif [ "$CHAR" == "a" ]; then A=1
		else
			printf "This program compiles c/cpp files using make and runs the\
 executable file while calculating the elapsed time\n
Note: In the case of using more than one parameter, concatenate them (i.e.\
 ./compile_and_run.sh -cra arg1 arg2)\n
./compile_and_run.sh [-[c|r|d|t]] [-a [arg1 arg2 ...]]\n
  -c\tcleans (deletes) already built files
  -r\tbuilds using release configurations
  -d\tbuilds using debug configurations, and runs the debugger
  -t\tbuilds using threads configurations
  -a\tsends the following parameters as arguments to main function
  -h\tshows this help message"
			exit 0
		fi
		i=$(($i + 1))
	done
	shift
fi
if [ $C -eq 1 ]; then
	make clean
fi
if [ $D -eq 1 ]; then
	make DEBUG=1 all
	make debug
	exit 0
fi
if [ $R -eq 1 ]; then
	make RELEASE=1 all
elif [ $T -eq 1 ]; then
	make THREADS=1 all
else
	make all
fi
if [ $A -eq 0 ]; then
	shift $#
fi
echo -------------------------------
START_TIME=$(date +%s.%6N)
./*.exe $@
TMP=$?
END_TIME=$(date +%s.%6N)
echo
echo -------------------------------
echo "Program finished with exit code" $TMP
echo "Elapsed time:" $(python -c "print(round(($END_TIME - $START_TIME) * 1000, 3))") "ms"
exit 0
