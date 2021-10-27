#!/bin/bash

echo "nputs, worst1, worst2, worst3, worst4, worst5, worst6, worst7, worst8, worst9, worst10"

for nputs in [0-9]*  ; do
    echo -n $nputs
    awk 'BEGIN{FS="[, ]"} /put_wait/ {if(count){print $8}else{count=1}}' ${nputs}/fastput.1node.[1-5]/trace* | sort -n | tail -10 | awk '{printf(",%i",$1)}'
    echo
done
