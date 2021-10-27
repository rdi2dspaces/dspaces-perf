#!/usr/bin/python3

import csv
import numpy as np
from statistics import mean,stdev
import matplotlib.pyplot as plt

timers = {}
with open('writer1rank.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        if row[0] == 'nputs':
            continue
        size = row[0]
        timers[size] = {}
        timers[size]['raw'] = [(float(x) / 1e6) / int(size) for x in row[1:] ]
        timers[size]['avg'] = mean(timers[size]['raw'])
        timers[size]['stdev'] = stdev(timers[size]['raw'])


with open('rank1worst.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        if row[0] == 'nputs':
            continue
        size = row[0]
        timers[size]['worst_raw'] = [(float(x) / 1e6) / int(size) for x in row[1:] ]
        timers[size]['worst_avg'] = mean(timers[size]['worst_raw'])
        timers[size]['worst_stdev'] = stdev(timers[size]['worst_raw'])


indices = [int(x) for x in timers.keys()]
indices.sort()

res = [ timers[str(x)]['avg'] for x in indices ]
err = [ timers[str(x)]['stdev'] for x in indices ]
wres = [ timers[str(x)]['worst_avg'] for x in indices ]
werr = [ timers[str(x)]['worst_stdev'] for x in indices ]

X = np.arange(len(indices))
width = 0.35
plt.bar(X, res, width, yerr=err, label='avg')
plt.bar(X + width, wres, width, yerr=werr, label='worst')
plt.ylabel('time(s)')
plt.xlabel('puts per timestep')
plt.title('Wait time per-put (1 writer rank)')
plt.xticks(X + width / 2., indices)
plt.legend()
plt.savefig('singlerankwait.png')

plt.cla()

res = [ timers[str(x)]['worst_avg'] / timers[str(x)]['avg'] for x in indices ]

X = np.arange(len(indices))
width = 0.35
plt.bar(X, res, width)
plt.ylabel('time(s)')
plt.xlabel('puts per timestep')
plt.yscale('log')
plt.title('Ratio of worst 10 to avg')
plt.xticks(X, indices)
plt.savefig('singlerankwaitratio.png')

plt.cla()

timers = {}
with open('rank1all.csv') as f:
    data = csv.reader(f, delimiter=',')
    for row in data:
        if row[0] == 'nputs':
            continue
        size = row[0]
        timers[size] = {}
        timers[size]['raw'] = [(float(x) / 1e6) for x in row[1:] ]
        timers[size]['avg'] = mean(timers[size]['raw'])
        timers[size]['stdev'] = stdev(timers[size]['raw'])


indices = [int(x) for x in timers.keys()]
indices.sort()

res = [ timers[str(x)]['raw'] for x in indices ] 

quant = np.arange(0.0, 1.0, .25)
quants = []
for i in range(len(res)):
    quants.append(quant)

X = np.arange(len(indices))
width = 0.35
plt.violinplot(res, widths=0.7, showmeans=False, showmedians=True, quantiles=quants)
plt.ylabel('time(s)')
plt.yscale('log')
plt.xlabel('puts per timestep')
plt.title('Wait time per-put (1 writer rank)')
plt.xticks(X + 1, indices)
plt.savefig('singlerankviolin.png')

