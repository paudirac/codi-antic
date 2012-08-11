#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys, math
from sys_info import SysInfo

def main(directori, fitxer="forces", out=None):
    fitxer = os.path.join(directori, fitxer)
    #sys_info = SysInfo(open(fitxer).readlines()[0])
    if out is None:
        out = sys.stdout
    #print >> out, sys_info

    counter = 0
    for line in open(fitxer).readlines()[1:]:
        if not counter % 2 == 0:
            velos_snap = line.split(":")[1:-1]
            velos_snap = [tuple(map(float, tuple(velo.split()))) \
                         for velo in velos_snap]
            for velo in velos_snap:
                vx, vy = velo
                v = math.sqrt(vx * vx + vy * vy)
                print >> out, vx, vy, v
        else: pass
        counter = counter + 1

if __name__ == "__main__":
    directori = sys.argv[1]
    main(directori)
