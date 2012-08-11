#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys
import velocities, histograms

def parse_and_apply(algo):
    try:
        directori = algo.split()[0]
        try:
            calculate_histogram_velocities_for(directori)
        except:
            print >> sys.stderr, "**** %s failed ****" % directori
    except Exception, e: raise e

def calculate_histogram_velocities_for(directori):
    """Calcula l'histograma de velocitats del directori i
       esborra els fitxers temporals que utilitza."""
    v_name = "v.dat"
    v_filename = os.path.join(directori, "v.dat")
    out_v = open(v_filename,"w")
    velocities.main(directori, out=out_v)
    out_v.close()

    print >> sys.stderr, "Calculant l'histograma de velocitats de %s" \
        % directori
    histograms.main(directori, fitxer_data=v_name, nbins=100)
    os.remove(v_filename)

if __name__ == "__main__":
    directori = sys.argv[1]
    calculate_histogram_velocities_for(directori)
