#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys, math
#from itertools import izip

from sys_info import SysInfo
from file_managers import frames, parse_time

def printa_nonaffine_header(sys_info, out):
    if out == None:
        out = sys.stdout
    out.write(sys_info.one_liner() + "\n")

def printa_nonaffine(frame_info, posicions, out=None):
    if out == None:
        out = sys.stdout
    out.write(frame_info)
    posicions = ['%f %f' % punt for punt in posicions]
    posicions = ':'.join(posicions)
    out.write(posicions + "\n")

def affine_positions(sys_info, frame_info, posicions_inicials):
    X = sys_info.NX * sys_info.delta_x
    Y = sys_info.NY * sys_info.delta_y
    x0 = X / 2.
    y0 = Y / 2.
    V = sys_info.V
    t = parse_time(frame_info)
    gamma = - V * t / X # ja es strain! no

    for x,y in posicions_inicials:
        xa = gamma * (x - x0) # hi falta el young en algun lloc
        # utilitzar que nonaffine sera 0. per la component y mentre
        # no hi hagi plástic, per trobar el módul de Young?
        ya = 0. # provisional
        yield xa, ya

def nonaffine_positions(sys_info, frame_info, posicions,
                        posicions_inicials):
    nonaffine = []
    affine = affine_positions(sys_info, frame_info, posicions_inicials)

    for punt in posicions:
        x,y = punt
        xa,ya = affine.next()
        xna = x - xa
        yna = y - ya
        nonaffine.append((xna, yna))
    return nonaffine

def main(directori, posicions="posicions"):
    nom_fitxer = os.path.join(directori, posicions)
    sys_info = SysInfo(open(nom_fitxer).readlines()[0])

    out = None

    printa_nonaffine_header(sys_info, out)

    for index, frame_posicions in enumerate(frames(nom_fitxer, 
                                            sys_info.phys_size)):
        frame_info, posicions = frame_posicions
        if index == 0:
            posicions_inicials = posicions
        else:
            nonaffine = nonaffine_positions(sys_info, frame_info, posicions,
                                            posicions_inicials)
            printa_nonaffine(frame_info, nonaffine, out)

if __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    main(directori)
