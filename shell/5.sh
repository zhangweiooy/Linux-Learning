#!/bin/zsh

Sum=0
min=0
max=0
count=1
while read line
do 
	if test $count -eq 1
	then
		max=$line
		min=$line
	fi
	count=$[$count+1]
	
	if test $max -lt $line
	then
		max=$line
	fi
	Sum=$[$Sum+$line]
	if test $min -gt $line
	then
		min=$line
	fi
done < number.txt
printf "max:%d\nmin:%d\naverage:%d\n" $max $min $Sum/$[$count-1]

