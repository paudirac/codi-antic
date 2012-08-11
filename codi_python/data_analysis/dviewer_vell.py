#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Filename: dviewer.py
# Author: Pau Cervera cervera at ffn dot es
# status: codificant-se
"""
Printa historia defectes. Potser en primera implementació és
monocromàtic.
"""

import time
def pausa(t=0.05):
    time.sleep(t)

class Frame:
    def __init__(self, lines, nom):
        self.nom = nom
        self.data_dict = {5 : [], # cincs
                          7 : [], # sets
                         -1 : [],
                    "paret" : [],
                    "extra" : [],}
        for line in lines:
            snap, t, e_strain, xd, yd, set, x7, y7, cinc, x5, y5 \
                  = map(float, line.split())
            if set == 0 and cinc == 0:
                self.data_dict["paret"].append((xd,yd))
            else:
                self.data_dict[-1].append((xd,yd))

#                 self.data_dict[5].append((x5, y5))
#                 self.data_dict[7].append((x7,y7))

        self.data_dict["extra"].append(self.data_dict["paret"][-1])

        self.t = t
        self.e_strain = e_strain

        to_remove = [k for k,v in self.data_dict.iteritems() \
                     if len(v) == 0]
        for k in to_remove:
            self.data_dict.pop(k)

    def draw(self, g):
        nom_titol = "%s t = %.5f e_s = %.5f" % (self.nom,
                                                self.t, self.e_strain)
        nom_file = self.nom[:-4] + ".gif"
        g("set title '%s'" % nom_titol)

#         print "escrivint %s" % nom_file
#         g("set output '%s'" % nom_file)

        g.plot(*self.data_dict.values())

import Gnuplot

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

def pinta_dades(g, file_in="historia_defectes"):
    parser = HistoriaParser()
    frames = []
    for line in open(file_in).readlines():
        if parser.is_enough(line):
            frame = parser.getFrame()
            frame.draw(g)
            pausa()

def main(n_frames, n_zfill=4, cwd="./", file_in="historia_defectes"):
    import os
    cwd = os.getcwd()
    if cwd[-1] != "/":
        cwd = cwd + "/"
    
    g = Gnuplot.Gnuplot()
#     g("set terminal gif")
#     g("set xrange[-1:27]")
#     g("set yrange[-5:25]")

    pinta_dades(g)

from optparse import OptionParser
import sys
if __name__ == "__main__":
    global n_zfill
    parser = OptionParser()
#     parser.add_option("-x","--xrange", help="posa l'xrange",
#                       action="callback", callback=set_xrange)
#     parser.add_option("-y","--yrange", help="posa l'yrange",
#                       action="callback", callback=set_yrange)
    
    (options, args) = parser.parse_args()
    try:
        n_frames = int(args[0])
        n_zfill = int(args[1])
        file_in = args[2]
    except:
        print __doc__
        sys.exit(-1)

    main(n_frames,n_zfill,file_in=file_in)
    sys.exit(0)
