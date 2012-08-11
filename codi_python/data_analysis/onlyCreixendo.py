#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        onlyCreixendo(directory)
    except Exception, e: raise e

def parse(line): # index = columna - 1
    line = line.split()
    snap = int(line[0])
    t = float(line[1])
    strain = float(line[2])
    e = float(line[3])
    return snap, t, strain, e

class OnlyCreixendoFilter(object):
    def __init__(self, filename):
        self.fiter = open(filename, 'r')
        snap, t, strain, e = parse(self.fiter.next())
        self.eAnt = e
    def next(self):
        snap, t, strain, e = parse(self.fiter.next())
        if e < self.eAnt:
            self.eAnt = e
            return self.next()
        else:
            self.eAnt = e
            return snap, t, strain, e
    def __iter__(self):
        return self

def onlyCreixendo(directori):
    nomFitxer = os.path.join(directori, "ene_consTallat.dat")
    nomFitxerOut = os.path.join(directori, "ene_consTallatCreixendo.dat")
    fout = open(nomFitxerOut, 'w')
    for data in OnlyCreixendoFilter(nomFitxer):
        fout.write("%d %f %f %f\n" % data)
    sys.stderr.write("escrivint %s\n" % nomFitxerOut)
    fout.close()
    os.rename(nomFitxerOut, nomFitxer)
    sys.stderr.write("renombrant %s a %s\n" % (nomFitxerOut, nomFitxer))
