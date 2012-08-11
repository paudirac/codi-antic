#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import power_law_exponent as pwle

def get_data(filename):
    f = open(filename)
    data = [tuple(line.split()) for line in f.readlines()]
    data = [(float(e_strain), float(dy)) for s,e_strain,dx,dy in data]
    return data

pwle.get_data = get_data

def derivate(data):
    "La data ja està derivada!"
    return data

pwle.derivate = derivate

def main(filename, y_min=0., y_max=0., n=1.):
    delta_y = (y_max - y_min) / float(n)
    for i in range(n):
        x_min = y_min + float(i) * delta_y
        pwle.main(filename, x_min, bootstrap=False)
        print >> sys.stderr, "calculant... x_min: %f" % x_min

if __name__ == "__main__":
    filename_default = "smoothed_deriv_all.dat"
    y_min,y_max,n = 0., 0., 0
    try:
        filename = sys.argv[1]
        y_min = float(sys.argv[2])
        y_max = float(sys.argv[3])
        n = int(sys.argv[4])
    except:
        filename = filename_default

    main(filename, y_min, y_max, n)
