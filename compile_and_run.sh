#!/bin/bash
make all
echo -------------------------------
START_TIME=$(date +%s.%6N)
./main.exe
TMP=$?
END_TIME=$(date +%s.%6N)
echo -------------------------------
echo "Program finished with exit code" $TMP
echo "Elapsed time:" $(python -c "print(round(($END_TIME - $START_TIME) * 1000, 3))") "ms"
