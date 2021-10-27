#!/bin/bash -x

nputs=$1
nserv=$2
nproc=$3
ntrials=5

mkdir -p $nputs
cd $nputs

nputs=$(($nputs/$nproc))

for iter in `seq 1 ${ntrials}` ; do
    sleep 5
    echo "## Config file for DataSpaces
    ndim = 1
    dims = ${nproc}
    max_versions = 10
    num_apps = 1
    hash_version = 1" > dataspaces.conf

    rm conf.ds

    echo "starting ${nserv}node iteration ${iter}"
    mkdir -p dspaces.${nserv}node.${iter}
    export TRACEDIR=dspaces.${nserv}node.${iter}
    export APEX_OUTPUT_FILE_PATH=${TRACEDIR}
    export APEX_OTF2_ARCHIVE_PATH=${TRACEDIR}
    export IBRUN_TASKS_PER_NODE=1
    ibrun -n $nserv -o 0 task_affinity dspaces_server verbs &> dspaces-${nserv}.${iter}.log &
    while [ ! -f conf.ds ]; do
        sleep 1s
    done
    echo "started server"
    mkdir -p fastput.${nserv}node.${iter}
    export TRACEDIR=fastput.${nserv}node.${iter}
    export APEX_OUTPUT_FILE_PATH=${TRACEDIR}
    export APEX_OTF2_ARCHIVE_PATH=${TRACEDIR}
    export IBRUN_TASKS_PER_NODE=32
    startproc=$(($IBRUN_TASKS_PER_NODE * $nserv))
    ibrun -n ${nproc} -o $startproc task_affinity fast_put -s 10 -p $nputs > fastput.${nserv}node.${iter}.log
    echo "finished puts"
done

cd ..
