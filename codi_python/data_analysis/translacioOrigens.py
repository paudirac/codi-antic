#!/usr/bin/python
# -*- coding: UTF-8 -*-


import os, sys

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        translacioOrigen(directory)
    except Exception, e: raise e

def parse(line): # index = columna - 1
    line = line.split()
    snap = int(line[0])
    t = float(line[1])
    strain = float(line[2])
    e = float(line[3])
    return snap, t, strain, e

class TranlatedFilter(object):
    def __init__(self, filename):
        self.strainm = 1.
        self.em = 1e21
        for line in open(filename, 'r'):
            snap, t, strain, e = parse(line)
            if strain < self.strainm: self.strainm = strain
            if e < self.em: self.em = e
        self.fiter = open(filename, 'r')
    def next(self):
        snap, t, strain, e = parse(self.fiter.next())
        return snap, t, strain - self.strainm, e - self.em
    def __iter__(self):
        return self

def translacioOrigen(directori):
    nomFitxer = os.path.join(directori, "ene_consTallat.dat")
    nomFitxerOut = os.path.join(directori, "ene_consTallatTranlated.dat")
    fout = open(nomFitxerOut, 'w')
    for data in TranlatedFilter(nomFitxer):
        fout.write("%d %f %f %f\n" % data)
    sys.stderr.write("escrivint %s\n" % nomFitxerOut)
    fout.close()
    os.rename(nomFitxerOut, nomFitxer)
    sys.stderr.write("renombrant %s a %s\n" % (nomFitxerOut, nomFitxer))
