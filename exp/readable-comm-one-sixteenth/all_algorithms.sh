#!/bin/bash

nfiles=$1
min_memory_requirement=0


for((k=0; k<$nfiles; k++))
do
input=output_readable_${k}.txt
cat $input | tail -n +2 >temp1
Y=$(awk 'BEGIN{max_memory=0;}{if(($1) *8 >max_memory)max_memory= $1 * 8;}END{printf("%d", max_memory);}' temp1)
if [ $min_memory_requirement -lt $Y ];
then
min_memory_requirement=$Y
fi

done;


>capacity_values.txt
>makespan_values.txt

for((k=0; k<$nfiles; k++))
do
input=output_readable_${k}.txt
cat $input | tail -n +2 >temp1
#echo "#input_volume comm_time comp_time" >temp2
awk '{printf("%d %0.9f %0.9f\n", ($1)*8, ($2)/16, $NF - $2-$4);}' temp1 >temp2

awk 'BEGIN{sum_comm=0; sum_comp=0;}{sum_comm+= $2; sum_comp+= $3;}END{printf("%0.9f %0.9f %0.9f %0.9f", sum_comm,
                                                                              sum_comp,
                                                                              sum_comm>sum_comp?sum_comm:sum_comp,
                                                                              sum_comm+sum_comp);}' temp2 >temp1

#echo "Min_memory_required($i) = $Y"
sum_comm=$(awk '{print $1}' temp1)
sum_comp=$(awk '{print $2}' temp1)
min_time=$(awk '{print $3}' temp1)
sum_comm_comp=$(awk '{print $4}' temp1)

rm temp1

mv temp2 input.txt


nalgs=13


X=$min_memory_requirement
INC=$((min_memory_requirement/10))
for((i=0; i<10; i++))
do
g++  ../heuristics.cpp -DCAPACITY=$X  -o heuristics
for((j=0; j<$nalgs; j++))
do
./heuristics $j 
done > temp
awk '{print $2}' temp | sed ':a;N;$!ba;s/\n/ /g' >>makespan_values.txt
echo $X $sum_comm_comp $min_time $sum_comm $sum_comp>>capacity_values.txt


X=$((X+INC))
done

done

echo "#capacity sum_comm_cmp max(sum_comm,sum_comp) sum_comm sum_comp" >capacity.txt
cat capacity_values.txt >>capacity.txt
awk '{print $1}' temp | sed ':a;N;$!ba;s/\n/ /g' >makespans.txt
cat makespan_values.txt >>makespans.txt

rm temp capacity_values.txt makespan_values.txt

paste capacity.txt makespans.txt
