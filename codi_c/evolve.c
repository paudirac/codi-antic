/* evolve.c  */
#include <stdio.h>
#include <stdlib.h>
#include "output_settings.h"
#include "evolve.h"
#include "globals.h"
#include "cells.h"
#include "odedef.h"
#include "odedef_random.h"
#include "string_utils.h"
#include "data.h"
#include "boundaries.h"
#include "ioutils.h"
#include "sistema.h"
#include "gsl_stuff.h"

static double ti = 0.;
static double ti_ant = 0.;

/* Prototipus funcions.  */
int update_ti (const int i);
int update_sistema_then_cells (void);
int evolve_to_ti (void);
int log_evolution (const int i);

static int snapshots;

int
null_v_abort ()
{
  log_to_logfile ("[Error] V = 0.", __FILE__, __LINE__);
  log_to_logfile ("Abort imminent.", __FILE__, __LINE__);
  fprintf (stderr, "[Error] V = 0.\nAbort imminent.\n");
  abort ();
  return 0;
}

int
evolve_setup (void)
{
  dimensions * dim = sys_initial_dimensions_handle ();
  double x_total = dim->x_phys_max - dim->x_phys_min;

  char *v_ = getenv("V");
  if (v_ != 0) V = strtod (v_, NULL);
  if (V < 0.) V = -V;
  if (V == 0.) null_v_abort ();

  char log_v[100];
  sprintf (log_v, "V = %.5f", V);
  log_to_logfile (log_v, __FILE__, __LINE__);

  if (e_strain_final != 0.)
    T_FINAL = (0.5 * e_strain_final * x_total) / V;

  if (DELTA_T_SNAP)
    {
      if (DELTA_T_SNAP < 1.e-2) DELTA_T_SNAP = 1.e-2;
      snapshots = (int) T_FINAL / DELTA_T_SNAP;
    }
  else
    {
/*       if (T_FINAL / SNAPSHOTS > 1.e-2) */
	snapshots = SNAPSHOTS;
/*       else */
/* 	snapshots = (int) (T_FINAL / 1.e-2); */
    }

  if (snapshots > 4000) snapshots = 4000;

  char log_t_final[100];
  sprintf (log_t_final, "T_FINAL = %.5f", T_FINAL);
  log_to_logfile (log_t_final, __FILE__, __LINE__);

  char log_snapshots[100];
  sprintf (log_snapshots, "SNAPSHOTS = %d", snapshots);
  log_to_logfile (log_snapshots, __FILE__, __LINE__);

  double delta_t_snaps = T_FINAL / snapshots;
  char log_t_fotos[100];
  sprintf (log_t_fotos, "delta_t_snaps = %.5e = %.3f * 0.01", 
	   delta_t_snaps, delta_t_snaps / 0.01);
  log_to_logfile (log_t_fotos, __FILE__, __LINE__);

  return 0;
}

int evolve (void)
{
/*   sys_print (0); */
  update_sistema_then_cells ();
  data_extract ();
  data_print (0);
  cells_calculate ();

  t = T_INICIAL; /* t in globals.h  */

  int i;
  for (i = 1; i <= snapshots; i++)
    {
      update_ti (i);
      update_sistema_then_cells ();
      evolve_to_ti ();
      log_evolution (i);
      data_extract ();
      data_print (i);
    }

  return 0;
}

int
update_ti (const int i)
{
  ti_ant = ti;
  ti = T_INICIAL + i * (T_FINAL - T_INICIAL) / snapshots;
  return 0;
}

int
update_sistema_then_cells (void)
{
  boundaries_resolve_boundary_conflicts (); // ha de substituir sys_resove_pbc
  sys_calc_dimensions ();
  cells_calculate ();

  return 0;
}

int
evolve_to_ti ()
{
  gsl_evolve_to (ti);
  odedef_random_evolve_to (ti);

  return 0;
}

int
log_evolution (const int i)
{
  fprintf (stdout, "%d %d Evolving from t = %f to t = %f e_strain = %f\n", 
	   i, snapshots - i, ti_ant, ti, sys_engineering_strain ());
  fprintf (stdout, "appending frame %d to ----> %s\n", i, historia_file);

  if (log_print_snapshots)
    {
      char log_evolution_[200];
      sprintf (log_evolution_, "appended snap %d to %s", i, historia_file);
      log_to_logfile (log_evolution_, __FILE__, __LINE__);
    }
  fflush (stdout);

  return 0;
}
