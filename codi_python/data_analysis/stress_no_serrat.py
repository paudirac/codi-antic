#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os, sys

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        main(directory)
    except Exception, e: raise e

def parse(line): # index = columna - 1 
    line = line.split()
    snap = int(line[0])
    t = float(line[1])
    strain = float(line[2])
    fxR = float(line[14])
    return snap, t, strain, fxR

class NoSerratFilter(object):
    def __init__(self, filename):
        self.fiter = open(filename, 'r')
        self.fxRAnt = -1e21
        
    def next(self):
        snap, t, strain, fxR = parse(self.fiter.next())
        if fxR > self.fxRAnt:
            self.fxRAnt = fxR
        else:
            fxR = self.fxRAnt
        return snap, t, strain, fxR
    
    def __iter__(self):
        return self

def no_serrat(fitxer_in, fitxer_out):
    print "extraient el no_serrat de %s" % fitxer_in
    fitxer_out = open(fitxer_out,"w")
    for snap, t, strain, fxR in NoSerratFilter(fitxer_in):
        fitxer_out.write("%d %.5e %.5e %.5e\n" % (snap, t, strain, fxR))
    fitxer_out.close()
    print "escrit a %s" % fitxer_out.name

def atof(anumber):
    parts = anumber.split("p")
    return float(".".join(parts))

def parseFileName(fitxer_no_serrat):
    punt,dim,dir_name,fitxer = fitxer_no_serrat.split("/")
    print dim
    nx,ny = [int(x) for x in dim.split("x")]
    y,dim,V,T = dir_name.split("_")
    null,V = V.split("v")
    V = atof(V)
    null,T = T.split("t")
    T = atof(T)
    return nx,ny,V,T

def punts_first_load(fitxer_no_serrat):
    out = open("first_loads.dat","a")
    nx,ny,V,T = parseFileName(fitxer_no_serrat)
    lines = open(fitxer_no_serrat).readlines()
    lines = [[float(x) for x in line.split()]
             for line in lines]
    snap_a,t_a,e_strain_a,fxR_a = lines[0]
    for line in lines[20:]:
        snap,t,e_strain,fxR = line
        if fxR == fxR_a:
            #            1 2 3  4   5      6    7         8
            print >> out,T,V,nx,ny,snap_a,t_a,e_strain_a,fxR_a
            break
        else:
            snap_a,t_a,e_strain_a,fxR_a = snap,t,e_strain,fxR
    out.close()
    print "afegit %s a %s" % (fitxer_no_serrat, out.name)

def main(directori):
    arxiu_in = os.path.join(directori, "stressm.dat")
    arxiu_out = os.path.join(directori, "stressm_no_serrat.dat")
    no_serrat (arxiu_in, arxiu_out)
    punts_first_load(arxiu_out)
    print

if __name__ == "__main__":
    directori = None
    try:
        directory = sys.argv[1]
    except:
        print __doc__
        sys.exit(-1)

    if not directori:
        directori = "./"

    main(directori)
