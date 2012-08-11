#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys
from sys_info import SysInfo

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        main(directory)
    except Exception, e: raise e

def parse(line): # index = columna - 1
    line   = line.split()
    snap   = int(line[0])
    t      = float(line[1])
    strain = float(line[2])
    fxR = float(line[3])
#     fxR    = float(line[14])
    return snap, t, strain, fxR

DELTAX = 1.122462048309373
DELTAY = 0.97208064861983279

class ElasticEnergyFilter(object):
    def __init__(self, filename, nx):
        self.nx = nx
        self.fiter = open(filename, 'r')
        self.w = 0.
        self.xAnt = 0.

    def next(self):
        snap, t, strain, fxR = parse(self.fiter.next())
        x = DELTAX * strain
        x = strain # no fa falta posar-li el DELTAX, oi?, per calcular l'ene
        dx = x - self.xAnt
        self.xAnt = x
        self.w = self.w + dx * fxR
        return snap, t, strain, self.w

    def __iter__(self):
        return self

def main(directori, nx=0):
    fstress = os.path.join(directori, "consolidated.dat")#Tallat.dat")
    fwname = os.path.join(directori, "eEnergy.dat")#Tallat.dat")
    fw = open(fwname, 'w')
    for dada in ElasticEnergyFilter(fstress, nx):
        fw.write("%d %f %f %f\n" % dada)
    fw.close()
    sys.stderr.write("escrivint %s\n" % fwname)

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-x", None, type="int")

    (options, args) = parser.parse_args()
    main(args[0], options.x)
