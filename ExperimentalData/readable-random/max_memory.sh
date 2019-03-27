#!/bin/bash

nfiles=$1
min_memory_requirement=0

echo "Sum(comm) Sum(comp) min_time Sum(comm)+Sum(comp)"

for((i=0; i<$nfiles; i++))
do
input=output_readable_${i}.txt
cat $input | tail -n +2 >temp1
awk '{printf("%d %0.9f %0.9f\n", ($1)*8, $2, $NF - $2-$4);}' temp1 >temp2
rm temp1
Y=$(awk 'BEGIN{max_memory=0;}{if($1>max_memory)max_memory=$1;}END{printf("%d", max_memory);}' temp2)

if [ $min_memory_requirement -lt $Y ];
then
min_memory_requirement=$Y
fi

#SCC=$(awk 'BEGIN{sum=0;}{sum+= $2 + $3;}END{printf("%0.9f", sum);}' temp2)
(awk 'BEGIN{sum_comm=0; sum_comp=0;}{sum_comm+= $2; sum_comp+= $3;}END{printf("%0.9f %0.9f %0.9f %0.9f", sum_comm,
                                                                              sum_comp,
                                                                              sum_comm>sum_comp?sum_comm:sum_comp,
                                                                              sum_comm+sum_comp);}' temp2) >temp1

#echo "Min_memory_required($i) = $Y"
sum_comm=$(awk '{print $1}' temp1)
sum_comp=$(awk '{print $2}' temp1)
min_time=$(awk '{print $3}' temp1)
sum_comm_comp=$(awk '{print $4}' temp1)
echo $sum_comm $sum_comp $min_time $sum_comm_comp
done;
echo "Overall Minimum meory requirement = $min_memory_requirement"

##echo "#input_volume comm_time comp_time" >temp2
#SCC=$(awk 'BEGIN{sum=0;}{sum+= $2 + $3;}END{printf("%0.9f", sum);}' temp2)
##echo "Y=$Y"
#mv temp2 input.txt


