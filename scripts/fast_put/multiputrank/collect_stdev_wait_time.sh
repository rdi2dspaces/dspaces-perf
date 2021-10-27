#!/bin/bash

echo "nputs, nserv, trial1, trial2, trial3, trial4, trial5"

for nputs in [0-9]*  ; do
    for nserv in 1 2 ; do
        echo -n ${nputs},${nserv}
        for trial in ${nputs}/fastput.${nserv}node.[1-5] ; do
            echo -n > tmp
            for prof in ${trial}/profile.* ; do 
                pprof -s -f $prof | awk 'BEGIN{seen=0} /put_wait/ {if(!seen) {print $6; seen=1}}' >> tmp
            done
            echo -n ","
            ./calc_stdev.py
        done
        echo
    done
done
