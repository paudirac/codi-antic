#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys, math
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

def nonaffine_positions(sys_info, frame_info, posicions):
    X = (sys_info.NX - 0.5) * sys_info.delta_x
    Y = (sys_info.NY - 0.5) * sys_info.delta_y
    x0 = X / 2.
    y0 = Y / 2.
    V = sys_info.V
    t = parse_time(frame_info)
    gamma =  - V * t / X

    nonaffine = []
    for x,y in posicions:
        xa = gamma * (x - x0)
        xna = x - xa
        yna = y # provisional
        nonaffine.append((xna, yna))
    return nonaffine

def main(directori, posicions="posicions"):
    nom_fitxer = os.path.join(directori, posicions)
    sys_info = SysInfo(open(nom_fitxer).readlines()[0])

    out = None

    printa_nonaffine_header(sys_info, out)

    for frame_info, posicions in frames(nom_fitxer, sys_info.phys_size):
        nonaffine = nonaffine_positions(sys_info, frame_info, posicions)
        printa_nonaffine(frame_info, nonaffine, out)


if __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    main(directori)
