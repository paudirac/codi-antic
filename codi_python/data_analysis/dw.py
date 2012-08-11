#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys, math, random
from sys_info import SysInfo
from file_managers import frames, parse_time

def extract_forces(forces, t, dsf_list):
    for index, force in enumerate(forces):
        fx, fy = force
        dsf_list[index].append((t, force))

def main(directori, fitxer="forces", out=None):
    fitxer = os.path.join(directori, fitxer)
    sys_info = SysInfo(open(fitxer).readlines()[0])
    if out is None:
        out = sys.stdout

    for frame_info, forces in frames(fitxer, sys_info.NX * sys_info.NY):
        t = parse_time(frame_info)
        extract_forces(forces, t, dsf_list)

if __name__ == "__main__":
    directori = sys.argv[1]
    main(directori)
