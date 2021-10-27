#!/usr/bin/python3

import csv
import numpy as np
from statistics import mean,stdev
import matplotlib.pyplot as plt
from math import log2
import seaborn as sns

timers = {}
with open('rank1full.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        size = int(row[0])
        nserv = int(row[1])
        trial = int(row[2])
        step = int(row[3])
        if (size, nserv) not in timers:
            timers[(size, nserv)] = []
        if len(timers[(size, nserv)]) <= (trial - 1):
            timers[(size, nserv)].append([])
        timers[(size, nserv)][trial-1].append([float(row[4]) / 1e6,float(row[5]) / 1e6]) 

norm_factor = {}
for (size, nserv) in timers:
    for trial in timers[(size, nserv)]:
        baseline = trial[0][0]
        for i in range(len(trial)):
            trial[i][0] -= baseline
        if (size not in norm_factor) or norm_factor[size] < (trial[-1][0] + trial[-1][1]):
            norm_factor[size] = trial[-1][0] + trial[-1][1]

for (size, nserv) in timers:
    for trial in timers[(size, nserv)]:
        for i in range(len(trial)):
            trial[i][0] /= norm_factor[size]
            trial[i][1] /= norm_factor[size]

#sizes = sorted({x[0] for x in timers})
sizes = [1, 2]
colors = sns.color_palette("colorblind", len(sizes))
for j in range(len(sizes)):
    for i in range(len(timers[(size,1)])):
        plt.broken_barh(timers[(sizes[j],1)][i], ((15*i)+(5 * j),5), color=colors[j])

plt.savefig('rank1full.png')
