/* energy.c  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include "cells.h"
#include "energy.h"
#include "globals.h"
#include "sistema.h"
#include "boundaries.h"
#include "odedef.h"

static forca_function_pointer energy_f = NULL;
int energy_couette (const double t, double ene[], const double sys[],
		    const int a_index, const int b_index);
int energy_default (const double t, double ene[], const double sys[],
		    const int a_index, const int b_index);

int
energy_alloc (void)
{
  switch (cells_geometry)
    {
    case COUETTE:
      energy_f = &energy_couette;
      break;
    case FREE:
      energy_f = &energy_default;
      break;
    case SHEAR:
      energy_f = &energy_couette;
      break;
    default:
      energy_f = &energy_default;
      break;
    }

  return 0;
}

int
energy_free (void)
{
  return 0;
}

int
energy_func (double t, const double sys[], double ene[], void *params)
{
  int i;
  for (i = 0; i < phys_size; i++)
    ene[i] = 0.; /* Crec que no passarà res perquè les energy
		  * ja ho saben, però mirar que no apunti 
		  * fora de rang.  */
/*   energy_function_parets (t, sys, ene, params);   */
  cells_loop_force (t, sys, ene, params, energy_f);

  return 0;
}

int
energy_couette (const double t, double ene[], const double sys[],
		const int a_index, const int b_index)
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double y_max = dim->y_max;
  double y_min = dim->y_min;

  double r2, over_r2, factor;
  double over_r6;

  double x = sys[a_index] - sys[b_index];

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

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 4 * over_r6 * (over_r6 - 1);
  if (no_es_paret (a_index))
    ene[a_index] = ene[a_index] + factor;
  if (no_es_paret (b_index))
    ene[b_index] = ene[b_index] + factor;

  return 0;
}

int
energy_default (const double t, double ene[], const double sys[],
		const int a_index, const int b_index)
{
  double r2, over_r2, factor;
  double over_r6;

  double x = sys[a_index] - sys[b_index];
  double y = sys[index_y(a_index)] - sys[index_y(b_index)];

  r2 = x*x + y*y;
  over_r2 = 1./r2;
  over_r6 = over_r2 * over_r2 * over_r2;
  factor = 4 * over_r6 * (over_r6 - 1);
  if (no_es_paret (a_index))
    ene[a_index] = ene[a_index] + factor;
  if (no_es_paret (b_index))
    ene[b_index] = ene[b_index] + factor;

  return 0;
}
