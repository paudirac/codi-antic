#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Un Graph té punts abstractes
# Un GGraph té un graph i Punts i Segments, que tria amb l'algoritme del cercle
# i els construeix a partir dels Vertex i els Edges

# Euclidean Graph
# Un EGraph té EVertex(Vertex, Punt)
# Un EGraph té Segments que assigna amb algun algoritme que treu dels EVertex

# Amb l'Euclidean Graph, podré calcular els elements de matriu (o els weights)
# dels edges, passant una funció a l'euclidean graph o així.
# I que ell em retorni una matriu.

from numpy import *
import math
import pyx
import sys

from euclidean import Coords
from graphics import Punt, Segment

__all__ = []

__all__.append("Vertex")
class Vertex(object):
    id = 0
    def __init__(self):
        self.id = Vertex.id
        Vertex.id += 1
    def __str__(self): return "Vertex: %d " % self.id

__all__.append("resetVertexCounter")
def resetVertexCounter(n=0): Vertex.id = 0
resetVertexCounter() # m'asseguro que està a zero al cridar el módul

__all__.append("EVertex")
class EVertex(object):
    def __init__(self, vertex, coord):
        self.id = vertex.id
        self.x, self.y = coord.x, coord.y

colorMappings = {
    0 : pyx.color.rgb.black,
    1 : pyx.color.rgb.black,
    2 : pyx.color.rgb.green,
    3 : pyx.color.rgb.blue,
    4 : pyx.color.rgb.red,
}

__all__.append("VertexError")
class VertexError(Exception): pass

def removeDuplicates(llista):
    lst = []
    if llista is None: return None
    for el in llista:
        if el not in lst:
            lst.append(el)
    return lst

#def removeDuplicates(llista): return llista

# crec que tot el codi de remove duplicates està de més, ja que ara
# l'adjacency matrix sí que està ben calculada

__all__.append("DiGraph")
class DiGraph(object):
    def __init__(self):
        self.V = []
        self.E = {}
        self.size = 0

    def addVertex(self, vertex, nebrs):
        if vertex not in self.V: # vertex ja en el graph. multigraphs no?
            self.V.append(vertex)
            vertex.id = self.size # per poder indexar l'adjacency
            self.size += 1
        self.addNeighbours(vertex, nebrs)

    def addNeighbours(self, vertex, nebrs):
        if vertex in nebrs: # no permetre loops
            del nebrs[nebrs.index(vertex)]
        if not self.E.has_key(vertex):
            self.E[vertex] = []
        tempNebrs = self.E[vertex]
        tempNebrs.extend(nebrs)
        self.E[vertex] = removeDuplicates(tempNebrs)

    def adjacencyMatrix(self):
        a = zeros((self.size, self.size), 'int')
        for vertex, nebrs in self.E.iteritems():
            for nebr in nebrs:
                assert (self.size > vertex.id)
                assert (self.size > nebr.id)
                a[vertex.id][nebr.id] = 1
        return a

    def fillMatrix(self, fn):
        a = zeros((self.size, self.size), 'float')
        for vertex, nebrs in self.E.iteritems():
            for nebr in nebrs:
                a[vertex.id][nebr.id] += fn(vertex, nebr)
            a[vertex.id][vertex.id] += fn(vertex, vertex)
        return a

    def fromAdjacencyMatrix(self, adj):
        class Punt(Vertex): pass
        ni,nj = adj.shape
        punts = [Punt() for punt in range(ni)]
        for i in range(ni):
            pi = punts[i]
            nebrs = [punts[j] for j in range(nj) if adj[i][j] != 0]
            self.addVertex(pi, nebrs)

    def fromAdjacencyAndCoordinates(self, adj, coords):
        ni,nj = adj.shape
        punts = [EVertex(Vertex(), coord) for coord in coords]
        for i in range(ni):
            pi = punts[i]
            nebrs = [punts[j] for j in range(nj) if adj[i][j] != 0]
            self.addVertex(pi, nebrs)

    #def laplacianMatrix(self)            ?
    #def degreeVector(self)               ?

__all__.append("Graph")
class Graph(DiGraph):
    def addVertex(self, vertex, nebrs):
        super(Graph, self).addVertex(vertex, nebrs)
        for nebr in nebrs:
            super(Graph, self).addVertex(nebr, [vertex])

__all__.append("EmptyGraphException")
class EmptyGraphException(Exception): pass

__all__.append("GGraph")
class GGraph(object):
    "Garaphical wrapper for a graph."
    def __init__(self, graph):
        self.g = graph
        self.showables = {}
        self._drawVertexSet()
        self._drawEdgeSet()

    def _drawVertexSet(self):
        if self.g.size == 0: return
        deltaAlpha = 2 * math.pi / self.g.size
        R = 2 * Punt.radius * self.g.size / math.pi
        for i, vertex in enumerate(self.g.V):
            try:
                coords = Coords(vertex.x, vertex.y)
            except Exception:
                coords = Coords(R * math.cos(i * deltaAlpha),
                                R * math.sin(i * deltaAlpha))
            punt = Punt(coords)
            ncoord = len(self.g.E[vertex])
            if ncoord > 4: ncoord = 4
            punt.color = colorMappings[ncoord]
            self.showables[vertex] = punt

    def _drawEdgeSet(self):
        if self.g.size == 0: return
        for vertex, nebrs in self.g.E.iteritems():
            origin = self.showables[vertex]
            for nebr in nebrs:
                dest = self.showables[nebr]
                self.showables[(vertex, nebr)] = Segment(origin, dest)

    def show(self, canvas):
        if self.g.size == 0: raise EmptyGraphException()
        for s in self.showables.itervalues(): s.show(canvas)

__all__.append("find_path")
def find_path(graph, start, end, path=[]):
    path = path + [start]
    if start == end:
        return path
    if not graph.E.has_key(start):
        return None
    for node in graph.E[start]:
        if node not in path:
            newpath = find_path(graph, node, end, path)
            if newpath: return newpath
    return None

__all__.append("find_all_paths")
def find_all_paths(graph, start, end, path=[]):
    path = path + [start]
    if start == end:
        return [path]
    if not graph.E.has_key(start):
        return []
    paths = []
    for node in graph.E[start]:
        if node not in path:
            newpaths = find_all_paths(graph, node, end, path)
            for newpath in newpaths:
                paths.append(newpath)
    return paths

__all__.append("find_shortest_path")
def find_shortest_path(graph, start, end, path=[]):
    path = path + [start]
    if start == end:
        return path
    if not graph.E.has_key(start):
        return None
    shortest = None
    for node in graph.E[start]:
        if node not in path:
            newpath = find_shortest_path(graph, node, end, path)
            if newpath:
                if not shortest or len(newpath) < len(shortest): # weight?
                    shortest = newpath
    return shortest
