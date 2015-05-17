@echo off
windres -O coff -o rsrc.res rsrc.rc

if exist rsrc.res gcc -Wall -std=c99 -mwindows -g -o matrix_calculator.exe matrix_calculator_gui.c implementation.c matrix_implementation.c rsrc.res
pause