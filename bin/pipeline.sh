#!/bin/bash

# Get current date. Replace if necessary
DATE=$(date '+%Y-%m-%d %H:%M:%S')
FOLDER="../$(date '+%Y%m%d-%H%M%S')"

# Observation parameters
OBSLONG=2.349
OBSLAT=48.864
ALT=45
AZ=270
INTEGRATION=3
FFTBINS=1024
#FREQ=1420405751 # HI line
FREQ=100000000
SR=2048000
##

#FOLDER="../$(echo $DATE | tr ' ' '_')"
mkdir $FOLDER

echo "Spectral aquisition"
sudo rtl_sdr -f $FREQ -s $SR -n $(($SR*INTEGRATION)) "$FOLDER/observation.dat"

echo "Reference acquisition. Disconnect antenna and press enter"
read
sudo rtl_sdr -f $FREQ -s $SR -n $(($SR*INTEGRATION)) "$FOLDER/reference.dat"

echo "Spectral averaging"
./avgfft "$FOLDER/observation.dat" | gawk -v BINS=$FFTBINS -v SR=$SR -v FREQ=$FREQ '{print ((NR*SR)/BINS + FREQ-(SR/2)), $1}' > "$FOLDER/spectralfft.txt"
./avgfft "$FOLDER/reference.dat" | gawk -v BINS=$FFTBINS -v SR=$SR -v FREQ=$FREQ '{print ((NR*SR)/BINS + FREQ-(SR/2)), $1}' > "$FOLDER/reffft.txt"

echo "Processing"
./proc -d "$FOLDER/spectralfft.txt" -r "$FOLDER/reffft.txt" > "$FOLDER/result.txt"

echo "Plotting"
./plot -output "$FOLDER/plot.png" "$FOLDER/result.txt" 

