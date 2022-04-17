#!/bin/bash
if [ "$1" == "CLEAN" ]; then
	make clean
	shift
fi
if [ "$1" == "DEBUG" ]; then
	make DEBUG=1 all
	make debug
	exit 0
fi
if [[ $1 =~ (RELEASE|THREADS) ]]; then
	make $1=1 all
	shift
else
	make all
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
