#!/usr/bin/python
# -*- coding: UTF-8 -*-
"""
Arbitrary precision 1-dimensional likelihood framework.

Framework to estimate the parameters and the variance of the parameters
of a given fitting_function one dimensional probability density function,
by the method of the maximum likelihood.

The fitting_function has two arguments:
\tx: the experimental points slot
\tparams: the parameters which the likelihood function depends on.
\tFor instance, if we want to fit a gaussian with zero mean and
\tvariance sigma, which is the parameter we want to estimate,
\tthe fitting function definition will read:

\t    def fitting_function(x,params):
\t        sigma = params
\t        sigma2 = sigma * sigma
\t        norm = 1. / math.sqrt(2 * math.pi * sigma2)
\t        return norm * math.exp(- x * x / (2 * sigma2))


References:
  [1] Statistical Methods in Experimental Physics. W. T. Eadie et al.
      North Holland, 1971
"""
# Notes:
# Potser estaria bé fer-ho per defecte amb el log-likelihood.
# O s'han de fer tots els càlculs amb precisió arbitrària fins al final
# i només donar la opció de retornar un gaph graficable amb el gnuplot?

__author__ = "Pau Cervera i Badia"

import math,decimal

import datetime
__date__ = datetime.date.today()

__all__ = []
__all__.append("PRECISION")

PRECISION = 10

def Decimal(number): return decimal.Decimal(str(number))

def decimal_likelihood(fitting_function, empiric_data, params):
    decimal.getcontext().prec = PRECISION
    like = Decimal(1.)
    for x in empiric_data:
        like = like * Decimal(fitting_function(x, params))
    return like

def decimal_log_likelihood(fitting_function, empiric_data, params):
    decimal.getcontext().prec = PRECISION
    like = Decimal(0.)
    for x in empiric_data:
        like = like + Decimal(fitting_function(x,params))
    return like

def scale_graph(dades):
    decimal.getcontext().prec = PRECISION
    ys = [y for x,y in dades]
    y_max = max(ys)
    return [(x,float(y/y_max)) for x,y in dades]

__all__.append("likelihood_scaled_graph")
def likelihood_scaled_graph(fitting_function, empiric_data,
                            params_ranges, n_points=[0],
                            logarithm=False):
    """Returns a list containing n_point pairs (param, likelihood(param)),
    that are the graph of the likelihood function of the function
    fitting_function and the data empiric_data, scaled with the maximum
    likelihood; i.e. the maximum value is 1. The returned pairs are floats,
    but the computation has been performed with Decimals with precision
    PRECISION.

    For the form of fitting_function, see the module description.
    """
    if not logarithm:
        likelihood = decimal_likelihood
    else:
        likelihood = decimal_log_likelihood
        
    a_min, a_max = params_ranges[0]
    n_as = n_points[0]
    delta_a = (a_max - a_min) / float(n_as)
    as = [a_min + float(i) * delta_a for i in range(n_as)]

    likelihood_graph = [(a, decimal_likelihood(fitting_function,
                                               empiric_data,
                                               a)) for a in as]
    return scale_graph(likelihood_graph)

def roots(data):
    roots_ = []
    x_0,y_0 = data[0]
    for x,y in data[1:]:
        if y_0 * y < 0.:
            roots_.append(0.5 * (x + x_0))
        x_0,y_0 = x,y
    return roots_

def LikelihoodException(Exception): pass

__all__.append("estimates")
def estimates(likelihood_graph,logarithm=False):
    """Calculates the maximum likelihood estimate of params and
    its variance by means of the roots of the auxiliary function:
    aux(param) = ln(L(param)) - ln(Lmax) + 0.5, which crosses zero
    at 68.3% confidence level; see [1] for more information.
    
    If something fails, raises LikelihoodException.

    Returns: (estimate, variance)."""
    # O potser canviar la precisió?
    if not logarithm:
        try:
            aux = [(x, math.log(y) + 0.5) for x,y in likelihood_graph]
        except Exception, e:
            print e
            raise LikelihoodException("No puc calcular els logaritmes.")
    else:
        aux = [(x,y + 0.5) for x,y in likelihood_graph]
    
    roots_ = roots(aux)

    if len(roots_) > 2: raise LikelihoodException(
        "Abnormal number of roots = %d" % len(roots_))

    x_m,x_M = roots_
    estimate = 0.5 * (x_M + x_m)
    variance = 0.5 * (x_M - x_m)
    return (estimate, variance)


#################### Tests ################################
import time,Gnuplot,random
def wait(for_this_seconds=3):
    time.sleep(for_this_seconds)

g = Gnuplot.Gnuplot()
to_graphicate = []

def generate_function(): return random.gauss(0.,2.)

def fitting_function(x,params):
    sigma = params
    sigma2 = sigma * sigma
    norm = 1. / math.sqrt(2 * math.pi * sigma2)
    return norm * math.exp(- x * x / (2 * sigma2))

def single_test_gaussian(number_of_points=20,to_print=False):
    empiric_data = [generate_function() for i in range(number_of_points)]
    g.plot(empiric_data)

    print "Experimental points N = %d" % number_of_points

    a_min,a_max = 1.,4.
    n_as = 1000
    likelihood_graph = \
       likelihood_scaled_graph(fitting_function, empiric_data,
                                params_ranges = [(a_min,a_max)],
                                n_points=[n_as])

    to_graphicate.append(likelihood_graph)
    estimates_ = None, None
    try:
        estimates_ = estimates(likelihood_graph)
    except Exception, e:
        print "Error in the estimation.", e
        print "Recalculant amb log_likelihood directament"
        likelihood_graph = \
            likelihood_scaled_graph(fitting_function, empiric_data,
                                    params_ranges = [(a_min,a_max)],
                                    n_points=[n_as],
                                    logarithm=True)
        try:
            estimates_ = estimates(likelihood_graph, logarithm=True)
        except Exception, e:
            print "Error in the estimation.", e
            print "Fins i tot amb el log_likelihood"                         
    else:
        print "Estimates:", estimates_

    return estimates_

to_estimate = []
def test(number_of_points=20,to_print=False):
    to_estimate.append(single_test_gaussian())
    xm,ym = 0.,0.
    for x,y in to_estimate:
        xm = xm + x
        ym = ym + y
    xm = xm / float(len(to_estimate))
    ym = ym / float(len(to_estimate))
    print "amb mitjana", xm, ym

    if to_print:
        for a,like in likelihood_graph:
            print a,like

    g.plot(*to_graphicate)
    wait(5)

def multiple_test():
    for i in range(10):
        test()
    
if __name__ == "__main__":
    print "Prova test, a vuere si s'ajusten els punts"
    print "a la Gaussiana N(0.,1.)"

#    single_test_gaussian(number_of_points=1000,to_print=False)
#    multiple_test()
    test()
