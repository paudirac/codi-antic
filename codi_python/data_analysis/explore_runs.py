#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os
from itertools import izip
from directory_apply import directories_line, directories_list

def len_fitxer(directories, fitxer):
    nom = os.path.join(directories[0], fitxer)
    return len(open(nom).readlines())

def len_fitxers(directories, fitxer):
    for dir in directories:
        nom = os.path.join(dir, fitxer)
        print dir, len(open(nom).readlines())
    
def means(directori, fitxer, pos):
    directories = directories_list(directori)
    noms = [os.path.join(dir, fitxer) for dir in directories]
    temporal = "temp_kkk_kk_.dat"
    open(temporal,"w")
    for nom in noms:
        f = open(temporal,"a")
        f.writelines(open(nom).readlines())
        f.close()

    #len_fitxers(directories, fitxer)

    n = len_fitxer(directories, fitxer)
    d = len(directories)
    norm = 1. / float(d)
    f = open(temporal, "r")
    means_ = [0. for i in range(n)]
    for dir in range(d):
        for lin in range(n):
            line = f.next()
            value = float(line.split()[pos - 1])
            means_[lin] = means_[lin] + norm * value

    os.remove(temporal)
    return means_

# falta posar-hi algo perquè pinti també els strains i tal

def printa_means(means_, directori, fitxer):
    directories = directories_list(directori)
    nom = os.path.join(directories[0], fitxer)
    f = open(nom)
    for line, mean_ in izip(f, means_):
        print "%s %s %s" % tuple(line.split()[:3]), mean_

def main(directori, fitxer, pos):
    means_ = means(directori, fitxer, pos)
    printa_means(means_, directori, fitxer)
    #for i, mean in enumerate(means_):
        #print i, mean

if __name__ == "__main__":
    default_directori = "./"
    try:
        directori = sys.argv[1]
    except:
        directori = default_directori
    
    try:
        fitxer = sys.argv[2]
    except:
        print __doc__

    try:
        pos = int(sys.argv[3])
    except:
        pos = 0
    
    main(directori, fitxer, pos)
