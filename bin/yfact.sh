#!/bin/sh

getpwr() {
	avgpwr -f 1420 2>/dev/null 
}

read -p "Insert load and press enter"
REF=$(getpwr)
echo "Ref: $REF"
read -p "Remove load and press enter"

while true; do
	PWR=$(getpwr)
	Y=$(echo "scale=2; $REF / $PWR" | bc)
	echo "Power: $PWR  Y-fact: $Y"
done

exit 0
