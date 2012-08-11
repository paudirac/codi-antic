#!/usr/bin/python
# -*- coding: UTF-8 -*-

import delaunay as delny
import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), "./../"))
from file_managers import frames
from sys_info import SysInfo
from triangulate import Triangulation
import math

#def vectorSum(center, neigh):
    #return reduce(lambda x,y: x + y,
            #[complex(*n) - complex(*center) for n in neigh])

def arg(c):
    try:
        return math.atan(c.imag / c.real)
    except ZeroDivisionError:
        return 0.

def polar(c):
    return abs(c), arg(c)

def moduloSix(center, neigh):
    centrats = [complex(*n) - complex(*center) for n in neigh]
    argument = reduce(lambda x,y: x + y, map(arg, centrats)) 
    return complex(math.cos(argument),
                   math.sin(argument))

def orderInFile(posicionsf):
    sys_info = SysInfo(open(posicionsf, 'r').readline())
    for frame_info, particules in frames(posicionsf, sys_info.sys_size):
        tri = Triangulation(particules)
        order = 0.
        orderAbs = 0.
        norm = 1. / float(len(tri.neighbours))
        order2 = 0.
        for center, neigh in tri.neighbours.iteritems():
            if center in particules[:sys_info.phys_size]:
                neig = [n for n in neigh 
                    if n in particules[:sys_info.phys_size]]
                order_ = moduloSix(center, neigh)
                order = order + norm * order_
                orderAbs = orderAbs + norm * abs(order_)
                order2 = orderAbs + norm * order_ * order_
        yield frame_info, order, order2, orderAbs

def main(directori):
    posicions = os.path.join(directori, "posicions")
    fout = os.path.join(directori, "order.dat")
    fout = open(fout, 'w')
    for frame_info, order, order2, orderAbs in orderInFile(posicions):
        order_r, order_angle = polar(order)
        fout.write("%s %e %e %e %e %e %e %e\n" % 
                (frame_info[:-1], order.real, order.imag,   # 3 4
                                  order_r, order_angle,     # 5 6
                                  order2.real, order2.imag, # 7 8
                                  orderAbs))                # 9
    fout.close()

def mainG(directori):
    import Gnuplot
    posicions = os.path.join(directori, "posicions")
    sys_info = SysInfo(open(posicions, 'r').readline())
    g = Gnuplot.Gnuplot()
    for frame_info, particules in frames(posicions, sys_info.phys_size):
        g.reset()
        g("set pm3d map")
        g("set dgrid %d,%d" % (sys_info.NX, sys_info.NY))
        g("set hidden3d")
        g("set cbrange[0:2 * pi]")
        g("set title '%s'" % frame_info[:-1])
        tri = Triangulation(particules)
        ordres = [(c,moduloSix(c, neigh)) for c, neigh 
            in tri.neighbours.iteritems()]
        #moduls = [(c,abs(o)) for c,o in ordres]
        args = [c + (arg(o),) for c,o in ordres]
        g.splot(Gnuplot.Data(args))

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-g", "--gnuplot", action="store_true")

    (options, args) = parser.parse_args()
    if options.gnuplot:
        mainG(args[0])
    else:
        main(args[0])
