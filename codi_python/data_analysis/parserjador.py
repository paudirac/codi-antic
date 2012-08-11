#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, re

def parseSize(nom):
    nx, ny = nom.split("x")
    return int(nx), int(ny), int(nx) * int(ny)

def parseVelo(nom):
    y, size, velo, t = nom.split("_")
    units, decimal = velo[1:].split("p")
    return float(units) + float("." + decimal)

def main(fin):
    fiter = open(fin, 'r')
    print "# nx ny size velo young eyoung gamma0 egamma0"
    while True:
        nom = fiter.next()[3:-1]
        data = fiter.next()[:-1]
        dirSize, dirVelo, fitxer = nom.split("/")
        nx,ny,size = parseSize(dirSize)
        velo = parseVelo(dirVelo)
        print "%d %d %d %.2f %s" % (nx, ny, size, velo, data)

if __name__ == "__main__":
    main(sys.argv[1])
