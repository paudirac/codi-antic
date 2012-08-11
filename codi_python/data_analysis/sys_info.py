#!/usr/bin/env python
# -*- coding: UTF-8 -*-

__all__ = []
class SysInfoError(Exception):
    pass

__all__.append("SysInfo")
class SysInfo(object):
    def __init__(self,line):
        try:
            line = line.split()
            self.NX = int(line[0])
            self.NY = int(line[1])
            self.phys_size = int(line[2])
            self.paret_size = int(line[3])
            self.sys_size = int(line[4])
            self.ny_extra = int(line[5])
            self.V = float(line[6])
            self.RATIO = float(line[7])
            self.random_seed = int(line[8])
            self.delta_x = float(line[9])
            self.delta_y = float(line[10])
            if len(line) == 12:
                self.SIGMA = float(line[11])
            else:
                self.SIGMA = 0
        except Exception, e:
            raise SysInfoError(e)

    def __str__(self):
        "Pretty printing of SysInfo, igual que en C."
        str_ =  "sys_info:   NX          = %d\n" % self.NX
        str_ += "            NY          = %d\n" % self.NY
        str_ += "            phys_size   = %d\n" % self.phys_size
        str_ += "            paret_size  = %d\n" % self.paret_size
        str_ += "            sys_size    = %d\n" % self.sys_size
        str_ += "            ny_extra    = %d\n" % self.ny_extra
        str_ += "            V           = %2.4f\n" % self.V
        str_ += "            RATIO (T)   = %2.4f\n" % self.RATIO
        str_ += "            random_seed = %d\n" % self.random_seed
        str_ += "            delta_x     = %2.4f\n" % self.delta_x
        str_ += "            delta_y     = %2.4f\n" % self.delta_y
        str_ += "            SIGMA       = %2.4f\n" % self.SIGMA
        return str_

    def one_liner(self):
        #novull = dir(object)
        #novull = novull + ['__dict__', '__module__', \
                 #'__weakref__', 'one_liner']
        #a = [str(getattr(self, prop)) for prop in dir(self) \
            #if prop not in novull]
        # Potser és més clar això d'aquí baix, no?
        a = []
        a.append(str(self.NX))
        a.append(str(self.NY))
        a.append(str(self.phys_size))
        a.append(str(self.paret_size))
        a.append(str(self.sys_size))
        a.append(str(self.ny_extra))
        a.append(str(self.V))
        a.append(str(self.RATIO))
        a.append(str(self.random_seed))
        a.append(str(self.delta_x))
        a.append(str(self.delta_y))
        a.append(str(self.SIGMA))
        return ' '.join(a)
