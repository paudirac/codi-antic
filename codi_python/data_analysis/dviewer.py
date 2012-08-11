#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os,sys,Gnuplot,time
from file_managers import defectFrames

nom_fitxer_defectes = "historia_defectes_n"
        
def wait(for_this=0.01):
    time.sleep(for_this)

def main(directori, fitxer=None):
    if fitxer is None:
        fitxer = nom_fitxer_defectes
    fitxer = os.path.join(directori, fitxer)

    g = Gnuplot.Gnuplot()

    for frame_info, (walls, dislos, cincs, sets) in defectFrames(fitxer):
        title = "frame: %s t = %s strain: %s" % tuple(frame_info.split())
        g.reset()
        g("set title '%s'" % title)
        if len(walls) > 0: g.replot(walls)
        if len(dislos) > 0:
            g.replot(dislos)
            g.replot(cincs)
            g.replot(sets)

if __name__ == "__main__":
    directori = None
    fitxer = None
    try:
        directori = sys.argv[1]
    except: pass
    try:
        fitxer = sys.argv[2]
    except: pass

    if directori is None: directori = "./"
    main (directori, fitxer)
