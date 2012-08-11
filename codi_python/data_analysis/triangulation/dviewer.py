#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), "./../"))
from file_managers import frames
from sys_info import SysInfo
from triangulate import Triangulation

def dislocate(five, seven):
    xf, yf = five
    xs, ys = seven
    return 0.5 * (xf + xs), 0.5 * (yf + ys)

class DislocationFilter(object):
    def __init__(self, filename):
        self.sys_info = SysInfo(open(filename, 'r').readline())
        self.frames = frames(filename, self.sys_info.sys_size)

    def next(self):
        frame_info, posicions = self.frames.next()
        walls = posicions[self.sys_info.phys_size:]
        ywalls = [y for x,y in walls]
        ymax, ymin = max(ywalls), min(ywalls)
        toTriangulate = [(x,y) for x,y in posicions if ymin < y < ymax]
        trian = Triangulation(toTriangulate)
        dislos = trian.dislocations()
        dislos = [dislocate(f,s) for f,s in dislos if
                    f not in walls and
                    s not in walls]
        return frame_info, dislos, toTriangulate[:self.sys_info.phys_size], \
            walls

    def __iter__(self):
        return self

def main(directori):
    import Gnuplot
    posicionsf = os.path.join(directori, "posicions")
    g = Gnuplot.Gnuplot()
    for frame_info, dislocations, posicions, walls in DislocationFilter(posicionsf):
        g.reset()
        g("set title '%s'" % frame_info[:-1])
        to_plot = [Gnuplot.Data(walls)]
        to_plot.append(Gnuplot.Data(posicions, with="points pt 6"))#dots"))
        #print len(posicions)
        if dislocations:
            to_plot.append(Gnuplot.Data(dislocations, with="points pt 6"))
        g.plot(*to_plot)
        import time
        time.sleep(0.1)

global fiter
def quetriga(iter):
    a,b,c,d = iter.next()

def test(directori):
    import timeit
    global fiter
    posicionsf = os.path.join(directori, "posicions")
    fiter = DislocationFilter(posicionsf)
    t = timeit.Timer(stmt="quetriga(fiter)",
            setup="from __main__ import quetriga, fiter")
    print t.timeit(10) / 10. * 1200

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-t", "--test", action="store_true")

    (options, args) = parser.parse_args()
    if options.test: test(args[0])
    else: main(args[0])
