/* data_stress.c  */

/*
 * Falta veure com s'implementa lo d'agafar sempre el mateix nombre de
 * particules. Oi? Fa falta posar-hi el nombre de particules extra, a
 * la info? Sembla que si.
 */

/* Ara crec que l'stress està més o menys ben calculat. Bé, fallen les parets.
 * Segueixo el Doi, M. $ Edwards, S.F., The Theory of Polymer Dynamics
 * Oxford Science Publications.  */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_histogram.h>
#include "data_stress.h"
#include "macroutils.h"
#include "llistad.h"
#include "llistai.h"

int
data_stress_alloc (FILE * pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_stress\n");
  fprintf (pfile, "# $ egrep str data | cut -f2,3,4,5,6,7,8,9,10,11," \
	   "12,13,14,15,16,17 -d' ' > stress.dat\n");

  return 0;
}

int
stress_free (void) { return 0; }

static double data_stress_s_xx;
static double data_stress_s_xy;
static double data_stress_s_yy;

static double data_stress_s_xx2;
static double data_stress_s_xy2;
static double data_stress_s_yy2;

static double forca_paretL_x;
static double forca_paretL_y;
static double forca_paretR_x;
static double forca_paretR_y;

static double e_strain;

static llistai_t * index_L;
static llistai_t * index_R;

int compleix_condicio_L (const sys_info_t * p_sys_info, 
			 const double * sys, const int i);
int compleix_condicio_R (const sys_info_t * p_sys_info, 
			 const double * sys, const int i);

int
data_stress_extract (const sys_info_t * p_sys_info, const double * sys,
		const double * f, const int snap, const double t)
     /* El calculo com arXiv:cond-mat/0111312
      * Berthier, L. & Barrat, J.L.  */
{
  data_stress_s_xx = 0.;
  data_stress_s_xy = 0.;
  data_stress_s_yy = 0.;

  data_stress_s_xx2 = 0.;
  data_stress_s_xy2 = 0.;
  data_stress_s_yy2 = 0.;

  int phys_size = p_sys_info->phys_size;
  double norm = 1./ ((double) phys_size);

  double s_xx = 0.;
  double s_xy = 0.;
  double s_yy = 0.;

  double s_xx2 = 0.;
  double s_xy2 = 0.;
  double s_yy2 = 0.;

  int i;
  for (i = 0; i < phys_size; i++)
    {
      double r_x = sys[index_x(i)];
      double r_y = sys[index_y(i)];
      double f_x = f[index_x(i)];
      double f_y = f[index_y(i)];

      s_xx = s_xx - norm * r_x * f_x;
      s_xy = s_xy - norm * 0.5 * (r_x * f_y + r_y * f_x);
      s_yy = s_yy - norm * r_y * f_y;

      s_xx2 = s_xx2 + norm * r_x * f_x * r_x * f_x;
      s_xy2 = s_xy2 + norm * 0.5 * (r_x * f_y + r_y * f_x)
	                   * 0.5 * (r_x * f_y + r_y * f_x);
      s_yy2 = s_yy2 + norm * r_y * f_y * r_y * f_y;
    }

  data_stress_s_xx = s_xx;
  data_stress_s_xy = s_xy;
  data_stress_s_yy = s_yy;

  data_stress_s_xx2 = s_xx2;
  data_stress_s_xy2 = s_xy2;
  data_stress_s_yy2 = s_yy2;

  int sys_size = p_sys_info->sys_size;
  if (snap == 0)
    {
      index_L = llistai_alloc_initial_length (p_sys_info->paret_size);
      index_R = llistai_alloc_initial_length (p_sys_info->paret_size);
      int i;
      for (i = phys_size; i < sys_size; i++)
	{
	  if (compleix_condicio_L (p_sys_info, sys, i)) 
	    llistai_add (index_L, i);
	  if (compleix_condicio_R (p_sys_info, sys, i)) 
	    llistai_add (index_R, i);
	}
    }

  if (snap == 0)
    {
      llistai_print (index_L, 0);
      llistai_print (index_R, 0);
      fprintf (stderr, "%d %d\n", 
	       llistai_length (index_L), 
	       llistai_length (index_R));
    }

  forca_paretL_x = 0.;
  forca_paretL_y = 0.;
  int n_particules_L = llistai_length (index_L);
  int * indexos_l = llistai_get_ints (index_L);
  for (i = 0; i < n_particules_L; i++)
    {
      int index = indexos_l[i];
      forca_paretL_x = forca_paretL_x + f[index_x(index)];
      forca_paretL_y = forca_paretL_y + f[index_y(index)];
    }
  
  forca_paretR_x = 0.;
  forca_paretR_y = 0.;
  int n_particules_R = llistai_length (index_R);
  int * indexos_r = llistai_get_ints (index_R);
  for (i = 0; i < n_particules_R; i++)
    {
      int index = indexos_r[i];
      forca_paretR_x = forca_paretR_x + f[index_x(index)];
      forca_paretR_y = forca_paretR_y + f[index_y(index)];
    }

  /* e_strain  **/  
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX - 0.5) * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  if (n_particules_L != 0)
    {
      forca_paretL_x = forca_paretL_x / ((double) n_particules_L);
      forca_paretL_y = forca_paretL_y / ((double) n_particules_L);
    }
  if (n_particules_R != 0)
    {
      forca_paretR_x = forca_paretR_x / ((double) n_particules_R);
      forca_paretR_y = forca_paretR_y / ((double) n_particules_R);
    }

  return 0;
}

int
data_stress_print (FILE *pfile, const sys_info_t * p_sys_info, 
		   const int snap, const double t)
{
  fprintf (pfile, "str %d %.5e %.5e %.5e %.5e %.5e " \
	   "%.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e\n",
	   snap, t, e_strain, data_stress_s_xx,
/*           1   2    3           4          */
	   data_stress_s_xy, data_stress_s_yy,
/*               5                 6         */
	   fabs (data_stress_s_xx), fabs (data_stress_s_xy),
/*               7                        8  */
	   fabs (data_stress_s_yy), 
/*                 9                         */
	   data_stress_s_xx2, data_stress_s_xy2, data_stress_s_yy2,
/*                  10               11               12  */
	   forca_paretL_x, forca_paretL_y, forca_paretR_x, forca_paretR_y);
/*               13           14                 15           16    */

  return 0;
}

static double PERCENT = 110.;

int 
compleix_condicio_L (const sys_info_t * p_sys_info, 
		     const double * sys, const int i)
{
  const int paret_size = p_sys_info->paret_size;
  const int phys_size = p_sys_info->phys_size;
  const double y_min_geom = 0.;
  const int NY = p_sys_info->NY;
  const double delta_y = p_sys_info->delta_y;
  const double y_max_geom = NY * delta_y;

  double y_mid = 0.5 * (y_max_geom - y_min_geom);
  double m = (y_max_geom - y_min_geom) / 100.;
  double y_min = y_mid - 0.5 * PERCENT * m;
  double y_max = y_mid + 0.5 * PERCENT * m;

  if ((i < phys_size) || (i >= phys_size + paret_size)) return 0;
  
  double y = sys[index_y(i)];
  if (( y > y_min + delta_y) && (y < y_max - delta_y)) return 1;

  return 0;
}
/*   const int ny_paret = p_sys_info->NY + 2 * p_sys_info->ny_extra; */
/*   const int extras = p_sys_info->ny_extra + 2; */
/*   const int nx_paret = paret_size / ny_paret; */
/*   int index = i - phys_size; */
/*   if ((index % nx_paret == 1) && (index > extras) &&  */
/*       ( index < paret_size - extras)) return 1; */


int 
compleix_condicio_R (const sys_info_t * p_sys_info, 
		     const double * sys, const int i)
{
  const int paret_size = p_sys_info->paret_size;
  const int phys_size = p_sys_info->phys_size;
  const double y_min_geom = 0.;
  const int NY = p_sys_info->NY;
  const double delta_y = p_sys_info->delta_y;
  const double y_max_geom = NY * delta_y;

  double y_mid = 0.5 * (y_max_geom - y_min_geom);
  double m = (y_max_geom - y_min_geom) / 100.;
  double y_min = y_mid - 0.5 * PERCENT * m;
  double y_max = y_mid + 0.5 * PERCENT * m;

  if (i < phys_size + paret_size) return 0;

  double y = sys[index_y(i)];
  if (( y > y_min + delta_y) && (y < y_max - delta_y)) return 1;

  return 0;
}
/*   const int ny_paret = p_sys_info->NY + 2 * p_sys_info->ny_extra; */
/*   const int extras = p_sys_info->ny_extra + 2; */
/*   const int nx_paret = paret_size / ny_paret; */
/*   int index = i - (phys_size + paret_size); */
/*   if ((index % nx_paret == 0) && (index > extras) &&  */
/*       ( index < paret_size - extras)) return 1; */


/* int */
/* contact_areas (double * areaL, double * areaR) */
/* { */
/*   double x_phys_min = dim->x_phys_min; */
/*   double x_phys_max = dim->x_phys_max; */
/*   double delta = 0.5 * delta_x; */

/*   llistad_t * llistaL = llistad_alloc (); */
/*   llistad_t * llistaR = llistad_alloc (); */

/*   int i; */
/*   for (i = 0; i < phys_size; i++) */
/*     { */
/*       double x = sys[i]; */
/*       double y = sys[index_y(i)]; */
/*       if (x < x_phys_min + delta) llistad_add (llistaL, y); */
/*       if (x_phys_max - delta < x) llistad_add (llistaR, y); */
/*     } */
/*   llistad_qsort (llistaL, 0); */
/*   llistad_qsort (llistaR, 0); */

/*   *areaL = llistad_max (llistaL) - llistad_min (llistaL); */
/*   *areaR = llistad_max (llistaR) - llistad_min (llistaR); */

/*   llistad_free (llistaL); */
/*   llistad_free (llistaR); */
  
/*   return 0; */
/* } */
