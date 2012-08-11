#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from burst_detector import Realitzacio, BurstDetector, BurstIntegrator
from burst_detector import BurstMaxims
from burst_detector import TimeLapses
import os, sys

def parse_and_apply(algo):
    try:
        directori, threshold, outfile = algo.split()
        threshold = float(threshold)
        main(directori,threshold=threshold,outfile=outfile)
    except:
        try:
            directori, threshold = algo.split()
            threshold = float(threshold)
            main(directori,threshold=threshold)
        except:
            try:
                directori = algo.split()[0]
                main(directori)
            except Exception, e: raise e

def parse_options_and_apply(directori, options):
    try:
        options = options.split()
        threshold = float(options[0])
        main(directori, threshold=threshold)
    except Exception, e: raise e

def assert_file_is_empty(path):
    try:
        os.rename(path,path + "ara_et_borro")
        os.remove(path + "ara_et_borro")
    except:
        pass
    f = open(path,"w")
    f.close()


class FitxerEneRealitzacio(Realitzacio):
    def __init__(self, nom_fitxer):
#        Realitzacio.__init__(self, nom_fitxer)
        super(FitxerEneRealitzacio, self).__init__(nom_fitxer)
        self.data = open(nom_fitxer).readlines()
        self.data = [tuple(line.split()) for line in self.data]

        real_data = [float(dada[7-1]) for dada in self.data] # tots v^2
        #real_data = [float(dada[9-1]) for dada in self.data] # |v| / N
        del self.data
        self.data = real_data[21:] #em carrego el transient

    def mitjana(self):
        mitjana_ = 0.
        norm = 1. / float(len(self.data))
        for value in self.data:
            mitjana_ = mitjana_ + norm * value
        return mitjana_

    def threshold(self):
        maxim = max(self.data)
        thresh = self.mitjana () / 10 * (1 + (5.5 / 100) * maxim)
        return thresh

def main(directori, threshold=None, outfile="burst_dW.dat",
         outfile_time_lapses="time_lapses.dat"):
    fitxer = os.path.join(directori, "eneNow.dat")
    data = FitxerEneRealitzacio(fitxer)
    #if threshold is None:
        #threshold = data.mitjana()/10.
    threshold = data.threshold()

    bd = BurstDetector(data)
    bd.set_threshold(threshold)
    bursts = bd.bursts()

    bi = BurstIntegrator(bursts)
    bi.integrate_with_x_from(column=3)
    bm = BurstMaxims(bursts)

    outfile = os.path.join(directori, outfile)
    assert_file_is_empty(outfile)
    outfile = open(outfile,"w")
    print >> outfile, bi
    print >> sys.stderr, "printing", outfile.name
    outfile.close()

    outfile = os.path.join(directori, "burst_maxims.dat")
    assert_file_is_empty(outfile)
    outfile = open(outfile,"w")
    print >> outfile, bm
    print >> sys.stderr, "printing", outfile.name
    outfile.close()

    tl = TimeLapses(bursts)
    outfile = os.path.join(directori, outfile_time_lapses)
    assert_file_is_empty(outfile)
    outfile = open(outfile,"w")
    print >> outfile, tl
    print >> sys.stderr, "printing", outfile.name
    outfile.close()

if __name__ == "__main__":
    
    def test():
        data = FitxerEneRealitzacio("./ene.dat")
        print data
        mitjana_ = data.mitjana()

        bd = BurstDetector(data)
        bd.set_threshold(mitjana_)
        bursts = bd.bursts()

        print "mitjana:", mitjana_
        bi = BurstIntegrator(bursts)
        print bi

        tl = TimeLapses(bursts)
        print tl

    def test2():
        main("./")

    test()
    test2()
