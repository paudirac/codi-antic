#!/usr/bin/python
# -*- coding: UTF-8 -*-

import time
import math, random, time, sys
import pyx
from numpy import *

from euclidean import Coords
from graph import Graph, GGraph, EVertex, Vertex, DiGraph, EmptyGraphException

def printFibersPS(canvas, name, doc):
        page = pyx.document.page(canvas,
                paperformat=pyx.document.paperformat.A4, fittosize=1)
        doc.append(page)
        log("writting %s.ps...." % name)

global lenFiber
global lenFiberH
global width
global widthH
global density # density = N lenFiber / totalArea
global totalArea
global n
global rng

def log(sentence, logFile=sys.stderr):
    logFile.write(sentence + "\n")

def init(dens=1., widthFactor=4.):
    # Using lenFiber = 1. by definition
    # Parameters: density, width (in units of fiber length)
    # calculated parameters: n
    global lenFiber, lenFiberH, width, widthH, n, rng, density
    density = dens
    lenFiber = 1.
    lenFiberH = 0.5 * lenFiber
    width = widthFactor * lenFiber
    widthH = 0.5 * width
    totalArea = width * width
    n = int(density * totalArea / lenFiber)
    rng = random
    log("n: %d" % n)
    log("density: %f" % density)
    log("size: %.2fx%.2f" % (width, width))
#open("angles.dat","w")

def rand_centers():
    minim = -widthH + lenFiberH
    maxim = widthH - lenFiberH
    return [(rng.uniform(minim, maxim),
             rng.uniform(minim, maxim)) for i in range(n)]

def rand_angles():
    return [rng.uniform(0., 2. * math.pi) for i in range(n)]

class Line(list):
    _id = 0
    def __init__(self, args):
        super(Line, self).__init__(args)
        self.id = Line._id
        Line._id = Line._id + 1
        self.crosses = {}
    def _line(self):
        return pyx.path.line(self[0][0], self[0][1], self[-1][0], self[-1][1])
    def show(self, canvas):
        canvas.stroke(self._line(), [pyx.style.linewidth.THIN])
    def addCrossLink(self, cross, param):
        self.crosses[param] = cross
    def fillAdjacency(self, adjacencyMatrx):
        lengths = self.crosses.keys()
        lengths.sort()
        for i in range(len(lengths) - 1):
            c1 = self.crosses[lengths[i]]
            c2 = self.crosses[lengths[i+1]]
            adjacencyMatrx[c1.id][c2.id] = 1
            adjacencyMatrx[c2.id][c1.id] = 1
    def fillGraph(self, g):
        lengths = self.crosses.keys()
        lengths.sort()
        for i in range(len(lengths) - 1):
            c1 = self.crosses[lengths[i]]
            c2 = self.crosses[lengths[i + 1]]
            g.addVertex(c1, [c2])
            g.addVertex(c2, [c1])

class CrossLink(EVertex):
    def __init__(self, arg):
        x,y = arg
        super(CrossLink, self).__init__(Vertex(), Coords(x,y))
        self.coordNumber = 0

def puntRecta(gamma, center, alpha):
    x0,y0 = center
    return (gamma * math.cos(alpha) + x0,
            gamma * math.sin(alpha) + y0)

def pointsLine(center, alpha):
    return (puntRecta(-lenFiberH, center, alpha),
            center,
            puntRecta(lenFiberH, center, alpha))

def liniaRecta(center, alpha):
    return Line(pointsLine(center, alpha))

global Angles
Angles = 0.
global count
count = 0

def crossing(recta1, recta2):
    global Angles, count
    p1i,p1c,p1f = recta1
    p2i,p2c,p2f = recta2
    
    x1c,y1c = p1c
    x2c,y2c = p2c
    dist_centres2 = (x1c - x2c) * (x1c - x2c) + (y1c - y2c) * (y1c - y2c)
    rrMax = lenFiber * lenFiber
    if dist_centres2 > rrMax: return None # segur que no es toquen
    
    x1i,y1i = p1i
    x2i,y2i = p2i
    
    x1f,y1f = p1f
    x2f,y2f = p2f

    v1x, v1y = x1f - x1i, y1f - y1i
    v2x, v2y = x2f - x2i, y2f - y2i

    det = v1y * v2x - v1x * v2y

    if det == 0.: return None # segur que no es toquen
    else:
        param1 = ((y2i - y1i) * v2x - (x2i - x1i) * v2y) / det
        param2 = ((y2i - y1i) * v1x - (x2i - x1i) * v1y) / det

        if param1 < 0.: return None
        if param2 < 0.: return None
        if param1 > 1.: return None
        if param2 > 1.: return None

        x1 = param1 * v1x + x1i
        y1 = param1 * v1y + y1i

        rr1 = (x1 - x1i) * (x1 - x1i) + (y1 - y1i) * (y1 - y1i)
        
        rrMax = lenFiber * lenFiber

        if rr1 > rrMax: return None # esta fora de la firbra1

        x2 = param2 * v2x + x2i
        y2 = param2 * v2y + y2i

        rr2 = (x2 - x2i) * (x2 - x2i) + (y2 - y2i) * (y2 - y2i)

        if rr2 > rrMax: return None # esta fora de la fibra2

        crossLink = CrossLink((x1, y1))
        recta1.addCrossLink(crossLink, param1)
        recta2.addCrossLink(crossLink, param2)

        dotProduct = v1x * v2x + v1y * v2y
        v1 = math.sqrt (v1x * v1x + v1y * v1y)
        v2 = math.sqrt (v2x * v2x + v2y * v2y)
        angle = math.acos(dotProduct / (v1 * v2))
        Angles = Angles + angle
        count = count + 1
#open("angles.dat","a").write("%f\n" % angle)

        return crossLink 

def computeAllCrossings(rectes):
    nRectes = len(rectes)
    crossings_ = [crossing(rectes[i], rectes[j]) for i in range(nRectes) \
                    for j in range(i + 1, nRectes)]
    return [cross for cross in crossings_ if cross is not None]

def generateFibers():
    centers = rand_centers ()
    angles = rand_angles ()
    rectes = [liniaRecta(centre, alpha) for centre, alpha in 
                zip(centers, angles)]
    log("Hi ha %d rectes" % len(rectes))
    return rectes

def showFibers(rectes, canvas):
    if len(rectes) > 0:
        log("dibuixant fibers...")
        for rect in rectes: rect.show(canvas)

def computeCrossings(fibers):
    log("computant crossings...")
    crossings = computeAllCrossings(fibers)
    nCrossings = len(crossings)
    log("Hi ha %d talls" % nCrossings)
    log("%d %f %f %d" % (n, density, width, nCrossings), logFile=sys.stdout)
    log("%d %f %f %d" % (n, density, width, nCrossings))
    return crossings

def main(dens=1., widthFactor=1.):
    doc = pyx.document.document()
    canvas = pyx.canvas.canvas()
    init (dens=dens, widthFactor=widthFactor)
    rectes = generateFibers()
    crossings = computeCrossings(rectes)
    showFibers(rectes,canvas)
    printFibersPS(canvas, "graphFibers", doc)
    log("computing graph...")
    g = Graph()
    for rect in rectes: rect.fillGraph(g)
    log("g.size: %d" % g.size)
    log("computing ggraph...")
    gg = GGraph(g)
    canvas2 = pyx.canvas.canvas()
    try:
        gg.show(canvas2)
    except EmptyGraphException, e:
        sys.stderr.write("[Exception]: Surto perquè el graph no té cap vèrtex.\n")
        return
    printFibersPS(canvas2, "graphFibers", doc)
    log("writting gaphFibres.ps...")
    doc.writePSfile("graphFibres")

def loopDensities():
#densities = [2. + i * 0.5 for i in range(37)]
    densities = [2. + i * 5 for i in range(3)]
    print "n density width crossing crossings/2"
    for dens_ in densities:
        main (dens=dens_ , widthFactor=10.)

if __name__ == "__main__":
    loopDensities()
    #main (dens=2., widthFactor=10.)
