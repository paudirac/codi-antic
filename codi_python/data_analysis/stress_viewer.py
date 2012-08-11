#!/usr/bin/env python2.4
# -*- coding: UTF-8 -*-

import os,sys,Gnuplot,time
from sys_info import SysInfo
from file_managers import frames

nom_fitxer_stressos = "historia_stress"

def wait(for_this=0.01):
    time.sleep(for_this)

def main(directori):
    g = Gnuplot.Gnuplot()
    nom_fitxer = os.path.join(directori, nom_fitxer_stressos)
    sys_info = SysInfo(open(nom_fitxer).readlines()[0])
    print sys_info

    for frame_info, stressos in frames(nom_fitxer, sys_info.phys_size):
        g.reset()
        g("set pm3d")
        g("set dgrid %d,%d" % (sys_info.NX, sys_info.NY))
        g("set hidden3d")
        g("set ticslevel 0")
        g("set title '%s'" % frame_info[:-1])
        stressos = [(x, y, syy) for i, x, y, sxx, sxy, syy in stressos]
        g.splot(Gnuplot.Data(stressos))
        wait()
    
if  __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    if directori is None: directroi = "./"
    main (directori)
