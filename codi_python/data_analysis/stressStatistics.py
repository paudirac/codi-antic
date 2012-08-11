#!/usr/bin/python
# -*- coding: UTF-8 -*-

# m'he de posar d'acord en com s'utilitza aquest bitxo.
# què més he de fer amb l'stress? histograma per a cada strain donat?
# ajuntant la info de molts runs?

from histograms import *
from file_managers import * 
from sys_info import SysInfo
from directory_apply import directories_list
import pyx
import os, sys

def new__str__(time):
    def nouStr(self):
        str_ = ""
        for i in range(self.nbins):
            x_left, x_right = self.posicions[i]
            counts = self.bins[i]
            if counts > 0:
                str_ += "%e %e %e\n" % (time, x_left, counts)
        return str_
    return nouStr

def plot(hist, nframe):
    data = pyx.graph.data.list(hist.list(), x=1, y=2)
    g = pyx.graph.graphxy(width=8)
    g.plot(data, [pyx.graph.style.histogram()])
    g.writeEPSfile("stressHist%04d" % nframe)

XX = 4 - 1
XY = 5 - 1
YY = 6 - 1
def statisticsOnFrame(options): #nframe, fitxer):
    nframe = options.nframe
    fitxer = options.fitxer
    sys.stderr.write("extraient frame %d de %s\n" % (nframe, fitxer))
    sys_info = SysInfo(open(fitxer).readlines()[0])
    frame_info, stressos = frame(nframe, fitxer, sys_info.sys_size)
                                                                   
    V = sys_info.V
    NX = sys_info.NX
    delta_x = sys_info.delta_x
    time = parse_time(frame_info)
    strain = 2. * V * time / ((NX - 0.5) * delta_x)

    nbins = 100
    hist = HistogramMaker(extractComponent(XY, stressos),
            nbins, tipus_linear)
    hist.str = new__str__(strain)
    if options.check:
        print hist.xmin(), hist.xmax()
        return
    print hist.str(hist) # no m'agrada
    if options.graphics: plot(hist, nframe)

def extractComponent(compo, stressos):
    dades = []
    for stress in stressos:
        dades.append(stress[compo])
    return dades

def extractFromDirectories(options): #directoriesFile, nframe, fitxer):
    directoriesFile = options.directories
    nframe = options.nframe
    fitxer = options.fitxer
    directories = directories_list(directoriesFile)
    for dir in directories:
        aquestFitxer = os.path.join(dir, fitxer)
        statisticsOnFrame(nframe, aquestFitxer)
        sys.stderr.write("extret %s\n" % aquestFitxer)

def extractMultiples(options): #multiples, fitxer):
    multiples = options.multiples
    fitxer = options.fitxer
    nframeIni, nframeFin, step = map(int, multiples.split(","))
    for nframe in range(nframeIni, nframeFin + 1, step):
        options.nframe = nframe
        options.fitxer = fitxer
        statisticsOnFrame(options)#nframe, fitxer)

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-f", "--fitxer", type="string")
    parser.add_option("-n", "--nframe", type="int")
    parser.add_option("-d", "--directories", type="string")
    parser.add_option("-m", "--multiples", type="string")
    parser.add_option("-g", "--graphics", action="store_true")
    parser.add_option("-c", "--check", action="store_true")

    (options, args) = parser.parse_args()
    if options.directories and options.fitxer and options.nframe:
        extractFromDirectories(options) #.directories, 
                #options.nframe, options.fitxer)
    elif options.fitxer and options.nframe:
        statisticsOnFrame(options)#.nframe, options.fitxer)
    else:
        if options.multiples and options.fitxer:
            extractMultiples(options)#.multiples, options.fitxer)
