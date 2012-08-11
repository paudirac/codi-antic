#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys,Gnuplot,time
from itertools import izip
from sys_info import SysInfo
from file_managers import frames

nom_fitxer_posicions = "posicions"
        
def wait(for_this=0.01):
    time.sleep(for_this)

def main(directori, fitxer=None):
    if fitxer is None:
        fitxer = nom_fitxer_posicions
    fitxer = os.path.join(directori, fitxer)

    g = Gnuplot.Gnuplot()
    sys_info = SysInfo(open(fitxer).readlines()[0])
    print sys_info

    for frame_info, posicions in frames(fitxer, sys_info.sys_size):
        title = "frame: %s t = %s" % tuple(frame_info.split())
        g.reset()
        g("set title '%s'" % title)
        g.plot(Gnuplot.Data(posicions))

def mainPelis(directori, fitxer=None):
    if fitxer is None:
        fitxer = nom_fitxer_posicions
    fitxer = os.path.join(directori, fitxer)

    g = Gnuplot.Gnuplot()
    sys_info = SysInfo(open(fitxer).readlines()[0])
    print sys_info

    g("set term postscript enhanced color")
    foto = 0
    for frame_info, posicions in frames(fitxer, sys_info.sys_size):
        title = "frame: %s t = %s" % tuple(frame_info.split())
        if foto % 5 == 0:
            g.reset()
            g("set output 'foto%04d.eps'" % foto)
            g("set title '%s'" % title)
            g("set xrange[-5:35]")
            g("set yrange[-20:60]")
            g.plot(Gnuplot.Data(posicions))
        foto = foto + 1
                                       
def main2(directori, fitxer, fitxer2):
    fitxer = os.path.join(directori, fitxer)
    fitxer2 = os.path.join(directori, fitxer2)
    
    g = Gnuplot.Gnuplot()
    sys_info = SysInfo(open(fitxer).readlines()[0])
    print sys_info

    fr1 = frames(fitxer, sys_info.sys_size)
    fr2 = frames(fitxer2, sys_info.sys_size)
    for fr1_info, fr1_pos in fr1:
        fr2_info, fr2_pos = fr2.next()
        title = "frame: %s t = %s" % tuple(fr2_info.split())
        g("set title '%s'" % title)
        g.plot(fr1_pos, fr2_pos)

if __name__ == "__main__":
    directori = None
    fitxer = None
    fitxer2 = None
    try:
        directori = sys.argv[1]
    except: pass
    try:
        fitxer = sys.argv[2]
    except: pass

    try:
        fitxer2 = sys.argv[3]
    except:
        fitxer2 = None

    main = mainPelis

    if directori is None: directori = "./"
    if fitxer2 is None:
        main (directori, fitxer)
    else:
        main2 (directori, fitxer, fitxer2)
