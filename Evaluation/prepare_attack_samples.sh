#!/bin/bash

filein=$1
fileout=$2
datastarts=$3
dataends=$4


echo "reading from " $filein "and writing in" $fileout
s=$(($dataends-$datastarts))
echo $s
head -n $dataends $filein | tail -n $s > temp_file
wc temp_file
cat temp_file | grep Tim | awk '{print $2","$3","$4","$5","$6","$7","$8","$9","$10","$11","$12","$13","$14","$15","$16","$17","$19","$20","$21","$22","$23","$24","$25","$26","$27","$28","$29","$30","$31","$32","$33","$34","$36",1";}' >> $fileout
rm temp_file
