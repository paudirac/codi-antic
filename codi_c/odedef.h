/*************************************************
 * File: odedef.h
 *
 * Description:
 *   - definicio del sistema d'equacions
 *     diferencials ordinaries segons els
 *     convenis de la llibreria gsl.
 *
 * Author: Pau Cervera
 * Date: 10.01.2005
 *
 *************************************************/
#include <gsl/gsl_rng.h>
#ifndef ODEDEF_H
#define ODEDEF_H

typedef int (*ode_function_pointer) 
     (double t, const double sys[], double f[], void*params);

typedef int (*forca_function_pointer)
     (const double t, double f[], const double sys[],
      const int a_index, const int b_index);

int odedef_alloc (void);
int odedef_free (void);

ode_function_pointer odedef_get_ode_handle (void);
forca_function_pointer odedef_get_forca_handle (void);

int energia_de_forca (const double t, double f[],const double sys[],
		      const int a_index, const int b_index);

#endif
