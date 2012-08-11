#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys
from sys_info import SysInfo
from file_managers import frames, parse_time

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        mainDefault(directory)
    except Exception, e: raise e

def main(directori, fitxer=None):
    if fitxer is None: fitxer = "posicions"
    fitxer = os.path.join(directori, fitxer)
    sys_info = SysInfo(open(fitxer).readlines()[0])
    print sys_info

    length0 = None
    for frame_info, posicions in frames(fitxer, sys_info.phys_size):
        xmaxtup = max(posicions)
        xmintup = min(posicions)
        xmax = xmaxtup[0]
        xmin = xmintup[0]
        if length0 is None: length0 = xmax - xmin
        strainReal = (xmax - xmin) / length0
        t = parse_time(frame_info)
        print frame_info[:-1], 1 - (xmax - xmin) / length0, 2 * sys_info.V * t / length0

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()

    (options, args) = parser.parse_args()
    main(args[0])
