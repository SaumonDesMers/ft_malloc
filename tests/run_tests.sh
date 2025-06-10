#!/bin/sh
gcc -o test_intra tests/test.c

echo "Without malloc:"
/usr/bin/time -f %R ./test_intra
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test_intra

echo "With malloc:"
/usr/bin/time -f %R ./test_intra 0
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test_intra 0

echo "With malloc and free:"
/usr/bin/time -f %R ./test_intra 0 0
/usr/bin/time -f %R ./tests/run_custom_malloc.sh ./test_intra 0 0