#!/bin/bash
echo "I am working"
for((thr=1;thr<25;thr++))
do
	sum=0
	for((tries=0;tries<2;tries++))
	do
		v=$(./GameOfLife 1000 500 ${thr})
		echo "thread $thr try $tries = $v"
		sum=$((sum+ v))
	done
	sum=$((sum/2))
	echo $sum >> timePerCore.csv
	echo "thread $thr average = $sum"
done