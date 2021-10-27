#!/bin/bash
#SBATCH -J fast_put          # Job name
#SBATCH -o fast_put.o%j       # Name of stdout output file
#SBATCH -e fast_put.e%j       # Name of stderr error file
#SBATCH -p normal          # Queue (partition) name
#SBATCH -N 5             # Total # of nodes
#SBATCH -n 130             # Total # of mpi tasks
#SBATCH -t 00:10:00       # Run time (hh:mm:ss)
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH --mail-user=philip.e.davis@rutgers.edu

ulimit -c unlimited
ulimit -v unlimited
pwd
date
#export FI_PROVIDER=mlx,verbs
export PATH=/home1/04726/tg840257/dspaces-perf/build/bin:/home1/04726/tg840257/sw/frontera/dspaces-prod/bin:$PATH
export LD_LIBRARY_PATH=/home1/04726/tg840257/sw/frontera/dspaces-prod/lib:$LD_LIBRARY_PATH
export FI_PROVIDER=verbs
export FI_MR_CACHE_MAX_COUNT=0
export FI_OFI_RXM_USE_SRX=1
export MY_MPIRUN_OPTIONS="-ordered-output -prepend-rank -print-all-exitcodes"
export APEX_TRACE_EVENT=1

export ABT_THREAD_STACKSIZE=2097152
export ABT_MEM_MAX_NUM_STACKS=8

export APEX_PROFILE_OUTPUT=1
#export APEX_OTF2=1
export APEX_TIME_TOP_LEVEL_OS_THREADS=1
export APEX_UNTIED_TIMERS=1
#export DSPACES_NUM_HANDLERS=32
#export DSPACES_DEBUG=1

trials=5

echo $PATH

for nputs in 1 2 4 8 16 ; do
    ./size_job.sh $nputs 1 1
done

#for nputs in 512 2048 8196 32768 ; do
#    for nserv in 1 2 ; do
#        echo "done $nputs put test with $nserv servers"
#        ./size_job.sh $nputs $nserv 128
#    done
#done
