#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os

def parse_and_apply(algo):
    try:
        directori = algo
        main(directori=algo, file_in="historia_defectes")
    except Exception, e: raise e

class InWall(tuple):
    def __init__(self, args):
        super(InWall, self).__init__(args)
    def __str__(self):
        return "W %f %f" % self

class Cinc(tuple):
    def __init__(self, args):
        super(Cinc, self).__init__(args)
    def __str__(self):
        return "5 %f %f" % self

class Set(tuple):
    def __init__(self, args):
        super(Set, self).__init__(args)
    def __str__(self):
        return "7 %f %f" % self

class Dislo(tuple):
    def __init__(self, args):
        super(Dislo, self).__init__(args)
    def __str__(self):
        return "D %f %f" % self
   
class Frame:
    def __init__(self, lines, nom):
        self.nom = nom
        self.snap = 0
        self.time = 0.
        self.strain = 0.
        self.walls = []
        self.dislos = []
        self.cincs = []
        self.sets = []

        for line in lines:
            snap, t, e_strain, xd, yd, set, x7, y7, cinc, x5, y5 \
                  = map(float, line.split())
            if set == 0 and cinc == 0:
                self.walls.append(InWall((xd, yd)))
            else:
                self.dislos.append(Dislo((xd,yd)))
                self.cincs.append(Cinc((x5,y5)))
                self.sets.append(Set((x7,y7)))

        self.snap = snap
        self.time = t
        self.strain = e_strain

    def convert(self, stream=None):
        if stream is None: stream = sys.stdout
        self.writeHeader(to=stream)
        self.write(self.dislos, to=stream)
        self.write(self.cincs, to=stream)
        self.write(self.sets, to=stream)
        self.write(self.walls, to=stream, last=True)

    def writeHeader(self, to=sys.stdout):
        to.write("%d %f %f\n" % (int(self.snap), self.time, self.strain))

    def write(self, particles, to=sys.stdout, last=False):
        for particle in particles:
            to.write(":")
            to.write(str(particle))
        if last:
            to.write(":\n")

import re, string
pattern_start = re.compile('pystart')
pattern_end = re.compile('pyend')

class HistoriaParser:
    def __init__(self):
        self.list = []
        self.nom_frame = None

    def append(self, item):
        self.list.append(item)
    
    def is_enough(self,line):
        if pattern_start.search(line):
            self.nom_frame = line.split()[1]
            return False
        if pattern_end.search(line):
            return True
        if self.nom_frame:
            self.append(line)
            return False
        
    def getFrame(self):
        frame = Frame(self.list, self.nom_frame)
        self.__init__()
        return frame

def converteix_dades(file_in):
    fileout = open(file_in + "_n", "w")
    fileout.write("#linia extra per al dviewer\n")
    print >> sys.stderr, "convertint %s al nou format" % file_in
    parser = HistoriaParser()
    frames = []
    for line in open(file_in).readlines():
        if parser.is_enough(line):
            frame = parser.getFrame()
            frame.convert(fileout)
    fileout.close()

def main(directori="./", file_in="historia_defectes"):
    fitxer = os.path.join(directori, file_in)
    converteix_dades(file_in=fitxer)

if __name__ == "__main__":
    dir = "./"
    file_in = "historia_defectes"
    try:
        dir = sys.argv[1]
        file_in = sys.argv[2]
    except:
        print
        print "Supply a dir and filename"
        print
        sys.exit(1)

    main(directori="./", file_in=file_in)
    sys.exit(0)
