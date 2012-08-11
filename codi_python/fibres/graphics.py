#!/usr/bin/python
# -*- coding: UTF-8 -*-

import pyx
from euclidean import Coords

__all__ = []

__all__.append("Punt")
class Punt(object):
    radius = 0.01
    def __init__(self, coords):
        self.color = pyx.color.rgb.black
        self.coords = coords
        self.radius = Punt.radius
    def shape(self):
        return pyx.path.circle(self.coords.x, self.coords.y, self.radius)
    def attributes(self):
        return [pyx.style.linewidth.THIN, self.color]
    def show(self, canvas):
        canvas.stroke(self.shape(), self.attributes())

__all__.append("Segment")
class Segment(object):
    def __init__(self, origin, dest):
        self.color = pyx.color.rgb.black
        if isinstance(origin, Punt):
            self.origin = origin.coords
        else:
            self.origin = origin
        if isinstance(dest, Punt):
            self.dest = dest.coords
        else:
            self.dest = dest
    def shape(self):
        origin = pyx.path.moveto(self.origin.x, self.origin.y)
        dest = pyx.path.moveto(self.dest.x, self.dest.y)
        line = pyx.path.lineto(self.dest.x, self.dest.y)
        #return pyx.path.path(origin, line, dest)
        return pyx.path.line(self.origin.x, self.origin.y,
                             self.dest.x, self.dest.y)
    def attributes(self):
        return [pyx.style.linewidth.THIN, self.color]
    def show(self, canvas):
        canvas.stroke(self.shape(), self.attributes())

# potser wrappejar aquí el que s'hagi de wrappejar per tal de tenir el pyx
# funcionant com hauria de funcionar. copón, ja podria anar bé, no?
