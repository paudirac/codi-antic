#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from burst_detector import Realitzacio,BurstDetector,BurstIntegrator
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

class FitxerRugRealitzacio(Realitzacio):
    def __init__(self, nom_fitxer):
        super(FitxerRugRealitzacio, self).__init__(nom_fitxer)
        self.data = open(nom_fitxer).readlines()
        self.data = [tuple(line.split()) for line in self.data]

        real_data = []
        data_obj = self
        max_len = len(data_obj) - 1
        primers_fora = 20
        index_x = 3 - 1 # strain!!!
        index_y = 6 - 1
        for i in range(primers_fora,max_len):
            dada_0 = data_obj.data[i]
            dada_1 = data_obj.data[i+1]
            x_0 = float(dada_0[index_x])
            y_0 = float(dada_0[index_y])
            x_1 = float(dada_1[index_x])
            y_1 = float(dada_1[index_y])
            dy_dx = 0.
            denom = x_1 - x_0
            if denom > 0.:
                dy_dx = (y_1 - y_0) / denom
            real_data.append(abs(dy_dx))
            #print x_0, dy_dx
        del self.data
        self.data = real_data

    def mitjana(self):
        mitjana_ = 0.
        norm = 1./float(len(self.data))
        for value in self.data:
            mitjana_ = mitjana_ + norm * value
        return mitjana_

    def threshold(self):
        #return self.mitjana()
        maxim = max(self.data)
        thresh = self.mitjana() / 10 * (1 + (1.5 / 100) * maxim)
        return thresh

def main(directori, threshold=None, outfile="burst.dat"):
    fitxer = os.path.join(directori, "rug.dat")

    data = FitxerRugRealitzacio(fitxer)
    # amb aquest donava bé!!!
    #if threshold is None:
        #threshold = data.mitjana() / 10. # el / 10. el copio de energy
    threshold = data.threshold() 

    bd = BurstDetector(data)
    bd.set_threshold(threshold)
    bursts = bd.bursts()

    bi = BurstIntegrator(bursts)
    bi.integrate_with_x_from(column=3)

    outfile = os.path.join(directori, outfile)
    assert_file_is_empty (outfile)
    outfile = open(outfile,"w")
    print >> outfile, bi
    print >> sys.stderr, "printing", outfile.name
    outfile.close()

if __name__ == "__main__":

    def test():
        data = FitxerRugRealitzacio("rug.dat")
        mitjana_ = data.mitjana()

        bd = BurstDetector(data)
        bd.set_threshold(mitjana_)
        bursts = bd.bursts()

        bi = BurstIntegrator(bursts)
        print bi

    test()
