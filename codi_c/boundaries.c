/* boundaries.c  */
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include "boundaries.h"
#include "globals.h"
#include "sistema.h"

int (*boundary_resolve) (void);
int boundary_resolve_couette (void);
int boundary_resolve_bottom_reflexive_up_free (void);
int boundary_resolve_default (void) { return 0; }

int
boundaries_alloc (void)
{
  switch (cells_geometry)
    {
    case COUETTE:
      boundary_resolve = 
	&boundary_resolve_couette;
      break;
    case BOTTOM_REFLEXIVE_UP_FREE:
      boundary_resolve =
	&boundary_resolve_bottom_reflexive_up_free;
      break;
    case FREE:
      boundary_resolve =
	&boundary_resolve_default;
      break;
    case SHEAR:
      boundary_resolve = 
	&boundary_resolve_couette;
      break;
    default:
      boundary_resolve = 
	&boundary_resolve_default;
      break;
    }

  return 0;
}

int
boundaries_free (void)
{
  return 0;
}

int
boundaries_resolve_boundary_conflicts (void)
{
  boundary_resolve ();
  return 0;
}

int
boundary_resolve_couette (void)
     /* Resol els conflictes deguts a que partícules surtin per
      * la part superior o inferior i hagin de passar-se a la seva
      * imatge periòdica.  */
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double y_max = dim -> y_max; // initial_dimensions -> y_max;
  double y_min = dim -> y_min; // initial_dimensions -> y_min;

  y_min = y_min - delta_y / 2.; // ups, ja estava bé, crec.
  y_max = y_max + delta_y / 2.;

  double *sys = sys_handle ();

  int i;
  for (i = 0; i < sys_size; i++)
    {
      double y = sys[index_y(i)];

      double y_new = y;
      if (y > y_max)
	{
	  y_new = y_min + (y - y_max);
/* 	  fprintf(stderr,"pbc_up_down: %d %f %f\t%f %f\n", */
/* 		  i,y,y_new,y_max,y_min); */
	}
      if (y < y_min)
	{
	  y_new = y_max - (y_min - y);
/* 	  fprintf(stderr,"pbc_down_up: %d %f %f\t%f %f\n", */
/* 		  i,y,y_new,y_max,y_min); */
	}

      sys[index_y(i)] = y_new;
    }

  return 0;
}

int
boundary_resolve_bottom_reflexive_up_free (void)
{
  dimensions *dim = sys_dimensions_handle ();
/*   double y_max = dim -> y_max; // initial_dimensions -> y_max; */
  double y_min = dim -> y_min; // initial_dimensions -> y_min;

  double *sys = sys_handle ();
  int i;
  for (i = 0; i < phys_size; i++)
    {
      double y = sys[index_y(i)];
      if (y < y_min)
	abort ();
    }

  return 0;
}

double r2_ab_couette (const int a, const int b, const double sys[]);
double r2_ab_default (const int a, const int b, const double sys[]);

double
r2_ab (const int a, const int b, const double sys[])
{
  double r2 = 0.;
  switch (cells_geometry)
    {
    case COUETTE:
      r2 = r2_ab_couette (a, b, sys);
      break;
    case FREE:
      r2 = r2_ab_default (a, b, sys);
      break;
    default:
      r2 = r2_ab_default (a, b, sys);
      break;
    }

  return r2;
}

double
r2_ab_couette (const int a, const int b, const double sys[])
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double y_max = dim->y_max;
  double y_min = dim->y_min;

  double x = sys[a] - sys[b];

  double ya = sys[index_y(a)];
  double yb = sys[index_y(b)];

  double yb_imatge = yb;
  y_min = y_min - delta_y / 2.;
  y_max = y_max + delta_y / 2.;

  if (yb > y_max)
    yb_imatge = y_min + (yb - y_max);
  else if (yb < y_min)
    yb_imatge = y_max - (y_min - yb);

  double y = ya - yb_imatge;

  return x*x + y*y;
}

double
r2_ab_default (const int a, const int b, const double sys[])
{
  double x = sys[a] - sys[b];
  double y = sys[index_y(a)] - sys[index_y(b)];

  return x*x + y*y;
}

/* double r_ab_y_couette (const int a, const int b, double sys[]); */
/* double r_ab_y_default (const int a, const int b, double sys[]); */

/* double */
/* r_ab_y (const int a, const int b, const double sys[]) */
/* { */
/*   double r_ab_y_ = 0.; */
/*   switch (cells_geometry) */
/*     { */
/*     case COUETTE: */
/*       r_ab_y_ = r_ab_y_couette (a, b, sys); */
/*       break; */
/*     case FREE: */
/*     default: */
/*       r_ab_y_ = r_ab_y_default (a, b, sys); */
/*       break; */
/*     } */

/*   return r_ab_y_; */
/* } */

/* double r_ab_y_couette (const int a, const int b, double sys[]) */
/* { */
/*   return 0.; */
/* } */

/* double r_ab_y_default (const int a, const int b, double sys[]) */
/* { */
/*   return 0.; */
/* } */
