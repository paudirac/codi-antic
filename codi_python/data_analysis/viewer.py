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
        if len(posicions) < 10000:
            g.plot(Gnuplot.Data(posicions))
        else:
            g.plot(Gnuplot.Data(posicions, with_="dots"))

def mainPygame(directori, fitxer=None):
    if fitxer is None: fitxer = nom_fitxer_posicions
    fitxer = os.path.join(directori, fitxer)
    sys_info = SysInfo(open(fitxer).readlines()[0])
    print sys_info

    import pygame
    dimensionsInPixels = (640, 480)
    dimensionsInPixels = (1024, 800)
    srf = pygame.display.set_mode(dimensionsInPixels)
    
    dimensionsInLengths = ((sys_info.NX + 2 + 2) * sys_info.delta_x,
                           (sys_info.NY + 2 * sys_info.ny_extra + 2 + 2) * 
                            sys_info.delta_y)
    def coord(x,y):
        pixx, pixy = dimensionsInPixels
        pixx, pixy = pixx - 10, pixy - 10
        lx, ly = dimensionsInLengths
        x0 = sys_info.delta_x * 2.1
        y0 = sys_info.ny_extra * sys_info.delta_y
        x = x + x0
        y = y + y0
        #print x,y
        #assert (x >= 0)
        #assert (y >= 0)
        pix = int(x * pixx / lx) + 10 #+ pixx/2
        piy = int(y * pixy / ly) + 10 #+ pixy/2
        #print pix, piy
        return pix, piy

    radius = int(0.25 * dimensionsInPixels[0] * sys_info.delta_x / dimensionsInLengths[0])
    #print "radius", radius

    for frame_info, posicions in frames(fitxer, sys_info.sys_size):
        srf.fill((255,255,255))
        for (x,y) in posicions:
            x,y = coord(x,y)
            pygame.draw.circle(srf, (55, 0, 200), (x,y), radius, 0)
        pygame.display.flip()

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

    if directori is None: directori = "./"
    if fitxer2 is None:
        #main (directori, fitxer)
        mainPygame(directori, fitxer)
    else:
        main2 (directori, fitxer, fitxer2)
