#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os,sys

def main(directori, fitxer, columna, snaps=1201):
    fitxer = os.path.join(directori, fitxer)
    data = open(fitxer).readlines()
    data = [map(float, dada.split()) for dada in data]
    
    out = sys.stdout

    runs = int(len(data)/snaps)
    totals = [0. for i in range(runs)]
    print >> sys.stderr, "Calculant sobre %d runs" % runs

    run = 0
    total = 0.
    for dada in data:
        index = int(dada[0])
        valor = dada[columna - 1]
        total = total + valor
        if index == snaps - 1:
            totals[run] = total
            total = 0.
            run = run + 1

    for total in totals:
        print "%d %e" % (totals.index(total), total)

    norm = 1. / float(len(totals))
    Total = 0.
    for total in totals:
        Total += norm * total
    print >> sys.stderr, "%.5f" % total

if __name__ == "__main__":
    directori = sys.argv[1]
    fitxer = sys.argv[2]
    columna = int(sys.argv[3])
    try:
        snaps = sys.argv[4]
    except:
        snaps = None

    if snaps is None:
        main(directori, fitxer, columna)
    else:
        main(directori, fitxer, columna, snaps)
