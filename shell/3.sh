#!/bin/zsh
n=20
ans=1
while test $n -gt 0
do
	ans=$[$ans * $n]
	n=$[$n-1]
done
echo n的阶乘为$ans
