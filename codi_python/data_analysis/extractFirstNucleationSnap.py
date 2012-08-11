#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys
from file_managers import frame, defectFrame
from sys_info import *
import math
import pyx

def writeFrame(dir, frame_info, posicions):
    fitxer = os.path.join(dir, "primeraNucleacio.dat")
    sys.stderr.write("escribint %s ...\n" % fitxer)
    fout = open(fitxer, "w")
    for part in posicions:
        fout.write("%f %f\n" % part)
    fout.close()

class Punt(tuple):
    _radius = 0.5 * math.pow(2., 1./6.)
    def __init__(self, args):
        self.x, self.y = args
        self.color = pyx.color.rgb.black
    def shape(self):
        return pyx.path.circle(self.x, self.y, Punt._radius)
    def attributes(self):
        return [pyx.style.linewidth.THIN, self.color]
    def show(self, canvas):
        canvas.stroke(self.shape(), self.attributes())

class Cinc(Punt):
    def __init__(self, args):
        super(Cinc, self).__init__(args)
        self.color = pyx.color.rgb.red
    def show(self, canvas):
        canvas.fill(self.shape(), self.attributes())

class Set(Punt):
    def __init__(self, args):
        super(Set, self).__init__(args)
        self.color = pyx.color.rgb.green
    def show(self, canvas):
        canvas.fill(self.shape(), self.attributes())

def pageFrame(dir, frame_info, posicions, cincs, sets, document = None):
    cincs = [Cinc(cinc) for cinc in cincs]
    sets = [Set(set) for set in sets]
    posicions = [Punt(punt) for punt in posicions 
                if punt not in cincs and punt not in sets]
    posicions.extend(cincs)
    posicions.extend(sets)

    canvas = pyx.canvas.canvas()
    for punt in posicions:
        punt.show(canvas)
    sys.stderr.write("dibuixant la pàgina %s ...\n" % dir)
    return pyx.document.page(canvas, pagename=dir,
            paperformat=pyx.document.paperformat.A4,
            fittosize=1) # ahí, ahí

def main(directori):
    fname = os.path.join(directori, "firstNucleations.Info")
    lines = open(fname,"r").readlines()
    lines = [line.split() for line in lines]
    lines = [(line[0], int(line[1])) for line in lines]
    document = pyx.document.document()
    for dir, framenum in lines:
        fitxer = os.path.join(dir, "posicions")
        fitxer = os.path.join(directori, fitxer)
        fitxerDef = os.path.join(dir, "historia_defectes_n")
        fitxerDef = os.path.join(directori, fitxerDef)
        sys_info = SysInfo(open(fitxer).readlines()[0])
        frame_info, posicions = frame(framenum, fitxer, sys_info.sys_size)
        frame_info, (walls, dislos, cincs, sets) = defectFrame(framenum, fitxerDef)
        writeFrame(dir, frame_info, posicions)
        page = pageFrame(dir, frame_info, posicions, cincs, sets)
        document.append(page)
    fitxer = os.path.join(directori, "firstNucleations")
    open(fitxer + ".ps", "w") # m'asseguro que està buit
    sys.stderr.write("escribint %s.ps...\n" % fitxer)
    document.writePSfile(fitxer)

if __name__ == "__main__":
    try:
        directori = sys.argv[1]
    except:
        sys.stderr.write("Merda!")
        sys.exit(-1)

    main(directori)
