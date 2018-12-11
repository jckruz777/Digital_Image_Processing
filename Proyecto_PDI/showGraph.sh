#!/bin/bash

VERSION_FOLDERS=$(ls -d */)
clean_versions=$(echo "${VERSION_FOLDERS///}" | tr '\n' '\t')

# Generating data by executiong the versions

IMAGE_NAME="waterfall.png"

for f in $clean_versions
do
	cd $f/ && make clean
	make
	./$f $IMAGE_NAME
	cd ..
done


# Generating Plots

touch data_min.dat
touch data_max.dat

echo "se_size	$clean_versions" > data_min.dat
echo "se_size	$clean_versions" > data_max.dat

kSizes=$(awk '{print $1}' Serial/data.dat | grep -v "se_size")
addmin=""
addmax=""

folder_counter=1
for v in $VERSION_FOLDERS
do
	minTimes=$(awk '{print $2}' ${v}data.dat | grep -v "min")
	
	if [ $folder_counter -eq 1 ]
	then
		addmin=$(paste <(printf %s "$kSizes") <(printf %s "$minTimes"))
	else
		addmin=$(paste <(printf %s "$addmin") <(printf %s "$minTimes"))
	fi
	
	maxTimes=$(awk '{print $3}' ${v}data.dat | grep -v "max")

	if [ $folder_counter -eq 1 ]; then
		addmax=$(paste <(printf "%s\n" "$kSizes") <(printf "%s\n" "$maxTimes"))
	else
		addmax=$(paste <(printf "%s\n" "$addmax") <(printf "%s\n" "$maxTimes"))
	fi

	folder_counter=$(($folder_counter + 1))
done

SAMPLES=$(($(cat Serial/data.dat | wc -l) - 1))
WORDS=$(echo $addmin | wc -w)

set -- $addmin
echo "${addmin[0]}" >> data_min.dat

set -- $addmax
echo "${addmax[0]}" >> data_max.dat

gnuplot -e "load 'results_min.plt';pause -1"
gnuplot -e "load 'results_max.plt';pause -1"
