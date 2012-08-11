#!/usr/bin/python
# -*- coding: UTF-8 -*-

from itertools import izip

__all__ = []

class TwoLinesFile(file):
    def __init__(self, fitxer):
        super(TwoLinesFile, self).__init__(fitxer)
        super(TwoLinesFile, self).next()

    def next(self):
        return super(TwoLinesFile, self).next(), super(TwoLinesFile, self).next()

__all__.append("frames")
def frames(fitxer, nparticules):
    f = TwoLinesFile(fitxer)

    for frame_info, line in f:
        frame = [tuple(float(x) for x in chunk.split())
                    for chunk in line.split(":")[1:-1] ]
        yield frame_info, frame[:nparticules]

__all__.append("frame")
def frame(framenum, fitxer, nparticules):
    for frame_info, posicions in frames(fitxer, nparticules):
        snap = int(frame_info.split()[0])
        if snap == framenum:
            return frame_info, posicions

__all__.append("defectFrames")
def defectFrames(fitxer):
    f = TwoLinesFile(fitxer)

    for frame_info, line in f:
        frame = [chunk.split() for chunk in line.split(":")[1:-1]]
        dislos = [(float(x),float(y)) for pos, x, y in frame if pos == "D"]
        cincs = [(float(x),float(y)) for pos, x, y in frame if pos == "5"]
        sets = [(float(x),float(y)) for pos, x, y in frame if pos == "7"]
        walls = [(float(x),float(y)) for pos, x,y in frame if pos == "W"]
        yield frame_info, (walls, dislos, cincs, sets)

__all__.append("defectFrame")
def defectFrame(framenum, fitxer):
    for frame_info, laResta in defectFrames(fitxer):
        snap = int(frame_info.split()[0])
        if snap == framenum:
            return frame_info, laResta

__all__.append("parse_time")
def parse_time(linia):
    linia = linia.split()
    return float(linia[1])
