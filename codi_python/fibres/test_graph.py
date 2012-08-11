#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Testos per al graph.py

from graph import *
from numpy import *
from euclidean import Coords
from graphics import Punt, Segment
import pyx

def testAbstracte():
    N = 100
    resetVertexCounter()
    import random
    class Punt(Vertex): pass
    punts = [Punt() for i in range(N)]
    g = Graph()
    connect = 4 #len(punts)
    for punt in punts:
        g.addVertex(punt, random.sample(punts, random.randint(0, connect)))
    adj = g.adjacencyMatrix()
    #print adj
    #vaps = linalg.linalg.eigvals(adj)
    #for vap in vaps:
        #print >> sys.stderr, vap

    gg = GGraph(g)
    canvas = pyx.canvas.canvas()
    gg.show(canvas)
    page = pyx.document.page(canvas, paperformat=pyx.document.paperformat.A4,
                             fittosize=1)
    doc = pyx.document.document()
    doc.append(page)
    doc.writePSfile("gg")

def testEuclidi():
    N = 100
    resetVertexCounter()
    import random
    epunts = [Coords(random.uniform(0.,10.), random.uniform(0.,10.))
             for _ in range(N)]
    punts = [EVertex(Vertex(), punt) for punt in epunts]
    g = Graph()
    connect = 2
    for punt in punts:
        nebrs = random.sample(punts, random.randint(0, connect))
        g.addVertex(punt, nebrs)
    adj = g.adjacencyMatrix()
    #print adj
    #vaps = linalg.linalg.eigvals(adj)
    #for vap in vaps:
        #print >> sys.stderr, vap

    gg = GGraph(g)
    canvas = pyx.canvas.canvas()
    gg.show(canvas)
    page = pyx.document.page(canvas, paperformat=pyx.document.paperformat.A4,
                             fittosize=1)
    doc = pyx.document.document()
    doc.append(page)
    doc.writePSfile("gg")

def testAdjacency():
    a = array([[1,0,0],[1,0,1],[2,1,0]])
    a = array([[0,1,1,1,1],
               [1,0,0,0,0],
               [1,0,0,0,0],
               [1,0,0,0,0],
               [1,0,0,0,0]])
    a = array([[0,1,1,1,1],
               [1,0,1,0,0],
               [1,1,0,0,0],
               [1,0,0,0,0],
               [1,0,0,0,0]])
    a = array([[0,1,1,1,1],
               [1,0,1,0,0],
               [1,1,0,1,0],
               [1,0,1,0,1],
               [1,0,0,1,0]])
    a = array([[0,1,1,1,1],
               [1,0,1,0,0],
               [1,1,0,1,1],
               [1,0,1,0,1],
               [1,0,1,1,0]])
    g = DiGraph()
    g.fromAdjacencyMatrix(a)
    print "graph size: %d" % g.size
    canvas = pyx.canvas.canvas()
    gg = GGraph(g)
    gg.show(canvas)

    page = pyx.document.page(canvas, paperformat=pyx.document.paperformat.A4,
                             fittosize=1)
    doc = pyx.document.document()
    doc.append(page)

    doc.writePSfile("gg")

def testPath():
    a = array([[0,1,1,1,1],
               [1,0,1,0,0],
               [1,1,0,1,1],
               [1,0,1,0,1],
               [1,0,1,1,0]])
    a = array([[0,1,0,0,0,0],
               [1,0,1,0,0,0],
               [0,1,0,1,1,0],
               [0,0,1,0,0,1],
               [0,0,1,0,0,1],
               [0,0,0,1,1,0]])
    g = DiGraph()
    g.fromAdjacencyMatrix(a)
    keys = g.E.keys()
    start = keys[2]
    end = keys[3]
    path = find_path(g,start,end)
    print "path de %s a %s" % (start, end)
    print "\t%s" % " ".join([str(vertex) for vertex in path])
    print
    paths = find_all_paths(g, start, end)
    print "all paths de %s a %s" % (start, end)
    for path in paths:
        print "\t%s" % " ".join([str(vertex) for vertex in path])
    print
    shortest = find_shortest_path(g, start, end)
    print "shortest path de %s a %s" % (start, end)
    print "\t%s" % " ".join([str(vertex) for vertex in shortest])
    print


    canvas = pyx.canvas.canvas()
    gg = GGraph(g)
    gg.show(canvas)
    
    page = pyx.document.page(canvas, paperformat=pyx.document.paperformat.A4,
                             fittosize=1)
    doc = pyx.document.document()
    doc.append(page)

    doc.writePSfile("gg")

if __name__ == "__main__":
#testAbstracte()
#testEuclidi()
#testAdjacency()
    testPath()
