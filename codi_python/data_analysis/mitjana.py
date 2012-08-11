#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys,os

def main(directori, fitxer, columna, snaps=1201):
    fitxer = os.path.join(directori, fitxer)
    data = open(fitxer).readlines()
    data = [map(float, dada.split()) for dada in data]
    out = sys.stdout
    norm = 1. / float(snaps)
    #for snap in range(snaps):
        #mitjana = 0
        #for dada in data:
        #    if dada[0] == snap:
                #mitjana = mitjana + norm * dada[columna - 1]
                #strain = dada[3-1]
        #print >> out, "%i %e %e" % (snap, strain, mitjana)
        #print >> sys.stderr, "%i %e %e" % (snap, strain, mitjana)

    mitjanes = [0. for i in range(snaps)]
    strains = [0. for i in range(snaps)]
    strain_col = 3 - 1
    for dada in data:
        index = int(dada[0])
        mitjanes[index] += norm * dada[columna - 1]
        strains[index] = dada[strain_col]
    
    #for i in range(snaps):
        #strain = strains[i]
        #mitjana = mitjanes[i]
        #print >> out, "%i %e %e" % (i, strain, mitjana)

    for index, mitjana in enumerate(mitjanes): #més pythonic
        strain = strains[index]
        print >> out, "%i %e %e" % (index, strain, mitjana)

if __name__ == "__main__":
    directori = sys.argv[1]
    fitxer = sys.argv[2]
    columna = int(sys.argv[3])
    try:
        snaps = int(sys.argv[4])
    except:
        snaps = None
        print >> sys.stderr, "Suposo snaps=1201"
    if snaps is None:
        main(directori, fitxer, columna)
    else:
        main(directori, fitxer, columna, snaps)
