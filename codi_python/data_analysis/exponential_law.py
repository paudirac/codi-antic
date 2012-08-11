#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys,math,os,random
__all__ = []

__all__.append("parse_and_apply")
def parse_and_apply(arguments):
    directori,filename,y_min,bootstrap = arguments.split()
    filename = os.path.join(directori, filename)
    print >> sys.stderr, "calling main for", filename
    y_min = float(y_min)
    main(filename, y_min, bootstrap)

def get_data(filename):
    lines = open(filename,'r').readlines()
    data = [line.split() for line in lines]
    data = [float(line[4-1]) for line in data]
    return data

def lambda_normalitzacio(xs, x_min):
    def lambda_(xs, x_min):
        "Estimació de la lambda segons el max Likelihood."
        suma = 0.
        for x in xs:
            suma = suma + (x - x_min)
        n = float(len(xs))
        return n / suma

    lambda_ = lambda_(xs, x_min)
    normalitzacio = lambda_ / math.exp( - lambda_ * x_min)
    return lambda_, normalitzacio

def least_resamplings(xs):
    n = len(xs)
    logn = math.log(n)
    return n * logn * logn

def resample(xs, length=None):
    if length is None: length = len(xs)
    return [random.choice(xs) for i in range(length)]

def estadistica(bootstrap_data):
    b_exp, b_norm = 0., 0.
    norm_ = 1. / float(len(bootstrap_data))
    for exp, norm in bootstrap_data:
        b_exp = b_exp + norm_ * exp
        b_norm = b_norm + norm_ * norm

    b_sigma_exp, b_sigma_norm = 0., 0.
    for exp, norm in bootstrap_data:
        b_sigma_exp = b_sigma_exp + norm_ * (exp - b_exp) * (exp - b_exp)
        b_sigma_norm = b_sigma_norm + norm_ * (norm - b_norm) * (norm  - b_norm)
    b_sigma_exp = math.sqrt(b_sigma_exp)
    b_sigma_norm = math.sqrt(b_sigma_norm)

    return b_exp,b_sigma_exp,b_norm,b_sigma_norm        

__all__.append("main")
def main(filename, bootstrap=False, out=None):
    bootstrap_data = []
    data = get_data(filename)
    ys = [dada for dada in data if dada != 0.]
    y_min = min(ys)
    lambda_, normalitzacio = lambda_normalitzacio(ys, y_min)

    print y_min, lambda_, normalitzacio

    # guardo resultats
    class Results: pass
    results = Results()
    results.y_min = y_min
    results.lambda_ = lambda_
    results.normalitzacio = normalitzacio
    
    bootstrap_data.append((lambda_, normalitzacio))

    if not bootstrap: return
    
    resamplings = least_resamplings(ys)
    resamplings = len(ys)
    for i in range(resamplings):
        resample_ = resample(ys, len(ys))
        lamda_, normalitzacio = lambda_normalitzacio(resample_, y_min)
        bootstrap_data.append((lambda_, normalitzacio))
        if out is not None:
            print y_min, lambda_, normalitzacio
        print >> sys.stderr, "bootsraping #%d:%d [y_min: %f]" \
        % (i, resamplings - i, y_min), \
          "a set of %d elements" % len(ys)

    b_lambda_,b_sigma_lambda_,b_norm,b_sigma_norm = estadistica(bootstrap_data)
    print "bootstrap",
    print y_min, b_lambda_,b_sigma_lambda_,b_norm,b_sigma_norm

    results.b_lambda_ = b_lambda_
    results.b_sigma_lambda_ = b_sigma_lambda_
    results.b_norm = b_norm
    results.b_sigma_norm = b_sigma_norm
    r = results

    print
    print "En resum: els resultats de %s" % filename
    print "Magnitud estimacio estimacio_botstrap sigma"
    print "y_min:    %f" % r.y_min
    print "lambda_:      %f     %f     %f" % \
          (r.lambda_, r.b_lambda_, r.b_sigma_lambda_)
    print "norm:         %f     %f     %f" % \
          (r.normalitzacio, r.b_norm, r.b_sigma_norm)
    print

if __name__ == "__main__":
    filename_default = "bursts_all.dat"
    try:
        filename = sys.argv[1]
    except:
        filename = filename_default

    main(filename, bootstrap=True)
