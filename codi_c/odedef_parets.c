#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "sistema.h"
#include "odedef_parets.h"
#include "boundaries.h"
#include "ioutils.h"

typedef int (*funcio_paret_pointer)
     (double t, const double sys[], double f[], void *params);

funcio_paret_pointer left_forces = NULL;
funcio_paret_pointer right_forces = NULL;

/* Prototipus de les funcions paret.  */
int none_force (double t, const double sys[],
		double f[], void *params);
int left_xu (double t, const double sys[],
		  double f[], void *params);
int right_xu (double t, const double sys[],
		   double f[], void *params);
int left_y (double t, const double sys[],
	    double f[], void *params);
int right_y (double t, const double sys[],
	     double f[], void *params);

int bottom_reflexive_force (double t, const double sys[], 
			    double f[], void *params);

int
odedef_parets_alloc (void)
{
  switch (paret_L_mode)
    {
    case LEFT_XU:
      left_forces = &left_xu;
      break;
    case LEFT_Y:
      left_forces = &left_y;
      break;
    default:
      left_forces = &none_force;
      break;
    }

  switch (paret_R_mode)
    {
    case RIGHT_XU:
      right_forces = &right_xu;
      break;
    case RIGHT_Y:
      right_forces = &right_y; 
      break;
    default: 
      right_forces = &none_force; 
      break;
    }

  char log_paret_l[100];
  sprintf (log_paret_l, "paret L en mode: %s", 
	   odedef_parets_mode_name (paret_L_mode));
  log_to_logfile (log_paret_l, __FILE__, __LINE__);

  char log_paret_r[100];
  sprintf (log_paret_r, "paret R en mode: %s",
	   odedef_parets_mode_name (paret_R_mode));
  log_to_logfile (log_paret_r, __FILE__, __LINE__);

  return 0;
}

int
odedef_parets_free (void)
{
  left_forces = NULL;
  right_forces = NULL;
  return 0;
}

char *parets_name = NULL;

char *
odedef_parets_mode_name (const int mode)
{
/*   if (parets_name != NULL) */
/*     free (parets_name); */
  parets_name = (char *) malloc (100 * sizeof (char));
  
  switch (mode) {
  case LEFT_XU:
    parets_name = "left_xu";
    break;
  case RIGHT_XU:
    parets_name = "right_xu";
    break;
  case LEFT_Y:
    parets_name = "left_y";
    break;
  case RIGHT_Y:
    parets_name = "right_y";
    break;
  default:
    parets_name = "none";
    break;
  }

  return parets_name;
}

int
odedef_parets_forces (double t, const double sys[],
		      double f[], void *params)
{
  left_forces (t, sys, f, params);
  right_forces (t, sys, f, params);

  switch (cells_geometry)
    {
    case COUETTE:
      break;
    case BOTTOM_REFLEXIVE_UP_FREE:
      bottom_reflexive_force (t, sys, f, params);
      break;
    case FREE:
      break;
    case SHEAR:
      break;
    default:
      break;
    }
  return 0;
}

/* inline int */
/* odedef_parets_esborra_forces_espuries (double f[]) */
/* { */
/*   int i; */
/*   for (i = sys_paret_L_init(); i < sys_paret_L_fin(); i++) */
/*     f[i] = f[index_y(i)] = 0.; */
/*   for (i = sys_paret_R_init(); i < sys_paret_R_fin(); i++) */
/*     f[i] = f[index_y(i)] = 0.; */

/*   return 0; */
/* } */


int
odedef_parets_energia (double t, const double sys[],
		       double f[], void *params)
{
  return 0;
}

int
none_force (double t, const double sys[],
	    double f[], void *params)
{
  return 0;
}

int
left_xu (double t, const double sys[],
	      double f[], void *params)
{
  int i;
  for (i = sys_paret_L_init (); i < sys_paret_L_fin (); i++)
    f[i] = f[i] + V;
  return 0;
}

int
right_xu (double t, const double sys[],
	       double f[], void *params)
{
  int i;
  for (i = sys_paret_R_init (); i < sys_paret_R_fin (); i++)
    f[i] = f[i] - V;
  return 0;
}

int
left_y (double t, const double sys[],
	double f[], void *params)
{
  int i;
  for (i = sys_paret_L_init (); i < sys_paret_L_fin (); i++)
    f[index_y(i)] = f[index_y(i)] - V;
  return 0;
}

int
right_y (double t, const double sys[],
	double f[], void *params)
{
  int i;
  for (i = sys_paret_R_init (); i < sys_paret_R_fin (); i++)
    f[index_y(i)] = f[index_y(i)] + V;
  return 0;
}

int
bottom_reflexive_force (double t, const double sys[], 
			double f[], void *params)
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double y_min = dim -> y_phys_min;
  
  y_min = y_min + dist_a * range_factor / 2.;
  int i;
  for (i = 0; i < phys_size; i++)
    {
      if (sys[index_y(i)] < y_min)
	{
	  if (f[index_y(i)] < 0.)
	    {
	      f[index_y(i)] = - f[index_y(i)];
	      /* de fet, l'exponencial aquella ja
	       * era impenetrable, no sé perquè ho
	       * he tocat! Ara no recordo com coi
	       * era! :(  */
	    }
	}
    }

  return 0;
}

/* int left_evolve_to (const double t); */
/* int right_evolve_to (const double t); */

/* int odedef_parets_evolve_to (const double t) */
/* { */
/*   left_evolve_to (t); */
/*   right_evolve_to (t); */
/*   return 0; */
/* } */

/* int */
/* left_evolve_to (const double t) */
/* { */
/*   int i; */
/*   for (i = sys_paret_L_init (); i < sys_paret_L_fin (); i++) */
/*     { */
/*       printf("L %d\n",i); */
/*     } */
/*   return 0; */
/* } */

/* int */
/* right_evolve_to (const double t) */
/* { */
/*   int i; */
/*   for (i = sys_paret_R_init (); i < sys_paret_R_fin (); i++) */
/*     { */
/*       printf("R %d\n",i); */
/*     } */
/*   return 0; */
/* } */
