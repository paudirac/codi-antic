#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os, sys
from consolidatedResponses import ConsolidatedElasticFilter
from stress_no_serrat import NoSerratFilter

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
    e = float(line[3])
    return snap, t, strain, e

class ConsolidatedElasticFilterTalla(ConsolidatedElasticFilter):
    def __init__(self, filename, minSnap=75, renormalize=False):
        super(ConsolidatedElasticFilterTalla, self).__init__(filename, renormalize)
        self.minSnap = minSnap
    def skipTrash(self, snap, t, strain, e):
        if snap < self.minSnap:
            return snap, t, strain, e
        else:
            raise StopIteration

import consolidatedResponses, stress_no_serrat
consolidatedResponses.parse = parse
stress_no_serrat.parse = parse

def no_serrat(fitxer_in, fitxer_out):
    print "extraient el no_serrat de %s" % fitxer_in
    fitxer_out = open(fitxer_out,"w")
    for snap, t, strain, e in NoSerratFilter(fitxer_in):
        fitxer_out.write("%d %.5e %.5e %.5e\n" % (snap, t, strain, e))
    fitxer_out.close()
    print "escrit a %s" % fitxer_out.name

def consolidated(fitxer_in, fitxer_out):
    print "extraient de %s" % fitxer_in
    fitxer_out = open(fitxer_out, "w")
    for snap, t, strain, e in ConsolidatedElasticFilter(fitxer_in):
        fitxer_out.write("%d %.5e %.5e %.5e\n" % (snap, t, strain, e))
    fitxer_out.close()
    print "escrit a %s" % fitxer_out.name

def consolidatedTalla(fitxer_in, fitxer_out):
    print "extraient de %s" % fitxer_in
    fitxer_out = open(fitxer_out, "w")
    for snap, t, strain, e in ConsolidatedElasticFilterTalla(fitxer_in, minSnap=150):
        fitxer_out.write("%d %.5e %.5e %.5e\n" % (snap, t, strain, e))
    fitxer_out.close()
    print "escrit a %s" % fitxer_out.name

def main(directori):
    arxiu_in = os.path.join(directori, "eneNowBe.dat")
    #arxiu_no_serrat = os.path.join(directori, "ene_no_serrat.dat")
    #arxiu_consolidated = os.path.join(directori, "ene_consolidated.dat")
    arxiu_consolidated_talla = os.path.join(directori, "ene_consTallat.dat")
    #no_serrat (arxiu_in, arxiu_no_serrat)
    #consolidated(arxiu_in, arxiu_consolidated)
    consolidatedTalla(arxiu_in, arxiu_consolidated_talla)
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
