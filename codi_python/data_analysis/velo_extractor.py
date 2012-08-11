#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys, math, random
from sys_info import SysInfo
from file_managers import frames

class default_out(object):
    def write(self, something): pass

particula = 0
class DefaultAcumulator(object):
    def __init__(self): self.frame_info = ""
    def acumulate(self, *args, **kws):
        data = "%e %e %e %e" % args
        print self.frame_info[:-1], data
        #pass
acumulator = DefaultAcumulator()

def extract_velos(velos_snap, out, acumulator):
    for index, velo in enumerate(velos_snap):
        vx, vy = velo
        v2 = vx * vx + vy * vy
        v = math.sqrt(v2)
        if index == particula:
            acumulator.acumulate(vx,vy,v,v2)
        print >> out, vx, vy, v

def main(directori, fitxer="forces", out=None, acumulator=None):
    fitxer = os.path.join(directori, fitxer)
    sys_info = SysInfo(open(fitxer).readlines()[0])
    global particula
    particula = random.choice(range(sys_info.NX * sys_info.NY))
    print >> sys.stderr, "particula", particula
    if out is None:
        out = default_out()
    if acumulator is None:
        acumulator = DefaultAcumulator()
    #print >> out, sys_info

    for frame_info, velos_snap in frames(fitxer, sys_info.NX * sys_info.NY):
        #print >> sys.stderr, frame_info
        acumulator.frame_info = frame_info
        extract_velos(velos_snap,out,acumulator)

if __name__ == "__main__":
    directori = sys.argv[1]
    main(directori)
