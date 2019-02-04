#!/bin/bash


nfiles=$1
mkdir readable
for((i=0; i<$nfiles; i++))
do
input=output_${i}.txt
output=output_readable_${i}.txt
{
echo "#input_size get_time acc_size acc_time op_type overall_time";
awk '{ if($9 == "get_time="){ $4 += $8; $6+=$10;} printf("%d %0.9f %d %0.9f %s %0.9f\n", $4, $6,
        $(NF-6), $(NF-4), $(NF-1), $NF); }' $input
}>readable/$output
done;
