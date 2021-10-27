#!/usr/bin/python3

import csv
import numpy as np
from statistics import mean,stdev
import matplotlib.pyplot as plt

timers = {1:{}, 2:{}}
with open('writer128rank.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        if row[0] == 'nputs':
            continue
        size = row[0]
        nserv = int(row[1])
        timers[nserv][size] = {}
        timers[nserv][size]['raw'] = [(float(x) / 1e6) / (int(size)/128) for x in row[2:] ]
        timers[nserv][size]['avg'] = mean(timers[nserv][size]['raw'])
        timers[nserv][size]['stdev'] = stdev(timers[nserv][size]['raw'])

indices = [int(x) for x in timers[1].keys()]
indices.sort()

res1 = [ timers[1][str(x)]['avg'] for x in indices ]
err1 = [ timers[1][str(x)]['stdev'] for x in indices ]
res2 = [ timers[2][str(x)]['avg'] for x in indices ]
err2 = [ timers[2][str(x)]['stdev'] for x in indices ]

X = np.arange(len(indices))
width = 0.35
plt.bar(X, res1, width, label='1 server', yerr=err1)
plt.bar(X + width, res2, width, label='2 server', yerr=err2)
plt.ylabel('time(s)')
plt.xlabel('total puts per timestep')
plt.title('Wait time per-put (128 writer ranks)')
plt.xticks(X + width/2, indices)
plt.yscale('log')
plt.legend(loc='upper left')
plt.savefig('128rankwait.png')

plt.cla()

timers = {1:{}, 2:{}}
with open('writer128stdev.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        if row[0] == 'nputs':
            continue
        size = row[0]
        nserv = int(row[1])
        timers[nserv][size] = {}
        timers[nserv][size]['raw'] = [(float(x) / 1e6) / (int(size)/128) for x in row[2:] ]
        timers[nserv][size]['avg'] = mean(timers[nserv][size]['raw'])
        timers[nserv][size]['stdev'] = stdev(timers[nserv][size]['raw'])


indices = [int(x) for x in timers[1].keys()]
indices.sort()

res1 = [ timers[1][str(x)]['avg'] for x in indices ]
err1 = [ timers[1][str(x)]['stdev'] for x in indices ]
res2 = [ timers[2][str(x)]['avg'] for x in indices ]
err2 = [ timers[2][str(x)]['stdev'] for x in indices ]

X = np.arange(len(indices))
width = 0.35
plt.bar(X, res1, width, label='1 server', yerr=err1)
plt.bar(X + width, res2, width, label='2 server', yerr=err2)
plt.xlabel('total puts per timestep')
plt.ylabel('time(s)')
plt.yscale('log')
plt.title('stdev of rank times across ranks per-put (128 writer ranks)')
plt.xticks(X + width/2, indices)
plt.legend(loc='upper right')
plt.savefig('128rankstdev.png')

