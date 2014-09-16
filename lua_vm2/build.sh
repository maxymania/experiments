#!/bin/sh

cc -c *.c
cc *.o src/*.o arc/*.o -o test
