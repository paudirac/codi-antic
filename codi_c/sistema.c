#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <gsl/gsl_errno.h>
#include "globals.h"
#include "sistema.h"
#include "ioutils.h"
#include "boundaries.h"
#include "string_utils.h"
#include "macroutils.h"
#include "inicia_cristall.h"

double delta_x;
double delta_y;

static dimensions *dim;
static dimensions *initial_dimensions;

dimensions *
dimensions_alloc (void)
{
  register dimensions *dim_ = (dimensions *) malloc (sizeof (dimensions));
  if (dim_ == 0)
    {
      error_at_line (0,errno,__FILE__,__LINE__,
		     "No puc allocatar l'struct dimensions\n.");
      return 0;
    }
  return dim_;
}

int
dimensions_copy (dimensions *dest, dimensions *src)
{
  dest -> x_phys_max = src -> x_phys_max;
  dest -> x_phys_min = src -> x_phys_min;
  dest -> y_phys_max = src -> y_phys_max;
  dest -> y_phys_min = src -> y_phys_min;
  dest -> x_max = src -> x_max;
  dest -> x_min = src -> x_min;
  dest -> y_max = src -> y_max;
  dest -> y_min = src -> y_min;
 
  return 0;
}

int
dimensions_free (dimensions *dim_)
{
  if (dim_ != 0)
    FREE_POINTER (dim_); //free (dim_);

  return 0;
}


int calc_dimensions (void);

dimensions * sys_dimensions_handle (void)
{
  return dim;
}

dimensions * sys_initial_dimensions_handle (void)
{
  return initial_dimensions;
}

/* Status:
 * int load_sys (char *fitxer_inici, const int NX_EXTRA) no està
 * implementada.
 */

/* Sistema.  */
static double *sys = 0;
int sys_size;
double * sys_handle (void) { return sys; }

/*
 * sys està ordenat de la forma següent:
 * ---------------------------------------------------------------------------
 * | x0 | x1 | ... | x(N-1) | xL0 | xL1 | ... | xL(m-1) | xR0 |... | xR(m-1) |
 * ---------------------------------------------------------------------------
 * | y0 | y1 | ... | y(N-1) | yL0 | yL1 | ... | yL(m-1) | yR0 |... | yR(m-1) |
 * ---------------------------------------------------------------------------
 * On N = phys_size = NX * NY (nombre total de partícules físiques)
 *    m = nx_paret
 * És a dir:
 *    que sys[0:phys_size-1] conté les x de les partícules físiques
 *    que sys[phys_size:phys_size + NY * nx_paret] conté paret L
 *    que sys[phys_size + nx_paret: phys_size + 2 * NY * nx_paret] conté R
 *    que a continuació venen les y's que corresponen a y's, en l'ordre que
 *    toca:
 *         x_i = sys[i]
 *         y_i = sys[i + sys_size] = sys[index_y(i)]
 */

/* Columnes extra.  */
int nx_paret = 1; // 1?
int ny_paret;
// ny_extra ha de ser parell, molt em temo i ho habia oblidat. posar-ho
// per la força!
//static int ny_extra = 4; // això és l'únic que podria posar-se a globals.
int paret_size = 0;
int phys_size;
static int nx_total;

/* int sys_paret_L_init (void) { return phys_size; } */
/* int sys_paret_L_fin (void) { return phys_size + paret_size; } */
/* int sys_paret_R_init (void) { return phys_size + paret_size; } */
/* int sys_paret_R_fin (void) { return phys_size + 2 * paret_size; } */

int
log_system (void)
{
  char log_x[100];
  sprintf (log_x, "NX = %d", NX);
  log_to_logfile (log_x, __FILE__, __LINE__);

  char log_y[100];
  sprintf (log_y, "NY = %d", NY);
  log_to_logfile (log_y, __FILE__, __LINE__);

  char log_sys_[100];
  sprintf(log_sys_, "sys_size = %d", sys_size);
  log_to_logfile (log_sys_, __FILE__, __LINE__);

  char log_sys_array_[100];
  sprintf(log_sys_array_, "sys_array_size = %d", sys_array_size);
  log_to_logfile (log_sys_array_, __FILE__, __LINE__);

  char log_sys_phys[100];
  sprintf(log_sys_phys, "phys_size = %d", phys_size);
  log_to_logfile (log_sys_phys, __FILE__, __LINE__);

  return 0;
}

int
sys_alloc (void)
{
  if (ny_extra % 2 == 1)
    {
      ny_extra = ny_extra + 1;
      char log_ny_extra[100];
      sprintf (log_ny_extra, 
	       "**ATENCIÓ** Detectat ny_extra senar.\nSet ny_extra = %d.", 
	       ny_extra);
      log_to_logfile (log_ny_extra, __FILE__, __LINE__);
    }

  phys_size = NX * NY;
  ny_paret = NY + 2 * ny_extra;
  paret_size = nx_paret * ny_paret;
  // sys_size està a global.h
  sys_size =  phys_size + 2 * paret_size;
  sys_array_size = 2 * sys_size;

  nx_total = NX + 2 * nx_paret;

  sys = (double *) malloc (sys_array_size * sizeof (double));
  if (sys == 0)
    {
      error_at_line(0,errno,__FILE__,__LINE__,
		    "No puc allocatar el sistema de dimensió: %d\n.",
		    sys_array_size);
      return GSL_ENOMEM;
    }

  dim = dimensions_alloc ();
  initial_dimensions = dimensions_alloc ();
  
  log_to_logfile ("Sistema allocatat.", __FILE__, __LINE__);
  log_system ();

  return GSL_SUCCESS;
}

int
sys_free (void)
{
  FREE_POINTER (sys);//free (sys);
  log_to_logfile ("Alliberat l'espai de memòria ocupat per sys.",
		  __FILE__, __LINE__);
  return GSL_SUCCESS;
}

double sys_x_distance (void) // Això és la distància inicial.
{
  return (dim->x_max - dim->x_min);
}

double sys_y_distance (void) // Això és la distància inicial.
{
  //  double y_step = delta_y;
  //  return NY * y_step;
  /* Confiant que primer s'ha cridat a sys_x_distance, que crida
   * a calc_dimensions.  */
  return (dim->y_max - dim->y_min);
}

int
sys_calc_dimensions (void)
{
  double x_phys_max = sys[0];
  double x_phys_min = sys[0];
  double y_phys_max = sys[index_y(0)];
  double y_phys_min = sys[index_y(0)];

  int i;
  for (i = 0; i < phys_size; i++)
    {
      double x = sys[i];
      double y = sys[index_y(i)];

      if (x > x_phys_max) x_phys_max = x;
      if (x < x_phys_min) x_phys_min = x;
      if (y > y_phys_max) y_phys_max = y;
      if (y < y_phys_min) y_phys_min = y;
    }

  double x_max = x_phys_max;
  double x_min = x_phys_min;
  double y_max = y_phys_max;
  double y_min = y_phys_min;
  for (i = phys_size; i < sys_size; i++)
    {
      double x = sys[i];
      double y = sys[index_y(i)];

      if (x > x_max) x_max = x;
      if (x < x_min) x_min = x;
      if (y > y_max) y_max = y;
      if (y < y_min) y_min = y;
    }

  dim->x_phys_max = x_phys_max;
  dim->x_phys_min = x_phys_min;
  dim->y_phys_max = y_phys_max;
  dim->y_phys_min = y_phys_min;
  dim->x_max = x_max;
  dim->x_min = x_min;
  dim->y_max = y_max;
  dim->y_min = y_min;

  return 0;
}

int
sys_pinta_dimensions (void)
{
  printf ("dimensions:\n");
  printf ("\tx_phys_max: %f\n",dim->x_phys_max);
  printf ("\tx_phys_min: %f\n",dim->x_phys_min);
  printf ("\ty_phys_max: %f\n",dim->y_phys_max);
  printf ("\ty_phys_min: %f\n",dim->y_phys_min);
  printf ("\tx_max: %f\n",dim->x_max);
  printf ("\tx_min: %f\n",dim->x_min);
  printf ("\ty_max: %f\n",dim->y_max);
  printf ("\ty_min: %f\n",dim->y_min);

  return 0;
}

double load_sys (char * fitxer_inici, const int NX_EXTRA);

int
sys_init_configuration (void)
{
  if (LOAD) 
    T_INICIAL = load_sys (load_file, 2 * nx_paret);
  else
    inicia_sys_new_crystall (sys);

  inicia_cristall_add_surface_steps ();
  inicia_cristall_add_vacancies ();
  inicia_cristall_add_surface_vacancies ();
  inicia_cristall_add_random_perturbation_if_necessary ();

  sys_calc_dimensions ();
  dimensions_copy (initial_dimensions, dim);

  return GSL_SUCCESS;
}

int
inicia_sys_new_crystall (double *sys_)
{
  log_to_logfile("Inicio configuracio cristallina.", __FILE__, __LINE__);

  switch (cristall_dist_a_direction)
    {
    case X:
      delta_x = dist_a;
      delta_y = dist_a * sqrt (3.) / 2.;
      inicia_cristall_sistema_orientat_x ();
      inicia_cristall_paret_commensurada_amb_x ();
      log_to_logfile ("cristall_dist_a_direction = X.", __FILE__, __LINE__);
      break;
    case Y:
      delta_x = dist_a * sqrt (3.) / 2.;
      delta_y = dist_a;
      inicia_cristall_sistema_orientat_y ();
      inicia_cristall_paret_commensurada_amb_y ();
      log_to_logfile ("cristall_dist_a_direction = Y.", __FILE__, __LINE__);
      break;
    default:
      delta_x = dist_a;
      delta_y = dist_a * sqrt (3.) / 2.;
      inicia_cristall_sistema_orientat_x ();
      inicia_cristall_paret_commensurada_amb_x ();
      log_to_logfile ("cristall_dist_a_direction = X.", __FILE__, __LINE__);
      break;
    }

  return 0;
}

double
load_sys (char * fitxer_inici, const int NX_EXTRA)
{
  log_to_logfile(
      "Crida a la funció load_sys, NOT_IMPLEMENTED_YET. Aborto execució.",
      __FILE__, __LINE__);
  abort ();
  return 0.;
}

double
sys_engineering_strain (void)
{
  double long_x_ini = initial_dimensions -> x_phys_max 
                    - initial_dimensions -> x_phys_min;
/*   double long_x = dim -> x_max */
/*                 - dim -> x_min; */

  double long_y_ini = initial_dimensions -> y_phys_max
                    - initial_dimensions -> y_phys_min;

  double vt;
  if (V > 0) vt = 2 * V * t;
  else vt = - 2 * V * t;

  double to_return = 0.;

  switch (cells_geometry)
    {
    case FREE:
/*       to_return = (long_x_ini - long_x) / long_x_ini; */
      to_return = vt / long_x_ini;
      break;
    case SHEAR:
      to_return = vt / long_y_ini;
      break;
    default:
/*       to_return = (long_x_ini - long_x) / long_x_ini; */
      to_return = vt / long_x_ini;
      break;
    }

  return to_return;
}

int
sys_renormalize_for_vacancies (int vacancies)
{
  int i;
  for (i = phys_size - vacancies; i < sys_size - vacancies; i++)

    {
      sys[i] = sys[i + vacancies];
      sys[index_y(i)] = sys[index_y(i + vacancies)];
    }

  phys_size = phys_size - vacancies;

  for (i = sys_size - vacancies; i < sys_array_size; i++)
    sys[i] = sys[i + vacancies];

  //  phys_size = phys_size - vacancies; no se vale repetir!
  sys_size = phys_size + 2 * paret_size;
  sys_array_size = 2 * sys_size;

  log_to_logfile ("Sistema reallocatat per les vacants.", __FILE__, __LINE__);
  log_system ();
  
  return 0;
}


/** Deprecated methods:  */

/* int */
/* sys_print (const int snap) */
/* { */
/*   double e_strain = sys_engineering_strain (); */
/*   char *filename = string_utils_filename (out_prefixe, snap,  */
/* 					  n_zfill, out_sufixe); */
/*   FILE *pfile = fopen (filename,"w"); */
/*   if (pfile == 0) */
/*     pfile = stdout; */

/*   int flag = 0; */
/*   int i; */
/*   for (i = 0; i < phys_size; i++) */
/*     fprintf(pfile, "%f %f %f %d %.5e\n", */
/* 	    t, sys[i], sys[index_y(i)], flag, e_strain); */

/*   if (print_parets) */
/*     { */
/*       flag = -1; */
/*       for (i = sys_paret_L_init (); i < sys_paret_L_fin (); i++) */
/* 	fprintf(pfile, "%f %f %f %d %.5e\n", */
/* 		t, sys[i], sys[index_y(i)], flag, e_strain); */

/*       flag = -2; */
/*       for (i = sys_paret_R_init (); i < sys_paret_R_fin (); i++) */
/* 	fprintf(pfile, "%f %f %f %d %.5e\n", */
/* 		t, sys[i], sys[index_y(i)], flag, e_strain); */
/*     } */

/*   fflush (pfile); */
/*   fclose (pfile); */

/*   return 0; */
/* } */
