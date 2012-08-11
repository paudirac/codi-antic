#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys
from bursts_derivada_mitjana_i_smooth import smooth

def parse_and_apply(algo):
    directori = None
    try:
        directory,outfile = algo
        main(directori, outfile=outfile)
    except:
        try:
            directori = algo.split()[0]
            main(directori)
        except Exception, e: raise e

nom_fitxer_stress = "stressm.dat"

def forward(data):
    x0,y0 = data[0]
    deriv = []
    for x,y in data[1:]:
        dydx = (y - y0) / (x - x0)
        deriv.append((x0,dydx))
        x0,y0 = x,y
    return deriv

def get_data(nom_fitxer):
    lines = [line.split() for line in open(nom_fitxer).readlines()[1:]]
    return [(float(e_str), float(fxR))
            for snap,t,e_str,xx,xy,yy,Lxx,Lxy,Lyy, 
            Rxx,Rxy,Ryy,fxL,fyL,fxR,fyR in lines]

def main(directori, outfile="d2fxR_destrain2.dat"):
    nom_fitxer = os.path.join(directori,nom_fitxer_stress)
    outfile = os.path.join(directori, outfile)

    data = get_data(nom_fitxer)
    data = smooth(data, m=20)
    first = forward(data)
    first = smooth(first, m=20)
    second = forward(first)
    second = smooth(second, m=20)

    print "printing", outfile
    outfile = open(outfile,'w')
    for e_strain,d2fxR in second:
        print >> outfile, e_strain, d2fxR
    outfile.close()

if __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    if directori is None: directori = "./"
    main (directori)
