#include <stdlib.h>
#include <stdio.h>
#include "output_settings.h"
#include "globals.h"
#include "dr_writer.h"
/* #include "data_rug.h" */
/* #include "data_energy.h" */
/* #include "data_stress.h" */
/* #include "data_velos.h" */
/* #include "data_r.h" */
/* #include "data_n.h" */

/* part nova  **/
#include "sistema.h"
#include "odedef.h"
#include "odedef_parets.h"
#include "cells.h"
#include "macroutils.h"

static FILE *pfile_historia;
static FILE *pfile_historia_forces;

int abort_for_open_failure (const char * file_name);

int
data_alloc (void)
{
  pfile_historia = fopen (historia_file, "w");
  if (pfile_historia == 0) abort_for_open_failure (historia_file);
  
  pfile_historia_forces = fopen (historia_velos_file, "w");
  if (pfile_historia_forces == 0) 
    abort_for_open_failure (historia_velos_file);

  dr_writer_init (pfile_historia);
  dr_writer_init (pfile_historia_forces);

  return 0;
}

int
data_free (void)
{
  dr_writer_fin (pfile_historia);
  fflush (pfile_historia);
  fclose (pfile_historia);
  fflush (pfile_historia_forces);
  fclose (pfile_historia_forces);
  return 0;
}

int
data_extract (void)
{
  return 0;
}

int
data_print (const int snap)
{
  double * sys = sys_handle ();
  dr_writer_write_frame (pfile_historia, snap, sys);
  fflush (pfile_historia);

/* S'agafa forca_handle, enlloc de odedef_func, perquè volem la forca també
 * a les parets, i odedef_func, treu la interacció de phys amb parets i la
 * substitueix per V. Així la f de les parets és la que els toca.  */
  double * f = (double *) malloc (sys_array_size * sizeof (double));
  forca_function_pointer forca = odedef_get_forca_handle ();
  int i; for (i = 0; i < sys_array_size; i++) f[i] = 0.;
  cells_loop_force (t, sys, f, NULL, forca);

  dr_writer_write_frame (pfile_historia_forces, snap, f);
  fflush (pfile_historia_forces);
  FREE_POINTER (f);

  return 0;
}

int
abort_for_open_failure (const char * file_name)
{
  fprintf (stderr, "No s'ha pogut obrir %s. Abortant...", file_name);
  return 0;
}
