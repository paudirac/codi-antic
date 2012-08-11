#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os,sys,random, math
from sys_info import SysInfo
from file_managers import frames, parse_time

nom_fitxer_posicions = "posicions"

def print_trajectories(frame_info, trajectories, out=None):
    if out is None:
        out = sys.stdout
    traj = tuple(x for y in trajectories for x in y)
    print "%s" % frame_info[:-1], "%f %f " * len(trajectories) % traj

class Trajectoria(object):
    # internal representation: (t, x, y)
    def __init__(self, particula):
        self.history = []
        self.particula = particula

    def append(self, item):
        t, position = item
        x,y = position
        self.history.append((t,x,y))

    def __str__(self):
        str_ = ""
        for punt in self:
            str_ += "%e %e %e\n" % punt
        return str_

    def __len__(self):
        return len(self.history)
    def __getitem__(self, index):
        try:
            return self.history[index]
        except Exception, e:
            raise e
    def __setitem__(self, index, item):
        try:
            self.history[index] = item
        except Exception, e:
            raise e

def diferences(parella):
    dif = []
    for punt1, punt2 in zip(*parella):
        t1,x1,y1 = punt1
        t2,x2,y2 = punt2
        if t1 != t2:
            print "Eo!, passa algo"
        dx = x2 - x1
        dy = y2 - y1
        dr = math.sqrt(dx * dx + dy * dy)
        dif.append((t1,dr))
    return dif

def extract_trajectories(positions, t, traj_list):
    for index, position in enumerate(positions):
        traj_list[index].append((t, position))

def main(directori, n_trajectories):
    nom_fitxer = os.path.join(directori,nom_fitxer_posicions)
    counter = 0
    title = ""
    sys_info = SysInfo(open(nom_fitxer).readlines()[0])
    #print sys_info
    NX = sys_info.NX
    V = sys_info.V
    delta_x = sys_info.delta_x
    NY = sys_info.NY

    #elegidosi = [random.choice(range(NX * NY)) for i in range(n_trajectories)]
    #for frame_info, posicions in frames(nom_fitxer, NX * NY):
        #elegidos = [posicions[index] for index in elegidosi]
        #print_trajectories(frame_info, posicions)#elegidos)

    traj_list = [Trajectoria(i) for i in range(NX * NY)]
    for frame_info, posicions in frames(nom_fitxer, NX * NY):
        t = parse_time(frame_info)
        extract_trajectories(posicions, t, traj_list)

    #for traj in traj_list:
        #print traj

    elegidi = [random.choice(range(NX * NY)) for i in range(2)]
    elegidos = [traj_list[i] for i in elegidi]
    dif = diferences(elegidos)
    for t, dr in dif:
        print t, dr
    
if __name__ == "__main__":
    directori = None
    try:
        directori = sys.argv[1]
    except: pass

    n_trajectories = 10
    try:
        n_trajectories = int(sys.argv[2])
    except: pass

    if directori is None: directori = "./"
    main (directori, n_trajectories)
