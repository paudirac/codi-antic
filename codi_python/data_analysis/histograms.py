#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Així funciona, encara que no dóna pdfs, sinó histogrames.

import sys,os
import math

__all__= []

def log_binning(data, nbins):
    bins = [0. for i in range(nbins)]
    posicions = [(0., 0.) for i in range(nbins)]
    #xmin = min(data)
    #if xmin == 0.:
        #data = [dada for dada in data if dada != 0.]
    data = [dada for dada in data if dada > 0.]
    xmin = min(data)
    xmax = max(data)

    # arreclo això així perquè no passi res amb l'última dada
    imax = data.index(xmax)
    del data[imax]
    
    log_delta_x = math.log10(xmax/xmin) / float(nbins)
    delta_x = math.pow(10., log_delta_x)
    for x in data:
        i = int(math.log10(x/xmin) / log_delta_x)
     	if i not in range(nbins):
    	  print >> sys.stderr, i,nbins,x,xmin,xmax
    	  print >> sys.stderr, len(bins)
          continue
        bins[i] += 1.

    for i in range(nbins):
        x_left = xmin * math.pow(delta_x, float(i))
        x_right = xmin * math.pow(delta_x, float(i+1))
        length_bin = xmin * (delta_x - 1) * math.pow(delta_x, float(i))
        counts = bins[i]
        p = 0.
        if counts > 0.:
            p = counts / length_bin
        bins[i] = p
        posicions[i] = (x_left, x_right)

    return bins, posicions
        
def linear_binning(data, nbins): 
    bins = [0. for i in range(nbins)]
    posicions = [(0., 0.) for i in range(nbins)]
    xmin = min(data)
    xmax = max(data)
    delta_x = (xmax - xmin) / float(nbins)
    for x in data:
        i = int((x - xmin) / delta_x) - 1
        bins[i] += 1.

    for i in range(nbins):
        x_left = xmin + delta_x * float(i)
        x_right = xmin + delta_x * float(i+1)
        posicions[i] = (x_left, x_right)

    return bins, posicions

tipus_linear = "linear"
tipus_log = "log"

__all__.append("tipus_linear")
__all__.append("tipus_log")

def funcioCalculate(tipus):
    def decoradorCalculate(meth):
        def newCalculate(self, *args, **kws):
            print >> sys.stderr, "Instanciat un histograma del tipus %s" % tipus
            if tipus == tipus_linear:
                return linear_binning(*args, **kws)
            elif tipus == tipus_log:
                return log_binning(*args, **kws)
            else:
                print "Impossible, no existeix aquest típus d'histograma"
                return None
        return newCalculate
    return decoradorCalculate

__all__.append("Histogram")
class Histogram(object):
    def __init__(self, data, nbins):
        self.data = data
        self.nbins = nbins
        self.bins, self.posicions = self.calculate(data, nbins)
    def calculate(self): pass
    def _normalize(self):
        total_area = 0.
        for i in range(self.nbins):
            altura = self.bins[i]
            x_left, x_right = self.posicions[i]
            base = x_right - x_left
            area = altura * base
            total_area = total_area + area
        for i in range(self.nbins):
            count = self.bins[i]
            self.bins[i] = count / total_area
    def __str__(self):
        str_ = "#Histogram\n#x_left x_right counts ?p\n"
        for i in range(self.nbins):
            x_left, x_right = self.posicions[i]
            counts = self.bins[i]
            if counts > 0.:
                str_ += "%e %e %e\n" % (x_left, x_right, counts)
        return str_
    def __repr__(self): pass
    def xmin(self):
        x_left,x_right = self.posicions[0]
        return x_left
    def xmax(self):
        x_left,x_right = self.posicions[0]
        return x_left
    def list(self):
        return [(self.posicions[i][0], self.bins[i]) 
                for i in range(self.nbins)] # if self.bins[i] > 0.]

__all__.append("HistogramMaker")
def HistogramMaker(data, nbins, tipus):
    "Constructs a Histogram of nbins of type tipus for the data."
    class ParamHistogram(Histogram):
        @funcioCalculate(tipus)
        def calculate(self, data, nbins): pass

    hist = ParamHistogram(data, nbins)
    return hist

def main(directori, fitxer_data="bursts_dW_all.dat", nbins=100):
    fitxer = os.path.join(directori, fitxer_data)
    data = open(fitxer).readlines()
    data = [dada.split() for dada in data]
    data = [float(dada[4-1]) for dada in data]
    hist = HistogramMaker(data, nbins, tipus_log)

    out = "log_n" + str(nbins) + "_" + fitxer_data
    out = os.path.join(directori, out)
    open(out, "w").writelines(str(hist))
    print >> sys.stderr, "Escrivint %s" % out

    hist = HistogramMaker(data, nbins * 10, "linear")
    out = "linear_n" + str(nbins * 10) + "_" + fitxer_data
    out = os.path.join(directori, out)
    open(out, "w").writelines(str(hist))
    print >> sys.stderr, "Escrivint %s" % out

#    print hist
############################## Test ##########################
def power_law(rnd, alpha):
    x0 = 0.5
    x1 = 100000
    return math.pow(((math.pow(x1, alpha+1) - math.pow(x0, alpha+1)) * rnd + \
        math.pow(x0, alpha + 1)), 1./(alpha + 1))

def power_law2(rnd, alpha):
    x0 = 0.5
    return x0 * math.pow(1 - rnd, -1./(alpha - 1.))

def test ():
    import random
    rng = random.Random()
    alpha = 2.5
    #xs = [rng.paretovariate(alpha) for i in range(100000)]
    xs = [power_law2(rng.random(), alpha) for i in range(1000000)]
    #print xs
    #hist = Histogram(*linear_binning(xs, 100))
    linear_hist = HistogramMaker(xs, 100000, "linear")
    log_hist = HistogramMaker(xs, 100, "log")

    linear_out = os.path.join("./", "linear.dat")
    open(linear_out, "w").writelines(str(linear_hist))

    log_out = os.path.join("./", "log.dat")
    open(log_out, "w").writelines(str(log_hist))

    data = [str(x) + "\n" for x in xs]
    data_out = os.path.join("./", "data.dat")
    open(data_out, "w").writelines(data)

if __name__ == '__main__':
    #test ()
    directori = sys.argv[1]
    fitxer = sys.argv[2]
    nbins = int(sys.argv[3])
    main(directori, fitxer, nbins)
