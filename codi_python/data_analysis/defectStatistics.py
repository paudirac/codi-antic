#!/usr/bin/python
# -*- coding: UTF-8 -*-

import pyx
import os,sys
from file_managers import defectFrames
from directory_apply_exceptions import AbortDirectoryApply

nom_fitxer_defectes = "historia_defectes_n"

# Parse and apply stuff

def parse_and_apply(algo):
    try:
        directori = algo
        mainPersistent(directori, nom_fitxer_defectes)
    except Exception, e: raise e

# parse and apply stuf /

class Dislo(tuple):
    import math
    _radius = 0.5 * math.pow(2., 1./6.) #.2
    _color = pyx.color.rgb.red
    def __init__(self, args):
        super(Dislo, self).__init__(args)
        self.color = Dislo._color
    def _graph(self):
        return pyx.path.circle(self[0], self[1], Dislo._radius)
    def showIn(self, canvas):
        canvas.stroke(self._graph(), [pyx.style.linewidth.THIN,
                                      self.color])

def recupera(fitxerPersistent):
    import pickle
    persistFile = None
    try:
        persistFile = open(fitxerPersistent,'r')
    except:
        firstNucl = []
    if persistFile is not None:
        try:
            unpickler = pickle.Unpickler(persistFile)
            firstNucl = unpickler.load()
            persistFile.close()
        except Exception, e:
            raise e
    return firstNucl

def mainPersistent(directori, fitxer=None):
    if fitxer is None:
        fitxer = nom_fitxer_defectes
    fitxer = os.path.join(directori, fitxer)

    fitxerPersistent = os.path.join(directori, "./../firstNucleationsPersist")
    fitxerInfo = os.path.join(directori, "./../firstNucleations.Info")
    try:
        firstNucl = recupera(fitxerPersistent)
    except Exception, e:
        raise AbortDirectoryApply("fitxer: %s\nexception: %s" % (fitxer, e))
    
    firstTime = True # False
    for frame_info, (walls, dislos, cincs, sets) in defectFrames(fitxer):
        if len(firstNucl) == 0:
            for part in walls:
                firstNucl.append(part)
        if len(dislos) > 0:
            if firstTime:
                for dislo in dislos:
                    firstNucl.append(dislo)
                break
            else: firstTime = True

    open(fitxerInfo,"a").write("%s %s" % (directori, frame_info))
    
    import pickle
    try:
        sys.stdout.write("guardo %s %d\n" % (directori, len(firstNucl)))
        persistFile = open(fitxerPersistent, "w")
        pickler = pickle.Pickler(persistFile)
        pickler.dump(firstNucl)
        persistFile.close()
    except Exception, e:
        sys.stderr.write("No puc guardar info a %s\n" % fitxerPersistent)
        sys.stderr.write(e)
        raise e

def main(directori, fitxer=None):
    pyx.unit.set(defaultunit="cm", uscale=0.4)
    if fitxer is None:
        fitxer = nom_fitxer_defectes
    fitxer = os.path.join(directori, fitxer)

    foto = 0
    yet = False
    for frame_info, (walls, dislos, cincs, sets) in defectFrames(fitxer):
        if not yet:
            if len(dislos) > 0:
                yet = True
        if foto % 5 == 0 and yet:
            sys.stderr.write("processant dislo%04d.eps\n" % foto)
            canvas = pyx.canvas.canvas()
            #for dislo in dislos:
                #gdislo = Dislo(dislo)
                #gdislo.color = pyx.color.rgb.blue
                #gdislo.showIn(canvas)
            for part in walls:
                gpart = Dislo(part)
                gpart.color = pyx.color.rgb.black
                gpart.showIn(canvas)
            for part in cincs:
                gpart = Dislo(part)
                gpart.color = pyx.color.rgb.red
                gpart.showIn(canvas)
            for part in sets:
                gpart = Dislo(part)
                gpart.color = pyx.color.rgb.green
                gpart.showIn(canvas)

            canvas.writeEPSfile("dislo%04d" % foto)
        
        foto = foto + 1

    sys.exit(0)

def graphicsMain(directori, fitxer):
    fitxer = os.path.join(directori, fitxer)
    firstNucl = recupera(fitxer)
    canvas = pyx.canvas.canvas()
    for part in firstNucl:
        gpart = Dislo(part)
        gpart.showIn(canvas)
    canvas.writeEPSfile("firstNucleations")
    sys.stdout.write("renderitzant %s a %s\n" \
            % (fitxer, "firstNucleations.eps"))
    sys.exit(0)

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-g", "--graphics", action="store_true")

    (options, args) = parser.parse_args()
    if options.graphics:
        graphicsMain(*args)
    else:                          # no esta ben arreclat
        directori = None
        fitxer = None
        try:
            directori = args[1]
        except: pass
        try:
            fitxer = args[2]
        except: pass

        if directori is None: directori = "./"
        main (directori, fitxer)
