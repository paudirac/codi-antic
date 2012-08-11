#!/usr/bin/python
# -*- coding: UTF-8 -*-
# Author: Pau Cervera cervera at ffn dot es
# Date 23.08.2005
# Status: Funciona
"""
Programa que calcula la superfície del sòlid a partir de la intersecció
d'esferes centrades en les coordenades dels àtoms i la posició central
dels bins.
En teoria així no ha de dependre tant dels bins.

Utilització:
\t $ ./pysurface.py nom_fitxer_entrada n_bins

El format del fitxer d'entrada és el següent:
\t<temps> <oordenada x> <coordenada y> <tipus>
\t(tipus = 0 per als àtoms que volem tractar, altrament no se'ls té en
\tconsideració)
"""
import sys
import math

#Radi dels àtoms.
R = 1.122462048309373

class Handler:
    def first_line(self,line):
        pass

    def normal_line(self,line):
        pass

class Parser:
    def __init__(self, handler):
        self.handler = handler

    def parse(self, iname):
        try:
            ifile = open(iname,"r")
        except:
            print "El parser no pot obrir",iname
            sys.exit(1)

        try:
            lines = ifile.readlines()
        except:
            print "El parser no pot llegir",iname
            sys.exit(1)

        for line in lines:
            self.handler.normal_line(line)

class Punts(Handler):
    def __init__(self):
        self.punts = []
        self.bin = []
        self.y_max = -100000000
        self.y_min = 100000000
        self.t = None

    def get_t(self):
        """Retorna el temps de les dades."""
        return self.t
        
    def normal_line(self,line):
        """
        Escull dades (x,y) d'un fitxer de format:
        \t<temps> <coordenada x> <coordenada y> <tipus>
        Nomes agafa dades si tipus = 0. Altrament, no les agafa.
        """
        t,x,y,type = line.split()
        if self.t == None: self.t = float(t)
        if type == "0":
            x,y = float(x),float(y)
            punt = (x,y)
            self.punts.append(punt)
            if y > self.y_max : self.y_max = y
            if y < self.y_min : self.y_min = y
        else:
            pass

    def comparador_punts_x(self,p1,p2):
        x1,y1 = p1
        x2,y2 = p2
        return int(x1 - x2)

    def comparador_punts_y(self,p1,p2):
        x1,y1 = p1
        x2,y2 = p2
        return int(y1 - y2)

    def sup(self,n):
        sup = []
        self.punts.sort(self.comparador_punts_x)
        x_max = max(self.punts)[0]
        x_min = min(self.punts)[0]

        delta_x = (x_max + x_min)/float(n)

        for i in range(0,n):
            xL = x_min + i*delta_x
            xR = x_min + (i+1)*delta_x
            self.bin.append([p for p in self.punts \
                             if p[0] >= xL-R and p[0] <= xR + R])
            
        for i in range(0,n):
            x_mid = x_min + (i + 0.5)*delta_x
            interseccions = [intersect(p[0],p[1],R,x_mid) for p in self.bin[i]]
            interseccions = [inter for inter in interseccions if inter != None]
            interseccions = [inter[0] for inter in interseccions]
            interseccions.sort(self.comparador_punts_y)
            x,y = max(interseccions)
            sup.append((x,y))
        return sup


def intersect(x0,y0,R,x1):
    """
    Calcula les interseccions d'una esfera de radi R centrada
    al punt (x0,y0) i la recta x = x1.
    """
    delta = R * R - (x1 - x0)*(x1 - x0)
    if delta < 0: return None
    if delta == 0: return (x1,y0)
    if delta > 0:
        p1 = (x1, y0 + math.sqrt(delta))
        p2 = (x1, y0 - math.sqrt(delta))
        return (p1,p2)

def rug(superficie):
    """Calcula la \emph{interface width}:
    \tw = \sqrt{\ frac{1}{n} \sum_{i=1}^n (y - \ bar y)^2}.

    superficie és una llista de tuples (x,y), que són el perfil
    de la superfície.
    Retorna: (y_mean, w)"""
    n = float(len(superficie))
    y_mean = 0.
    for s in superficie:
        x,y = s
        y_mean = y_mean + y

    y_mean = y_mean/n
    w = 0.
    for s in superficie:
        x,y = s
        w = w + (y - y_mean)*(y - y_mean)
    w = w/n
    w = math.sqrt(w)
    return y_mean,w

def print_xy(llista,filename):
    file = open("surf_history.dat","a")
    lines = []
    lines.append("pystart " + filename + "\n")
    for l in llista:
        x,y = l
        lines.append("%.5e %.5e\n" % (x,y))
    lines.append("pyend\n")
    file.writelines(lines)
    file.close()
    
def main(ifilename,n):
    """Crea Punts i parseja el fitxer d'entrada.
    Calcula una superfície de n punts."""

    punts = Punts()
    parser = Parser(punts)
    parser.parse(ifilename)
    sup =  punts.sup(n)
    t = punts.get_t()

    file = open("surf_history.dat","a")
    lines = []
    lines.append("pystart rug.dat\n")
    lines.append("%.5e %.5e %.5e %d\n" % ((t,) + rug(sup) + (n,)))
    lines.append("pyend\n")
    file.writelines(lines)
    file.close()

    print_xy(sup,"sup_"+ str(n) + "_" + ifilename)

    
if "__main__" == __name__:
    ifilename = None
    n = 1
    try:
        ifilename = sys.argv[1]
    except:
        print __doc__
        sys.exit(1)

    try:
        n = sys.argv[2]
        n = int(n)
    except:
        print __doc__
        sys.exit(1)

    main(ifilename,n)
    sys.exit(0)
