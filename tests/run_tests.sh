#!/bin/sh
gcc -o test tests/test.c

echo "Without malloc:"
/usr/bin/time -f %R ./test
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test

echo "With malloc:"
/usr/bin/time -f %R ./test 0
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test 0

echo "With malloc and free:"
/usr/bin/time -f %R ./test 0 0
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test 0 0