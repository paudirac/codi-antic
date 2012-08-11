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

class BaixadesStress(BurstDetector):
    def __init__(self, w_data, threshold=None):
        super(BaixadesStress, self).__init__(w_data, threshold=None)
        first_point = self.data[0]
        index, value = first_point
        self.last_value = value
        self.threshold = 0.

    def _dispatch_state(self, point, state):
        index, value = point
        if state == BurstDetector.NOBURST:
            if value > self.last_value:
                nstate = BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                nstate = BurstDetector.BURST
        elif state == BurstDetector.BURST:
            if value > self.last_value:
                self._append_burst()
                nstate = BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                nstate = BurstDetector.BURST
        self.last_value = value
        return nstate

class SaltsStress(BurstIntegrator):
    def __init__(self, bursts):
        super(SaltsStress, self).__init__(bursts)

    def _integrate(self):
        for index, dada in enumerate(self._burst_list):
            init, fin = dada
            indexmax, stressmmax = self.data[init]
            indexmin, stressmmin = self.data[fin]
            integral = stressmmax - stressmmin
            if integral < 0.:
                integral = 0.
            if indexmin - indexmax < 3:
                del self._burst_list[index]
            self.integral.append(integral)

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
    baixades = BaixadesStress(stressm)
    bursts_baixades = baixades.bursts()
    #bi = BurstIntegrator(bursts_baixades)
    salts = SaltsStress(bursts_baixades)

    outfile = os.path.join(directori, "baixades_stress.dat")
    open(outfile, "w").writelines(str(salts))
    print >> sys.stderr, "printing", outfile

    tl = TimeLapses(bursts_baixades)
    outfile = os.path.join(directori, "time_lapses_stress.dat")
    open(outfile, "w").writelines(str(tl))
    print >> sys.stderr, "printing", outfile

if __name__ == "__main__":
    directori = sys.argv[1]
    main(directori)
