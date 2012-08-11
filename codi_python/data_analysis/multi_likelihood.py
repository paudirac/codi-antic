#!/usr/bin/python
# -*- coding: UTF-8 -*-
import math,decimal
from likelihood import decimal_likelihood,PRECISION
from cartesia import cartesia

def likelihood_multidimensional_scaled_graph(fitting_function,
                                             empiric_data,
                                             params_ranges,
                                             n_points):
    index = 0
    total_as = []
    for params,n_as in zip(params_ranges,n_points):
        a_min,a_max = params
        delta_a = (a_max - a_min) / float(n_as)
        as = [a_min + float(i) * delta_a for i in range(n_as)]
        total_as.append(as)

    total_points = cartesia(total_as)

    likelihood_graph = [(a + (decimal_likelihood(fitting_function,
                                                 empiric_data,
                                                 a),))
                        for a in total_points]
    
    return scale_multidimensional_graph(likelihood_graph)

def scale_multidimensional_graph(dades):
    decimal.getcontext().prec = PRECISION
    ys = [data[-1] for data in dades]
    y_max = max(ys)
    return [(data[:-1] + (float(data[-1]/y_max),)) for data in dades]

## Na. S'ha de trobar el màxim del likelihood, que és on s'estimen tots
## els paràmetres. També s'ha d'invertir la informació de fisher,
## el log del likelihood, que és la matriu de covariança.

###################### Tests ###########################################
import random,Gnuplot,time
    
def wait(for_this_seconds=3): time.sleep(for_this_seconds)    
def generate_function(): return random.gauss(0.,2.)

g = Gnuplot.Gnuplot()

to_graphicate = []

def fitting_function(x, params):
    "pdf of the gaussina(mu,sigma)"
    mu, sigma = params
    sigma2 = sigma * sigma
    norm = 1. / math.sqrt(2 * math.pi * sigma2)
    return norm * math.exp(- (x - mu) * (x - mu) / (2 * sigma2))

def single_test_gaussian2d(number_of_points=20, to_print=False):
    empiric_data = [generate_function() for i in range(number_of_points)]
    g.plot(empiric_data)
    wait()

    print "Experimental points N = %d" % number_of_points
    a_min,a_max,n_as = -1.,1.,100
    b_min,b_max,n_bs = 1.,4.,100

    multi_likelihood_graph = \
      likelihood_multidimensional_scaled_graph(fitting_function,
                                               empiric_data,
                                               [(a_min,a_max),(b_min,b_max)],
                                               [n_as, n_bs])
    to_graphicate.append(multi_likelihood_graph)

    g('set pm3d')
    g('set hidden3d')
    g('set pm3d map')
    NX,NY = n_as, n_bs
    g('set dgrid3d %s,%s' % (NY,NX))
    
    g.splot(*to_graphicate)
    wait(10)

if __name__ == "__main__":
    single_test_gaussian2d()
