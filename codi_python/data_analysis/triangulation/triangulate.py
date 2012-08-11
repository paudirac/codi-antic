#!/usr/bin/python
# -*- coding: UTF-8 -*-

import delaunay as delny
import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), "./../"))
from file_managers import frames
from sys_info import SysInfo

def logE(str_): sys.stderr.write(str_)
def log(str_): sys.stdout.write(str_)


class Triangulation(delny.core.Triangulation):
    def __init__(self, inputset, dim=None):
        super(Triangulation, self).__init__(inputset, dim)
    def dislocations(self):
        neighDict = self.neighbours
        fives = [k for k, v in neighDict.iteritems() if len(v) == 5]
        sevens = [k for k, v in neighDict.iteritems() if len(v) == 7]
        return [(f,s) for f in fives for s in sevens if f in neighDict[s]]
        #logE("%d %d %d\n" % (len(fives), len(sevens), len(dislos)))

def main(directori):
    posicionsf = os.path.join(directori, "posicions")
    sys_info = SysInfo(open(posicionsf, 'r').readline())
    triangulacionsf = os.path.join(directori, "triangulacions")

    for frame_info, posicions in frames(posicionsf, sys_info.phys_size):
        #sys.stderr.write(frame_info)
        tri = Triangulation(posicions)
        dislos = tri.dislocations()
        #log("%s %d\n" % (frame_info[:-1], len(dislos)))

        neig = tri.get_neighbours()
        coords = map(len, neig.values())
        coordM = sum(coords) / float(len(coords))
        log("%s %f %d\n" % (frame_info[:-1], coordM, len(coords)))

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    
    (options, args) = parser.parse_args()
    main(args[0])
