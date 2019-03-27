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

echo "#Capacity Gilmore_Gomory_makespan" >results_Gilmore_Gomory.txt

for((k=0; k<$nfiles; k++))
do
input=output_readable_${k}.txt
cat $input | tail -n +2 >temp1
#echo "#input_volume comm_time comp_time" >temp2
awk '{printf("%d %0.9f %0.9f\n", ($1)*8, ($2)/1, $NF - $2-$4);}' temp1 >temp2

rm temp1

mv temp2 input.txt




X=$min_memory_requirement
g++-7  ~/DataTransferSimulation/exp/Gilmore_Gomory.cpp -DCAPACITY=$X  -o Gilmore_Gomory
./Gilmore_Gomory >> results_Gilmore_Gomory.txt
#INC=$((min_memory_requirement/10))
#for((i=0; i<10; i++))
#do
##echo "Memory Capacity = $X"
#g++-7  ~/DataTransferSimulation/exp/Gilmore_Gomory.cpp -DCAPACITY=$X  -o Gilmore_Gomory
#./Gilmore_Gomory >> results_Gilmore_Gomory.txt
#
#X=$((X+INC))
#done

done
