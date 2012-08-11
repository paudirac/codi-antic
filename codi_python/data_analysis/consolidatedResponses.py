#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys,os

def parse_and_apply(algo):
    try:
        directory = algo.split()[0]
        mainDefault(directory)
    except Exception, e: raise e

def parse_options_and_apply(directori, options):
    try:
        mainTalla(directori.split()[0])
    except Exception, e: raise e

def parse(line): # index = columna - 1 
    line = line.split()
    snap = int(line[0])
    t = float(line[1])
    strain = float(line[2])
    fxR = float(line[14])
    return snap, t, strain, fxR

class ConsolidatedElasticFilter(object):
    def __init__(self, filename, renormalize=True):
        self.fiter = open(filename, 'r')
        self.fxRAnt = -1e21
        self.fxRMax = -1e21
        self.strainToRest = None 
        self.renormalize = renormalize
        
    def next(self):
        snap, t, strain, fxR = parse(self.fiter.next())
        if self.strainToRest is None: # poso el zero d'strain bé
            self.strainToRest = strain
        if fxR > self.fxRMax:
            self.fxRMax = fxR
            if self.renormalize:
                strain = strain - self.strainToRest
            return snap, t, strain, fxR
        else:
            return self.skipTrash(snap, t, strain, fxR)

    def skipTrash(self, snap, t, strain, fxR):
        firstPoint = strain
        secondPoint = strain
        while fxR < self.fxRMax:
            secondPoint = strain
            snap, t, strain, fxR = parse(self.fiter.next())
        self.strainToRest = self.strainToRest + (secondPoint - firstPoint)
        strain = strain - self.strainToRest
        return snap, t, strain, fxR

    def __iter__(self):
        return self

class ConsolidatedElasticFilterTalla(ConsolidatedElasticFilter):
    def __init__(self, filename, renormalize=True):
       super(ConsolidatedElasticFilterTalla, self).__init__(filename,
                                                            renormalize)
       self.countSeguretat = 10
       self.count = 0
    def skipTrash(self, snap, t, strain, fxR):
        if self.count < self.countSeguretat:
            self.count += 1
            return (snap, t, strain, fxR)
        else:            
            raise StopIteration
        
def mainDefault(directori):
    nomFitxer = os.path.join(directori, "stressm.dat")
    nomFitxerOut = os.path.join(directori, "consolidated.dat")
    fout = open(nomFitxerOut, 'w')
    for data in ConsolidatedElasticFilter(nomFitxer):
        fout.write("%d %f %f %f\n" % data)
    sys.stderr.write("escrivint %s\n" % nomFitxerOut)
    fout.close()

def nomesCreixent(data):
    stressos = [stress for snap, t, strain, stress in data]
    ymax = max(stressos)
    index = stressos.index(ymax)
    return [(snap,t,strain,stress) for i,(snap,t,strain,stress) in
            enumerate(data) if i <= index]    

def mainTalla(directori):
    nomFitxer = os.path.join(directori, "stressm.dat")
    nomFitxerOut = os.path.join(directori, "consoTallat.dat")
    fout = open(nomFitxerOut, 'w')
    data = [dada for dada in ConsolidatedElasticFilterTalla(nomFitxer)]
    data = nomesCreixent(data)
    for dada in data:
         fout.write("%d %f %f %f\n" % dada)
    sys.stderr.write("escrivint %s\n" % nomFitxerOut)
    fout.close()

def main(options, args):
    if options.talla: mainTalla(args[0])
    else: mainDefault(args[0])

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-t", "--talla", action="store_true")

    (options, args) = parser.parse_args()
    print args[0]
    main(options, args)
