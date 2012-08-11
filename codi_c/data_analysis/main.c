#include <stdlib.h> /* where qsort resides  */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dr_reader.h"
#include "macroutils.h"

#include "data_dimensions.h"
#include "data_stress.h"
#include "data_velos.h"
#include "data_n.h"
#include "data_rug.h"
#include "data_stress_molecular.h"
//#include "data_volums.h" <-------- Necessito triangulacio!
#include "data_energy.h"

static int historia_defectes = 1;
static int historia_coordinacio = 0;
static int historia_stress = 1;

int setup_files (const char * directori);
int setup_funcs (const int ARGC, char * ARGV[]);
int main_args_error (void);
int not_compatible_histories (void);
int read_eval_data (void);

/* ara el prototipus d'extraccio de dades es estadistica  */
int estadistica (const sys_info_t * p_sys_info, const double * sys, 
		 const double * f, const int snap, const double t);

typedef int (* extract_function_t)
     (const sys_info_t * p_sys_info, const double * sys, 
      const double * f, const int snap, const double t);

typedef int (* print_function_t)
     (FILE * pfile, const sys_info_t * p_sys_info,
      const int snap, const double t);

static extract_function_t * extract_funcs;
static print_function_t * print_funcs;
static int extract_funcs_n;

#define POSICIONS_ "posicions"
#define FORCES_ "forces"
#define DATA_ "data"
#define DEFECTES_ "historia_defectes"
#define COORDINACIO_ "historia_coordinacio"
#define STRESS_ "historia_stress"

static FILE * pfile_posicions;
static FILE * pfile_forces;
static FILE * pfile_data;
static FILE * pfile_defectes;
static FILE * pfile_coordinacio;
static FILE * pfile_stress;

int
main (int ARGC, char *ARGV[])
{
  if (ARGC < 2) main_args_error ();
  char * directori = ARGV[1];
  setup_files (directori);
  setup_funcs (ARGC, ARGV);

  read_eval_data ();

  return 0;
}

int
setup_files (const char * directori)
{
  int dir_len = 1000;

  char * posicions = (char *) malloc 
    ((strlen(POSICIONS_) + dir_len) * sizeof (char));
  posicions = strcat (posicions, directori);
  posicions = strcat (posicions, POSICIONS_);

  char * forces = (char *) malloc 
    ((strlen (FORCES_) + dir_len) * sizeof (char));
  forces = strcat (forces, directori);
  forces = strcat (forces, FORCES_);

  pfile_posicions = fopen(posicions, "r");
  pfile_forces = fopen(forces, "r");

  fprintf (stderr, "Llegint posicions de:\t%s\n", posicions);
  fprintf (stderr, "Llegint forces de:\t%s\n", forces);

  char * data = (char *) malloc
    ((strlen (DATA_) + dir_len) * sizeof (char));
  data = strcat (data, directori);
  data = strcat (data, DATA_);

  pfile_data = fopen (data, "w");
  fprintf (stderr, "Escrivint dades a: %s\n", data);

  char * defectes = (char *) malloc
    ((strlen (DEFECTES_) + dir_len) * sizeof (char));
  defectes = strcat (defectes, directori);
  defectes = strcat (defectes, DEFECTES_);

  char * coordinacio = (char *) malloc
    ((strlen (COORDINACIO_) + dir_len) * sizeof (char));
  coordinacio = strcat (coordinacio, directori);
  coordinacio = strcat (coordinacio, COORDINACIO_);

  char * stress = (char *) malloc
    ((strlen (STRESS_) + dir_len) * sizeof (char));
  stress = strcat (stress, directori);
  stress = strcat (stress, STRESS_);

  if (historia_defectes)
    {
      pfile_defectes = fopen (defectes, "w");
      fprintf (stderr, "Escrivint historia_defectes a: %s\n", defectes);
    }

  if (historia_coordinacio)
    {
      pfile_coordinacio = fopen (coordinacio, "w");
      fprintf (stderr, "Escrivint historia_coordinacio a: %s\n", coordinacio);
    }
  if (historia_stress)
    {
      pfile_stress = fopen (stress, "w");
      fprintf (stderr, "Escrivint historia_stress a: %s\n", stress);
    }

  return 0;
}

int
setup_funcs (const int ARGC, char * ARGV[])
{
  int extract_funcs_n_max = 6;
  extract_funcs = (extract_function_t *) 
    malloc (extract_funcs_n_max * sizeof (extract_function_t));
  print_funcs = (print_function_t *) 
    malloc (extract_funcs_n_max * sizeof (print_function_t));

  extract_funcs_n = 0;

  extract_funcs[extract_funcs_n] = &data_dimensions_extract;
  print_funcs[extract_funcs_n] = &data_dimensions_print;
  extract_funcs_n++;

/*   extract_funcs[extract_funcs_n] = &data_stress_extract; */
/*   print_funcs[extract_funcs_n] = &data_stress_print; */
/*   extract_funcs_n++; */

  extract_funcs[extract_funcs_n] = &data_velos_extract;
  print_funcs[extract_funcs_n] = &data_velos_print;
  extract_funcs_n++;

  extract_funcs[extract_funcs_n] = &data_rug_extract;
  print_funcs[extract_funcs_n] = &data_rug_print;
  extract_funcs_n++;

  //extract_funcs[extract_funcs_n] = &data_n_extract;
  //print_funcs[extract_funcs_n] = &data_n_print;
  //extract_funcs_n++;

  extract_funcs[extract_funcs_n] = &data_stress_molecular_extract;
  print_funcs[extract_funcs_n] = &data_stress_molecular_print;
  extract_funcs_n++;

  extract_funcs[extract_funcs_n] = &data_energy_extract;
  print_funcs[extract_funcs_n] = &data_energy_print;
  extract_funcs_n++;

  fprintf (stderr, "setted up %d function(s) to extract data\n", 
	   extract_funcs_n);


  return 0;
}

int
read_eval_data (void)
{
  sys_info_t * p_sys_info = sys_info_alloc ();
  dr_reader_init (pfile_posicions, p_sys_info); //&NX, &NY, &sys_size);

  sys_info_t * p_sys_info2 = sys_info_alloc ();
  dr_reader_init (pfile_forces, p_sys_info2); // &nx, &ny, &sys_size_);

  if (!sys_info_equal (p_sys_info, p_sys_info2)) 
    not_compatible_histories ();
  else sys_info_free (p_sys_info2);

  int sys_size = p_sys_info->sys_size;
  double * sys = (double *) malloc (2 * sys_size * sizeof (double));
  double * f = (double *) malloc (2 * sys_size * sizeof (double));

  data_dimensions_alloc (pfile_data, p_sys_info); /* aixo tambe en un for?  */
/*   data_stress_alloc (pfile_data, p_sys_info); */
  data_velos_alloc (pfile_data, p_sys_info);
  data_rug_alloc (pfile_data, p_sys_info);
  data_n_alloc (pfile_data, p_sys_info);
  data_stress_molecular_alloc (pfile_data, p_sys_info);
  data_energy_alloc (pfile_data, p_sys_info);

  if (historia_defectes) data_n_set_defect_history (pfile_defectes);
  if (historia_coordinacio) 
    data_n_set_coordination_history (pfile_coordinacio);
  if (historia_stress)
    data_stress_molecular_set_stress_history (pfile_stress, p_sys_info);

  sys_info_print (p_sys_info);
  
  int snap;
  double t;
  do
    {
      dr_reader_read_frame (pfile_posicions, sys, p_sys_info, &snap, &t);
      dr_reader_read_frame (pfile_forces, f, p_sys_info, &snap, &t);
      //      fprintf (stderr, "computing snap %d\n", snap);
      int n_func;
      for (n_func = 0; n_func < extract_funcs_n; n_func++)
	{
	  extract_funcs[n_func] (p_sys_info, sys, f, snap, t);
	  print_funcs[n_func] (pfile_data, p_sys_info, snap, t);
	  fflush (pfile_data);
	  //	  fprintf (stderr, "computed func %d\n", n_func);
	}
    }
  while (1);

  fclose (pfile_posicions); pfile_posicions = 0;
  fclose (pfile_forces); pfile_forces = 0;
  fclose (pfile_data); pfile_data = 0;

  if (historia_defectes) 
    {
      fclose (pfile_defectes);
      pfile_defectes = 0;
    }
  if (historia_coordinacio) 
    {
      fclose (pfile_coordinacio);
      pfile_coordinacio = 0;
    }

  data_dimensions_free ();
/*   data_stress_free (); */
  data_velos_free ();
  data_rug_free ();
  data_n_free ();
  data_stress_molecular_free ();
  data_energy_free ();

  FREE_POINTER (sys);
  FREE_POINTER (f);

  return 0;
}

int
main_args_error (void)
{
  fprintf (stderr, "S'ha d'entrar el directori de les dades com" \
	   " a primer argument!\n");
  exit (0);
  return 0;
}

int
not_compatible_histories (void)
{
  fprintf (stderr, "Els dos fitxers historia no són compatibles.\n");
  exit (0);
  return 0;
}
