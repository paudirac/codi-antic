#!/usr/bin/python
# -*- coding: UTF-8 -*-

# Fer que printi strain, també?
# Aniria bé, encara que pot fer-se fàcil manualment.

import os, sys, math, random
from sys_info import SysInfo
from file_managers import frames, parse_time

def parse_and_apply(directori):
    try:
        main(directori)
    except Exception, e: raise e

global particula

class default_out(object):
    def write(self, something): pass

class DS(object):
    # internal representation es amb una tuple de tuples,
    # del típus (t, ds)
    # s'utilitza com a:
    #   dsi = DS()
    #   dsi.append((t, posicions))
    # ...
    #   print dsi
    def __init__(self, particula):
        self.history = []
        self.pos_ant = None
        self.particula = particula

    def __getitem__(self, i):
        return self.history[i]

    def append(self, item):
        t, position = item
        if self.pos_ant is None:
            ds = 0.
        else:
            x,y = position
            x_ant, y_ant = self.pos_ant
            dx, dy = x - x_ant, y - y_ant
            ds = math.sqrt(dx * dx + dy * dy)
        self.history.append((t, ds))
        self.pos_ant = position

    def __str__(self):
        ds_acum = 0.
        #str_ = "# ds(t) de la particula %d\n" % self.particula
        str_ = ""
        for t, ds in self.history:
            ds_acum = ds_acum + ds
            str_ += "%e %e %e\n" % (t, ds, ds_acum)
        return str_

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

    def acumulated(self):
        ds_acum = 0.
        for t, ds in self.history:
            ds_acum = ds_acum + ds
        return ds_acum

    def __len__(self):
        return len(self.history)

    def normalize(self, ds_normal):
        for index, item in enumerate(self.history):
            t, ds = item
            tn, dsn = ds_normal[index]
            try:
                ds = ds / dsn
            except:
                pass
            self.history[index] = (t, ds)

class DSx(DS):
    def append(self, item):
        t, position = item
        x,y = position
        if self.pos_ant is None:
            ds = 0.
        else:
            x_ant = self.pos_ant
            dx = x - x_ant
            ds = math.sqrt(dx * dx)
        self.history.append((t, ds))
        self.pos_ant = x

class DSy(DS):
    def append(self, item):
        t, position = item
        x,y = position
        if self.pos_ant is None:
            ds = 0.
        else:
            y_ant = self.pos_ant
            dy = y - y_ant
            ds = math.sqrt(dy * dy)
        self.history.append((t, ds))
        self.pos_ant = y

def extract_ds(positions_snap, t, dsi_list):
    for index, position in enumerate(positions_snap):
        dsi_list[index].append((t, position))

def statistics_ds(dsi_list):
    ds_mean = DS(0)
    for i in range(len(dsi_list[0])):
        ds_mean.append((0.,(0., 0.)))
    norm = 1. / float(len(dsi_list))
    for dsi in dsi_list:
        for index, item in enumerate(dsi):
            t, ds = item
            tm,dsm = ds_mean[index]
            dsm = dsm + norm * ds
            ds_mean[index] = (t, dsm)
    return ds_mean

def extract_dss(dsi_list, count=0):
    for count in range(len(dsi_list[0])):
        yield _extract_dss(dsi_list, count)
# Això no xuta. Canviar approach.
def _extract_dss(dsi_list, count):
    dss_ = []
    for dsi in dsi_list:
        for index, item in enumerate(dsi):
            if index == count:
                t, ds = item
                dss_.append(ds)
                continue
    return index, t, dss_

def participation_ratio(dsi_list, ds_mean=None):
    if ds_mean is None:
        ds_mean = statistics_ds(dsi_list)
    for dsi in dsi_list:
        dsi.normalize(ds_mean)
    ds_part = DS(-1)
    for index, t, dss in extract_dss(dsi_list):
        tmean, dsmean = ds_mean[index]
        dss = [ds for ds in dss if ds > dsmean]
        print index, t, dss
        print index, len(dss)

def main(directori, fitxer="posicions", out=None):
    fitxer = os.path.join(directori, fitxer)
    sys_info = SysInfo(open(fitxer).readlines()[0])
    global particula
    nparticules = sys_info.NX * sys_info.NY
    if out is None:
        out = default_out()

    dsi_list = [DS(i) for i in range(nparticules)]

    for frame_info, posicions in frames(fitxer, nparticules):
        t = parse_time(frame_info)
        extract_ds(posicions, t, dsi_list)

    dsix_list = [DSx(i) for i in range(nparticules)]
    for frame_info, posicions in frames(fitxer, nparticules):
        t = parse_time(frame_info)
        extract_ds(posicions, t, dsix_list)

    dsiy_list = [DSy(i) for i in range(nparticules)]
    for frame_info, posicions in frames(fitxer, nparticules):
        t = parse_time(frame_info)
        extract_ds(posicions, t, dsiy_list)

    for index, dsi in enumerate(dsi_list):
        print >> out, dsi.acumulated()

    # es "legal" agafar una partícula at random?
    particula = random.choice(range(nparticules))
    dsi = dsi_list[particula]

    fitxer_out = os.path.join(directori, "random_particle_ds.dat")
    open(fitxer_out, "w").writelines(str(dsi))
    print >> sys.stderr, "Escrivint %s" % fitxer_out

    dsix = dsix_list[particula]
    fitxer_out = os.path.join(directori, "random_particle_dsx.dat")
    open(fitxer_out, "w").writelines(str(dsix))
    print >> sys.stderr, "Escrivint %s" % fitxer_out

    dsiy = dsiy_list[particula]
    fitxer_out = os.path.join(directori, "random_particle_dsy.dat")
    open(fitxer_out, "w").writelines(str(dsiy))
    print >> sys.stderr, "Escrivint %s" % fitxer_out

    #fitxer_out = open(fitxer_out, "w")
    #for dsi in dsi_list:
        #fitxer_out.writelines(str(dsi))
    #fitxer_out.close()
    #print >> sys.stderr, "Escrivint %s" % fitxer_out.__name__

    fitxer_out = "ds_mean.dat"
    fitxer_out = os.path.join(directori, fitxer_out)
    ds_mean = statistics_ds(dsi_list)
    open(fitxer_out, "w").writelines(str(ds_mean))
    print >> sys.stderr, "Escrivint %s" % fitxer_out

    participation_ratio(dsi_list)

if __name__ == "__main__":
   directori = sys.argv[1]
   main(directori)
