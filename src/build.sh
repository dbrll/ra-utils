#!/bin/sh
[ ! -d ../bin ] && mkdir ../bin
cd ../bin
gcc ../src/proc.c      	-lm		-Os	-Wall	-o proc	  
gcc ../src/avgpwr.c 	-lrtlsdr	-Os	-Wall	-o avgpwr 
gcc ../src/avgfft.c	-lm -lfftw3	-Os	-Wall	-o avgfft
gcc ../src/radec.c	-lm -lnova	-Os	-Wall	-o radec
go build ../src/plot.go

