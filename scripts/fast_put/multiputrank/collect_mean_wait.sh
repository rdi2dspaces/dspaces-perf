#!/bin/bash

echo "nputs, nserv, trial1, trial2, trial3, trial4, trial5"

for nputs in [0-9]*  ; do
    for nserv in 1 2 ; do
        echo -n ${nputs},${nserv}
        for trial in ${nputs}/fastput.${nserv}node.[1-5] ; do
            (cd $trial && pprof -s | awk 'BEGIN{seen=0} /put_wait/ {if(!seen) {printf(",%i",$6); seen=1}}')
        done 
        echo
    done
done
