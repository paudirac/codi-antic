/*************************************************
 * File: odedef.c
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include "cells.h"
#include "odedef.h"
#include "odedef_parets.h"
#include "globals.h"
#include "sistema.h"
#include "boundaries.h"

int ode_func (double t, const double sys[],
	      double f[], void *params);

ode_function_pointer odedef_get_ode_handle (void) { return &ode_func; }
//ode_function_pointer odedef_get_energy_handle (void) { return &ode_energy_; }

forca_function_pointer forca;
static __inline__ int forca_default (const double t, double f[], 
				     const double sys[],
				     const int a_index, const int b_index);
int forca_couette (const double t, double f[], const double sys[],
		   const int a_index, const int b_index);
static __inline__ int forca_morse (const double t, double f[],
				   const double sys[],
				   const int a_index, const int b_index);
static __inline__ int forca_default_amb_bonds (const double t, double f[], 
					       const double sys[],
					       const int a_index, const int b_index);

forca_function_pointer
odedef_get_forca_handle (void)
{
  return forca;
}

int
odedef_alloc (void)
{
  switch (cells_geometry)
    {
    case COUETTE:
      forca = &forca_couette;
      break;
    case BOTTOM_REFLEXIVE_UP_FREE:
      forca = &forca_default;
      break;
    case FREE:
      forca = &forca_default;
      //      forca = &forca_morse;
      //      forca = &forca_default_amb_bonds;
      break;
    case SHEAR:
      forca = &forca_couette;
      break;
    default:
      forca = &forca_default;
      break;
    }

  return 0;
}

int
odedef_free (void)
{
  return 0;
}

int
ode_func (double t, const double sys[],
	   double f[], void *params)
     /* Funcio on es defineix tot el sistema
      * d'equacions diferencials de la forma
      * dy_i(t)/dt = f_i(t,y_1(t),...,y_n(t)).
      * y[] emmagatzema el sistema, mentre que
      * f[] emmagatzema les forces generalitzades.
      * params conte els parametres del sistema.*/
{
  int i;
  for (i = 0; i < sys_array_size; i++)
    f[i] = 0.;

  cells_loop_force (t, sys, f, params, forca);
  odedef_parets_esborra_forces_espuries (f);
  odedef_parets_forces (t, sys, f, params);

  return 0;
}

#define minima_imatge(a, b) ( abs( (a) ) < abs( (b) ) ? (a) : (b) )

static inline int
forca_default (const double t, double f[],const double sys[],
	       const int a_index, const int b_index)
     /* Funcio que calcula les forces que es fan les partícules
      * amb índexs: a_index i b_index
      */
{
  double r2, over_r2, factor;
  double over_r6;
  r2 = over_r2 = factor = 0.;
  
  double x = sys[a_index] - sys[b_index]; /* La distància en x és correcta.  */
  double y = sys[index_y(a_index)] - sys[index_y(b_index)];

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 24./GAMMA;
  factor = over_r6 * (2 * over_r6 - 1) * factor;
  factor = over_r2 * factor;

  f[a_index] = f[a_index] + factor * x;
  f[index_y(a_index)] = f[index_y(a_index)] + factor * y;


  f[b_index] = f[b_index] - factor * x;
  f[index_y(b_index)] = f[index_y(b_index)] - factor * y;

  return 0;
}

int
forca_couette (const double t, double f[],const double sys[],
       const int a_index, const int b_index)
     /* Funcio que calcula les forces que es fan les partícules
      * amb índexs: a_index i b_index
      */
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double y_max = dim->y_max;
  double y_min = dim->y_min;

  double r2, over_r2, factor;
  double over_r6;
  r2 = over_r2 = factor = 0.;
  
  double x = sys[a_index] - sys[b_index]; /* La distància en x és correcta.  */

  /* Hem de triar la imatge més propera.  */
  double ya = sys[index_y(a_index)];
  double yb = sys[index_y(b_index)];

  double yb_imatge = yb;

  y_min = y_min - delta_y / 2.;
  y_max = y_max + delta_y / 2.;

  if (yb > y_max)
    yb_imatge = y_min + (yb - y_max);
  else if (yb < y_min)
    yb_imatge = y_max - (y_min - yb);

  double y = ya - yb_imatge;
/*   fprintf (stderr, "%d %d ya: %f yb: %f yb_imatge: %f\n", */
/* 	   a_index, b_index, ya, yb, yb_imatge); */

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 24./GAMMA;
  factor = over_r6 * (2 * over_r6 - 1) * factor;
  factor = over_r2 * factor;
/*   if (no_es_paret (a_index)) { */
/*     f[a_index] = f[a_index] + factor * x; */
/*     f[index_y(a_index)] = f[index_y(a_index)] + factor * y; */
/*   } */
/*   if (no_es_paret (b_index)) { */
/*     f[b_index] = f[b_index] - factor * x; */
/*     f[index_y(b_index)] = f[index_y(b_index)] - factor * y; */
/*   } */

  f[a_index] = f[a_index] + factor * x;
  f[index_y(a_index)] = f[index_y(a_index)] + factor * y;

  f[b_index] = f[b_index] - factor * x;
  f[index_y(b_index)] = f[index_y(b_index)] - factor * y;

  return 0;
}

int
energia_de_forca (const double t, double f[],const double sys[],
		  const int a_index, const int b_index)
{
  double distancia_y = sys_y_distance ();
  double r2, over_r2, factor;
  double over_r6;
  r2 = over_r2 = factor = 0.;
  
  double x = sys[a_index] - sys[b_index]; /* La distància en x és correcta.  */

  /* Hem de triar la imatge més propera.  */
  double ya = sys[index_y(a_index)];
  double yb = sys[index_y(b_index)];
  double yb_imatge1 = yb + distancia_y;
  double yb_imatge2 = yb - distancia_y;
  
  double y = minima_imatge (ya - yb, ya - yb_imatge1);
  y = minima_imatge (y, ya - yb_imatge2); /* y és la menor distància.  */

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 1./GAMMA;

  if (no_es_paret (a_index))
    f[a_index] = f[a_index] + factor;
  if (no_es_paret (b_index))
    f[b_index] = f[b_index] + factor;

  return 0;
}

static inline int
forca_morse (const double t, double f[], const double sys[],
	     const int a_index, const int b_index)
{
  const double r_0 = 1.122462048309373; // equilibri L-J 12-6

  const double a = 14.;
  const double D = GAMMA * r_0 / a;//1. / (2. * 24. * a);

  double x = sys[a_index] - sys[b_index];
  double y = sys[index_y(a_index)] - sys[index_y(b_index)];
  double r2 = x * x + y * y;
  double r = sqrt(r2);
  double exp_ = exp (-1. * a * (r - r_0));
  double factor = 1. / GAMMA;
  factor = -2. * D * a * (1 - exp_) * exp_ / r;

  f[a_index] = f[a_index] + factor * x;
  f[index_y(a_index)] = f[index_y(a_index)] + factor * y;

  f[b_index] = f[b_index] - factor * x;
  f[index_y(b_index)] = f[index_y(b_index)] - factor * y;

  return 0;
}

static inline int
forca_default_amb_bonds (const double t, double f[],const double sys[],
	       const int a_index, const int b_index)
     /* Funcio que calcula les forces que es fan les partícules
      * amb índexs: a_index i b_index
      */
{
  double r2, over_r2, factor;
  double over_r6;
  r2 = over_r2 = factor = 0.;
  
  double x = sys[a_index] - sys[b_index]; /* La distància en x és correcta.  */
  double y = sys[index_y(a_index)] - sys[index_y(b_index)];

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 24./GAMMA;
  factor = over_r6 * (2 * over_r6 - 1) * factor;
  factor = over_r2 * factor;

  f[a_index] = f[a_index] + factor * x;
  f[index_y(a_index)] = f[index_y(a_index)] + factor * y;


  f[b_index] = f[b_index] - factor * x;
  f[index_y(b_index)] = f[index_y(b_index)] - factor * y;

  /* Ara els bonds de la xarxa quadrada  */
  double k = 24.;
  f[a_index] = f[a_index] - k * abs(x);
  f[index_y(a_index)] = f[index_y(a_index)] - k * abs(y);

  f[b_index] = f[b_index] - k * abs(x);
  f[index_y(b_index)] = f[index_y(b_index)] - k * abs(y);

  return 0;
}


