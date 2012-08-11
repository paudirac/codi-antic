/* data_r.c  */

/* Nota: de moment faig l'histograma de les distàncies, és a dir,
 * el g(r), de tots els runs i sumat per tots els àtoms. No sé si
 * això té sentit, però també podria calcular-se localment, suposo.  */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_histogram.h>
#include "output_settings.h"
#include "globals.h"
#include "sistema.h"
#include "cells.h"
#include "boundaries.h"
#include "macroutils.h"

static gsl_histogram *hist;
static int n_bins_r = 1000;
static double r_min = 0.;
static double r_max;

function_to_loop_pointer distancia = NULL;
int distancia_couette (const double t, const double sys[],
		       double d[], void * other,
		       const int a_index, const int b_index);
int distancia_default (const double t, const double sys[],
		       double d[], void * other,
		       const int a_index, const int b_index);


int
data_r_alloc (FILE *pfile)
{
  switch (cells_geometry)
    {
    case COUETTE:
      distancia = &distancia_couette;
      break;
    case FREE:
      distancia = &distancia_default;
      break;
    case SHEAR:
      distancia = &distancia_couette;
      break;
    default:
      distancia = &distancia_default;
      break;
    }

  if (histogram_r)
    {
      r_max = 1.2 * range_factor * dist_a;
      hist = gsl_histogram_alloc (n_bins_r);
      gsl_histogram_set_ranges_uniform (hist, r_min, r_max);
    }

  return 0;
}

int
data_r_free (void)
{
  if (histogram_r)
    {
      FILE *pfile = fopen (histograms_file, "a");
      if (pfile == 0)
	pfile = stderr;

      double sum_hist = gsl_histogram_sum (hist);
      double rang_bins = gsl_histogram_max (hist)
	- gsl_histogram_min (hist);
      double riemman_area = sum_hist * rang_bins;
      gsl_histogram_scale (hist, 1./riemman_area);

      fprintf (pfile, "pystart rhist.dat\n");
      gsl_histogram_fprintf (pfile, hist, "%e", "%e");
      fprintf (pfile, "pyend\n");

      fflush (pfile);
      fclose (pfile);

      gsl_histogram_free (hist);
    }

  return 0;
}


int
data_r_extract (void)
{
  double * sys = sys_handle ();
  double * distancies = (double *) malloc (phys_size * sizeof (double));

  int i;
  for (i = 0; i < phys_size; i++)
    distancies[i] = 0.;
  
  cells_loop_function (t, sys, distancies, hist, distancia);

/*   for (i = 0; i < phys_size; i++) */
/*     { */
/*       double r = distancies[i]  */
/* 	/ (3 * range_factor * dist_a); // Això és com una mena de densitat. */
/*     } */

/*   free (distancies); */
  FREE_POINTER (distancies);

  return 0;
}

int
data_r_print (FILE *pfile, const int snap)
{
  if (pfile == 0)
    pfile = stderr;
  return 0;
}

/* Potser s'hauria d'implementar a boundaries, perquè ha de tenir
 * en compte les condicions de contorn.  */
int
distancia_couette (const double t, const double sys[],
		   double d[], void * other,
		   const int a_index, const int b_index)
{
  gsl_histogram * hist_ = (gsl_histogram *) other;

  dimensions *dim = sys_initial_dimensions_handle ();
  double y_max = dim->y_max;
  double y_min = dim->y_min;
  y_min = y_min - delta_y;
  y_max = y_max + delta_y;

  double x = sys[a_index] - sys[b_index];
  
  double ya = sys[index_y(a_index)];
  double yb = sys[index_y(b_index)];
  
  double yb_imatge = yb;
  if (yb > y_max)
    yb_imatge = y_min + (yb - y_max);
  else if (yb < y_min)
    yb_imatge = y_max - (y_min - yb);

  double y = ya - yb_imatge;

  double r = sqrt (x*x + y*y);

  if (no_es_paret (a_index))
    {
      d[a_index] = d[a_index] + r;
      if (histogram_r)
	gsl_histogram_increment (hist_, r);
    }
  if (no_es_paret (b_index))
    d[b_index] = d[b_index] + r;

  return 0;
}

int
distancia_default (const double t, const double sys[],
		   double d[], void * other,
		   const int a_index, const int b_index)
{
  gsl_histogram * hist_ = (gsl_histogram *) other;

  double x = sys[a_index] - sys[b_index];
  double y = sys[index_y(a_index)] - sys[index_y(b_index)];

  double r = sqrt (x*x + y*y);

  if (no_es_paret (a_index))
    {
      d[a_index] = d[a_index] + r;
      if (histogram_r)
	gsl_histogram_increment (hist_, r);
    }
  if (no_es_paret (b_index))
    d[b_index] = d[b_index] + r;

  return 0;
}
