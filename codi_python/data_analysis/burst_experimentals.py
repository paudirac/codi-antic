#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os, math
from burst_detector import BurstDetector, Realitzacio, BurstIntegrator
from burst_detector import TimeLapses

def parse_and_apply(algo):
    try:
        directori = algo.split()[0]
        main(directori)
    except Exception, e:
        raise e

class RealitzacioStressm(Realitzacio):
    def __init__(self, nom_fitxer):
        super(RealitzacioStressm, self).__init__(nom_fitxer)
        self.data = []
        for snap, t, strain, stressR in stressm(nom_fitxer):
            self.data.append(stressR)

class ExperimentalBursts(BurstDetector):
    def __init__(self, w_data, threshold=None):
        super(ExperimentalBursts, self).__init__(w_data, threshold=None)
        first_point = self.data[0]
        index, value = first_point
        self.highest = -1.e9
        self.threshold = 0.

    def _dispatch_state(self, point, state):
        index, value = point
        if state == BurstDetector.NOBURST:
            if value > self.highest:
                self.highest = value
                nstate = BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                nstate = BurstDetector.BURST
        elif state == BurstDetector.BURST:
            if value > self.highest:
                self.highest = value
                self._append_burst()
                nstate = BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                nstate = BurstDetector.BURST
        return nstate

def stressm(fitxer):
    for line in open(fitxer).readlines():
        line = map(float, line.split())
        snap, t, strain = line[:3]
        stressR = line[15-1]
        yield (snap, t, strain, stressR)

def main(directori, fitxer="stressm.dat",out=None):
    fitxer = os.path.join(directori, fitxer)
    if out is None:
        out = sys.stdout

    stressm = RealitzacioStressm(fitxer)
    constants = ExperimentalBursts(stressm)
    bursts_exp = constants.bursts()
    #bi = BurstIntegrator(bursts_baixades)

    tl = TimeLapses(bursts_exp)
    outfile = os.path.join(directori, "time_lapses_experimentals.dat")
    open(outfile, "w").writelines(str(tl))
    print >> sys.stderr, "printing", outfile

if __name__ == "__main__":
    directori = sys.argv[1]
    main(directori)
