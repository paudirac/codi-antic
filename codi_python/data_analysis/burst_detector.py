#!/usr/bin/env python
# -*- coding: UTF-8 -*-
"""
Framework for the detection of 'bursts' in a signal.
The bursts are detected by a finite state machine called BurstDetector,
which provides the interface:

   * __init__(w_data, threshold): constructor. It accepts w_data as the
   wrapped data (see below) and threshold, a keyword for the burst threshold.
   
   * set_threshold(threshold): method to change/set the threshold.

   * bursts: method that returns a Bursts object with the calculated bursts.

There are a few helper classes, that allow for wrapping the original data
and the data returned by bursts.

   * Realitzacio: is an abstract base class which wrapps the data.
   Any w_data has to be a Realitzacio and provide at leas two methods:

       ** __getitem__ should work as in a list a, a[item_index]--> item
       ** __len__ should work as in a list a, len(a) --> length of list a

   they are used by BurstDetector while calculating the bursts from the
   data wrapped.

   * Bursts: is the class that encapsulates burst data.

       ** __str__ default method which allows to print the data in a
       human/file readable way:
       burst_number initial_index final_index
       ...
       
   It contains a data field that contains the initial w_data (Realitzacio),
   for subsequent data analysis.

   * BurstIntegrator: is the default burst_area integrator. It uses the
   Bursts class.

       ** __str__ default method which allows to print the data in a
       human/file readable way:
       burst_number initial_index final_index burst_area
       ...

All classes but BurstDetector can be oberriden, provided that the necessary
methods are implemented (or inherited).
"""

__autor__ = "pau dot cervera at gmail dot com"

class BurstException(Exception):
    "Base Class for any BurstException"
    pass

class Realitzacio(object):
    """Abstract interface for realització, which is passed to the
    constructor of BurstDetector."""
    def __init__(self, nom_fitxer):
        self.source = nom_fitxer
    def __getitem__(self, key):
        return (key, self.data[key])
    def __len__(self):
        return len(self.data)
    def __str__(self):
        return "Data from %s\n" % self.source

class Bursts(object):
    """Class that encapsulates the burst information of a given signal
    and the signal (which is previously encapsulated in a Realitzacio
    object.
    It is used by BurstIntegrator, for instance.
    """
    def __init__(self, bursts, data):
        """Recives the bursts as a list of lists of initial and end points
        indices of the bursts:
        [[burst_0_init, burst_0_end],[burst_1_init, burst_1_end],...]
        where burst_i_init is the index of start of the burst i and
        where burst_i_end is the index of the start of the burst i."""
        self._burst_list = bursts
        self.data = data

    def __str__(self):
        str_ = "# burst\tinit\tfin\n"
        for index in range(len(self._burst_list)):
            init,fin = self._burst_list[index]
            str_ += "  %d\t%s\t%s\n" % (index, str(init), str(fin))
        return str_
    
class BurstDetector(object):
    """Finite state machine which detects bursts based on a signal
    and a given threshold.

    The machine has two states NOBURST and BURST, and changes from
    one to the other if the threshold is surpassed.

    The signal must implement Realitzacio for BurstDetector to work.
    
    BurstDetector saves the index at which the burst appears and the
    index at which the burst disappears (according to the threshold).
    That information and the data from Realitzacio is given back to
    the caller of BurstDetector.bursts, encapsulated in a Bursts object.
    """
    NOBURST = 0
    BURST = 1
    
    def __init__(self, w_data, threshold=None):
        self.data = w_data
        self.threshold = threshold
        self._burst_list = []
        self.current = None

    def set_threshold(self, threshold):
        self.threshold = threshold

    def bursts(self):
        if self.threshold is None:
            raise BurstException, "None threshold"
        else:
            return self._bursts()

    def _bursts(self):
        state = BurstDetector.NOBURST
        #relies in __getitem__ method in self.data
        for point in self.data:
            state = self._dispatch_state(point, state)
        return Bursts(self._burst_list, self.data)

    def _dispatch_state(self, point, state):
        index, value = point
        if state == BurstDetector.NOBURST:
            if value < self.threshold: # abans estava en abs(value)!!!
                return BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                return BurstDetector.BURST
        elif state == BurstDetector.BURST:
            if value < self.threshold: # abans estava en abs(value)!!!
                self._append_burst()
                return BurstDetector.NOBURST
            else:
                self._add_to_current(point)
                return BurstDetector.BURST

    def _add_to_current(self, point):
        index, value = point
        if self.current is None:
            self.current = [index, index]
        else:
            self.current[1] = index

    def _append_burst(self):
        self._burst_list.append(self.current)
        self.current = None

class BurstIntegrator(object):
    """Default BurstIntegrator. It performs the integration of the
    bursts detected on a given signal.
    That signal is wrapped in a Realitzacio object which is given to
    BurstDetector. BurstIntegrator uses the Bursts object returned
    by BurstDetector, which encapsulates the bursts and the signal.
    """
    def __init__(self, bursts):
        self._burst_list = bursts._burst_list
        self.data = bursts.data
        self.integral = []
        self._integrate()

    def _integrate(self):
        self._default_integral()

    def _default_integral(self):
        for init,fin in self._burst_list:
            integral = 0.
            for i in range(init, fin):
                index, value = self.data[i]
                integral = integral + value # pero aixi no esta normalitzat!
            self.integral.append(integral)

    def integrate_with_x_from(self, column=1):
        del self.integral
        self.integral = []
        
        def x(init, fin): # les x son a la columna column, molt ineficient
            xinit, xfin = 0., 0.
            for line in open(self.data.source):
                line = map(float, line.split())
                index = int(line[0])
                if index == init: xinit = line[column -1]
                if index == fin:
                    xfin = line[column - 1]
                    break
            return xinit, xfin

        for init, fin in self._burst_list:
            integral = 0.
            for i in range(init, fin):
                index, value = self.data[i]
                x0,x1 = x(init, i)
                integral = integral + value * (x1 - x0)
            self.integral.append(integral)

    def __str__(self):
        str_ = "# burst\tinit\tfin\tsize\n"
        for index in range(len(self._burst_list)):
            init,fin = self._burst_list[index]
            size = self.integral[index]
            str_ += "  %d\t%s\t%s\t%.5e\n" % (index, str(init), str(fin), size)
        return str_


class BurstMaxims(BurstIntegrator):
    def __init__(self, bursts):
        self._burst_list = bursts._burst_list
        self.data = bursts.data
        self.integral = []
        for init, fin in self._burst_list:
            values = [self.data[i][1] for i in range(init, fin)]
            if len(values) > 0:
                self.integral.append(max(values))
            else:
                self.integral.append(0.)

class TimeLapses(Bursts):
    def __init__(self, bursts):
        self.bursts = bursts
        self.time_lapses = []
        self._time_lapses()

    def _time_lapses(self):
        fitxer = self.bursts.data.source
        data = open(fitxer,"r").readlines()
        bursts = self.bursts._burst_list
        for burst in bursts:
            init, fin = burst
            data_init = self._parse(data[init])
            data_fin = self._parse(data[fin])
            t_init, e_init = data_init
            t_fin, e_fin = data_fin
            delta_t = t_fin - t_init
            delta_e = e_fin - e_init
            self.time_lapses.append((init, fin, delta_t, delta_e))

    def _parse(self, data):
        line = data.split()
        t = float(line[1])
        e_strain = float(line[2])
        return (t, e_strain)

    def __str__(self):
        str_ = "# burst\tinit\tfin\tdelta_t\t\tdelta_e\n"
        for lap in self.time_lapses:
            index = self.time_lapses.index(lap)
            str_ += "  %d\t%d\t%d\t%e\t%e\n" % ((index,) + lap)
        return str_
