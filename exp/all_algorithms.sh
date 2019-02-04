#!/bin/bash

input=$1
cat $input | tail -n +2 >temp1
#echo "#input_volume comm_time comp_time" >temp2
awk '{printf("%d %0.9f %0.9f\n", ($1)*8, $2, $NF - $2-$4);}' temp1 >temp2
rm temp1
Y=$(awk 'BEGIN{max_memory=0;}{if($1>max_memory)max_memory=$1;}END{printf("%d", max_memory);}' temp2)
SCC=$(awk 'BEGIN{sum=0;}{sum+= $2 + $3;}END{printf("%0.9f", sum);}' temp2)
#echo "Y=$Y"
mv temp2 input.txt


nalgs=12

echo "#capacity sum_comm_cmp" >capacity.txt

>makespans.txt
X=$Y
INC=$((Y/10))
for((i=0; i<10; i++))
do
g++ -g heuristics.cpp -DCAPACITY=$X  -o heuristics
for((j=0; j<$nalgs; j++))
do
./heuristics $j 
done > temp
awk '{print $2}' temp | sed ':a;N;$!ba;s/\n/ /g' >>makespans.txt
echo $X $SCC>>capacity.txt


X=$((X+INC))
done

awk '{print $1}' temp | sed ':a;N;$!ba;s/\n/ /g' >header.txt
rm temp

cat makespans.txt >>header.txt
paste capacity.txt header.txt
