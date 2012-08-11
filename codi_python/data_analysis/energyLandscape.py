#!/usr/bin/env python2.4
# -*- coding: UTF-8 -*-

import os,sys,Gnuplot,time
from sys_info import SysInfo
from file_managers import frames

def wait(for_this=0.01): time.sleep(for_this)

def energies(nom_fitxer, n):
    for frame_info, posicions in frames(nom_fitxer, n):
        enes = []
        for punt1 in (posicions):
            ei = 0.
            xi,yi = punt1
            for punt2 in (posicions):
                if punt1 is not punt2:
                    xj,yj = punt2
                    r2 = (xj - xi) * (xj - xi) + (yj - yi) * (yj - yi)
                    over_r2 = 1./r2
                    over_r6 = over_r2 * over_r2 * over_r2
                    over_r12 = over_r6 * over_r6
                    ei = ei + 4. * (over_r12 - over_r6)
            enes.append((xi, yi, ei))
        yield frame_info, enes[:]

def main(directori):
    g = Gnuplot.Gnuplot()
    nom_fitxer = os.path.join(directori, "posicions")
    sys_info = SysInfo(open("posicions").readlines()[0])
    print sys_info

    for frame_info, enes in energies(nom_fitxer, sys_info.phys_size):
        g.reset()
        g("set pm3d map")
        g("set dgrid %d,%d" % (sys_info.NX, sys_info.NY))
        g("set hidden3d")
        g("set ticslevel 0")
        g("set title '%s'" % frame_info[:-1])
        g.splot(Gnuplot.Data(enes))
        wait()
    
if  __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    if directori is None: directroi = "./"
    main (directori)
