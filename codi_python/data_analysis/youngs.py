#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys

def derivate(data):
    def forward(index):
        x_0,y_0 = data[index]
        x_1,y_1 = data[index + 1]
        return x_0,(y_1 - y_0) / (x_1 - x_0)
    
    deriv_data = [forward(index) for index in range(len(data) - 1)]
#    deriv_data = [(x,abs(y)) for x,y in deriv_data]
    return deriv_data

class TrocosPositius(object):

    POSITIU = 0
    NEGATIU = 1
    
    def __init__(self, stress):
        self.stress = stress
        self.trocos = [[]]
        self._current = 0
        self._trocos()

    def _trocos(self):
        state = TrocosPositius.POSITIU
        self._ant = self.stress[0]
        for i in range(len(self.stress)):
            point = self.stress[i]
            state = self._dispatch_state(point, state)

    def _mes_gran(self, point):
        e,s = point
        e_ant, s_ant = self._ant
        if s_ant <= s:
            return True
        else:
            return False

    def _dispatch_state(self, point, state):
        if state == TrocosPositius.POSITIU:
            if self._mes_gran(point):
                self._add_to_current(point)
                self._ant = point
                return TrocosPositius.POSITIU
            else:
                self._add_troc()
                self._ant = point
                return TrocosPositius.NEGATIU
        else:
            if self._mes_gran(point):
                self._add_to_current(point)
                self._ant = point
                return TrocosPositius.POSITIU
            else:
                self._ant = point
                return TrocosPositius.NEGATIU

    def _add_troc(self):
        self.trocos.append([])
        self._current = self._current + 1

    def _add_to_current(self,point):
        current = self.trocos[self._current]
        current.append(point)

    def __str__(self):
        str_ = "# tram\tdelta_e\t\tdelta_s\t\tyoung\n"
        str_points = ""
        for troc in self.trocos:
            e_0,s_0 = troc[0]
            e_f,s_f = troc[-1]
            delta_e = e_f - e_0
            delta_s = s_f - s_0
            length = len(troc)
            if length > 1:
                young_troc = str(delta_s / delta_e)
            else:
                young_troc = "#"
            str_ += "  %d\t%f\t%f\t%s\n" % (self.trocos.index(troc),
                                    delta_e, delta_s, young_troc)
            str_points += self._str_troc(troc)
        str_ += "# only positive slope points"
        str_ += str_points
        return str_

    def _str_troc(self, troc):
        str_ = ""
        for punt in troc:
            str_ += "%f %f\n" % punt
        return str_
            

def main(directori):
    fitxer = os.path.join(directori, "stressm.dat")
    stressm = open(fitxer).readlines()
    stressm = [map(float,tuple(dada.split())) for dada in stressm]
    stress = [(dada[3-1],dada[15-1]) for dada in stressm]
    deriv = derivate(stress)

    tp = TrocosPositius(stress)
    print tp

#     def imask(ds):
#         if ds > 0.: return True
#         else: return False

#     imask_ = [deriv.index(dada) for dada in deriv
#               if imask(dada[1])]

#     youngs = [deriv[i] for i in imask_]

#     import Gnuplot
#     import time
#     g = Gnuplot.Gnuplot()
#     g.plot(youngs)
#     time.sleep(3)

#     for e,s in youngs:
#         print e,s

if __name__ == "__main__":
    directori = "./"

    main(directori)


#     def mascara(ds):
#         if ds > 0.: return 1.
#         else: return 0.

#     mask = [(e, mascara(ds)) for e,ds in deriv]
