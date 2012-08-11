#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os
from burst_detector import BurstDetector, Realitzacio, BurstIntegrator

def parse_and_apply(algo):
    try:
        directori = algo.split()[0]
        main(directori)
    except Exception, e:
        raise e

class RealitzacioDStressm(Realitzacio):
    def __init__(self, nom_fitxer):
        super(RealitzacioDStressm, self).__init__(nom_fitxer)

        def stressm(fitxer):
            for line in open(fitxer):
                line = map(float, line.split())
                snap, t, strain = line[:3]
                stressR = line[14]
                yield snap, t, strain, stressR

        raw_data = [(strain, stressR) for snap, t, strain, stressR
                    in stressm(nom_fitxer)]
        
        def domam2(dades):
            diter = iter(dades)
            now = diter.next()
            while True:
                next = diter.next()
                yield now, next
                now = next
        
        self.data = []
        for (x0, y0), (x1, y1) in domam2(raw_data[21:]):
            derivada = - (y1 - y0) / (x1 - x0) # derivada canviada de signe
            self.data.append(derivada)
            #print x0, derivada
            #(comprovat: dóna lo mateix que la forward de l'xmg)

    def threshold(self):
        #mitjana = 0.
        #norm = 1. / float(len(self.data))
        #for ds in self.data:
            #if ds > 0:
                #mitjana = mitjana + norm * ds
        #maxim = max(self.data)
        #thresh = mitjana / 10 * (1 + (0.005 / 100) * maxim)
        return 0.

def main(directori):
    stressmf = os.path.join(directori, "stressm.dat")
    data = RealitzacioDStressm(stressmf)
    threshold = data.threshold()
    
    bd = BurstDetector(data)
    bd.set_threshold(threshold)

    bursts = bd.bursts()
    bi = BurstIntegrator(bursts)
    bi.integrate_with_x_from(column=3)
    outfile = os.path.join(directori, "burst_dstressm.dat")
    outfile = open(outfile, 'w')
    print >> outfile, bi
    print >> sys.stderr, "printing", outfile.name
    outfile.close()

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()

    (options, args) = parser.parse_args()
    main(args[0])
