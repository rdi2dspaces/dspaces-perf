#!/bin/bash

for nputs in [0-9]* ; do
    for trial in ${nputs}/fastput.[0-9]node.[1-5] ; do
        nserv=$(echo $trial | sed 's/^.*\.\([0-9]*\)node.*$/\1/')
        iter=$(echo $trial | sed 's/^.*\.\([1-5]\)$/\1/')
        awk 'BEGIN {FS="[:, ]"} FNR==1 {count=1} /put_wait/ { printf("%i,%i,%i,%i,%f,%f\n",'${nputs}','${nserv}','${iter}',count,$10,$14); count++}' $trial/trace*

    done
done
