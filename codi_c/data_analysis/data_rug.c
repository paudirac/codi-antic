/** data_rug.c  */
#include <stdio.h>
#include <math.h>
#include "data_rug.h"
#include "dr_reader.h"
#include "macroutils.h"
#include "llistad.h"
#include "llistai.h"

int
data_rug_alloc (FILE * pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_rug\n");
  fprintf (pfile,
	   "# $ egrep rug data | cut -f2,3,4,5,6,7,8,9 -d' ' > rug.dat\n");

  return 0;
}

int data_rug_free (void) { return 0; }

static int n_ys = 0;
static double h_ymaxs;
static double h_ymins;
static double h2_ymaxs;
static double h2_ymins;
static double w_ymaxs;
static double w_ymins;
static double e_strain;
static int index_y_max = -1;

int
data_rug_extract (const sys_info_t * p_sys_info, const double * sys,
		  const double * f, const int snap, const double t)
{

  int NY = p_sys_info->NY;
  if (snap == 0) n_ys = NY / 5;
  h_ymaxs = 0.;
  h_ymins = 0.;
  h2_ymaxs = 0.;
  h2_ymins = 0.;
  w_ymaxs = 0.;
  w_ymins = 0.;
  index_y_max = -1;

  int phys_size = p_sys_info->phys_size;
  llistad_t * ys = llistad_alloc_initial_length (phys_size);
  llistai_t * indexs_ys = llistai_alloc_initial_length (phys_size);

  int i;
  for (i = 0; i < phys_size; i++)
    {
      double y = sys[index_y(i)];
      llistad_add (ys, y);
      llistai_add (indexs_ys, i);
    }

  llistad_qsort (ys, 0);
  double * ys_ = llistad_get_doubles (ys);

  auto int ordena_llistai_amb_ys_de_sys (const void * a, const void * b)
    /* La closure utilitza sys, que esta a l'enclosing scope. Jaja!  **/
    {
      const int * pia = (const int *) a;
      const int * pib = (const int *) b;
      int ia = * pia;
      int ib = * pib;
      return ((sys[index_y(ia)] > sys[index_y(ib)]) -
	      (sys[index_y(ia)] < sys[index_y(ib)]));
    }

  llistai_qsort (indexs_ys, &ordena_llistai_amb_ys_de_sys);

  int length = llistad_length (ys);
  for (i = 0; i < n_ys; i++)
    {
      double y_min_i = ys_[i];
      double y_max_i = ys_[length - i - 1]; // important el -1!!!
      h_ymins = h_ymins + y_min_i;
      h2_ymins = h2_ymins + y_min_i * y_min_i;
      h_ymaxs = h_ymaxs + y_max_i;
      h2_ymaxs = h2_ymaxs + y_max_i * y_max_i;
    }
  h_ymaxs = h_ymaxs / ((double) n_ys);
  h2_ymaxs = h2_ymaxs / ((double) n_ys);
  h_ymins = h_ymins / ((double) n_ys);
  h2_ymins = h2_ymins / ((double) n_ys);

  w_ymaxs = sqrt (h2_ymaxs - h_ymaxs * h_ymaxs);
  w_ymins = sqrt (h2_ymins - h_ymins * h_ymins);

  /* e_strain  **/  
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX + 1) * delta_x;//(NX - 0.5) * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  index_y_max = llistai_max (indexs_ys);
/*   fprintf (stderr, "%d %d %d %f %f %f %f\n", NY, snap, index_y_max, */
/* 	   sys[index_y(index_y_max)], llistad_max (ys), h_ymaxs,  */
/* 	   ys_[length - 1]); */

  llistad_free (ys);
  llistai_free (indexs_ys);

  return 0;
}

int
data_rug_print (FILE *pfile, const sys_info_t * p_sys_info,
		const int snap, const double t)
{
  /*  snap t e_strain w_ymaxs w_ymins h_ymaxs h_ymins  index_y_max  */
  /*   1   2    3        4       5       6       7         8        */
  const char * format = "rug %d %.5e %.5e %.5e %.5e %.5e %.5e %d\n";

  fprintf(pfile, format, 
	  snap, t, e_strain, w_ymaxs, w_ymins, h_ymaxs, h_ymins, index_y_max);

  return 0;
}
