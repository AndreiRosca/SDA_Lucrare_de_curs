@echo off
windres -O coff -o rsrc.res rsrc.rc

if exist rsrc.res gcc -Wall -std=c99 -g -o matrix_arithmetic.exe matrix_arithmetic.c implementation.c matrix_implementation.c rsrc.res
pause