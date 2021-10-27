#!/usr/bin/python3

from statistics import mean,stdev

times=[]

with open('tmp') as f:
    while True:
        line = f.readline()
        if line:
            times.append(int(line))
        else:
            break

print(int(stdev(times)), end='')

