/* data_velos.c  */
#include <math.h>
#include <gsl/gsl_histogram.h>
#include "data_velos.h"
#include "macroutils.h"


/* NOTA:
 * Esborrar els velos_0? No fan falta per a res, oi?
 * Només fan que xupar memòria.
 */

//gsl_histogram * h = 0;

static double * velos_0 = 0;

int
data_velos_alloc (FILE *pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_velos\n");
  fprintf (pfile, "# $ egrep vel data | cut -f2,3,4,5,6,7,"  \
	   "8,9,10,11,12 -d' ' > velos.dat\n");
  
  int phys_size = p_sys_info->phys_size;
  velos_0 = (double *) malloc (phys_size * sizeof (double));

  return 0;
}

int data_velos_free (void)
{ 
  FREE_POINTER (velos_0);

  return 0; 
}

static double velos_vx;
static double velos_vx2;
static double velos_vy;
static double velos_vy2;
static double velos_vxy;
static double velos_vxy2;
static double velos_v;
static double velos_v2;
static double correl_velos;

static double e_strain;

const double SQRT_2 = 1.4142135623730951;
static const double range2 = 3 * 1.122462048309373 * 3 * 1.122462048309373;

int
data_velos_extract (const sys_info_t * p_sys_info, const double * sys,
		    const double * f, const int snap, const double t)
{
  velos_vx = 0.;
  velos_vx2 = 0.;
  velos_vy = 0.;
  velos_vy2 = 0.;
  velos_vxy = 0.;
  velos_vxy2 = 0.;
  velos_v = 0.;
  velos_v2 = 0.;
  correl_velos = 0.;

  /* Calculo les velocitas  */
/*   int sys_size = p_sys_info->sys_size; */
/*   double * velos_ = (double *) malloc (2 * sys_size * sizeof (double)); */
/*   int m; */
/*   for (m = 0; m < sys_size; m++) */
/*     { */
/*       double x_m = sys[index_x(m)]; */
/*       double y_m = sys[index_y(m)]; */
      
/*       int n; */
/*       for (n = 0; n < m; n++) */
/* 	{ */
/* 	  /\* avoid self  *\/ */
/* 	  if (n == m) continue; */

/* 	  double x_n = sys[index_x(n)]; */
/* 	  double y_n = sys[index_y(n)]; */
	  
/* 	  double x_mn = x_n - x_m; */
/* 	  double y_mn = y_n - y_m; */
/* 	  double r2_mn = x_mn * x_mn + y_mn * y_mn; */

/* 	  if (r2_mn > range2) continue; */

/* 	  double f_x_mn = 0.; */
/* 	  double f_y_mn = 0.; */
/* 	  /\* forca entre _m_ i _n_  *\/ */
/* 	  double over_r2 = 1. / r2_mn; */
/* 	  double over_r6 = over_r2 * over_r2 * over_r2; */
/* 	  double factor; */
/* 	  factor = 24. * over_r2 * over_r6 * (2 * over_r6 - 1); */

/* 	  f_x_mn = factor * x_mn; */
/* 	  f_y_mn = factor * y_mn; */

/* 	  velos_[index_x(m)] = velos_[index_x(m)] + f_x_mn; */
/* 	  velos_[index_y(m)] = velos_[index_y(m)] + f_x_mn; */

/* 	  velos_[index_x(n)] = velos_[index_x(n)] - f_x_mn; */
/* 	  velos_[index_y(n)] = velos_[index_x(n)] - f_y_mn; */
/* 	} */
/*     } */

  int phys_size = p_sys_info->phys_size;
  double norm = 1. / ((double) phys_size);
  double norm_correl = 1. / ((double) phys_size);
  int i;
  for (i = 0; i < phys_size; i++)
    {
      double vx = f[index_x(i)];
      double vy = f[index_y(i)];

/*       double vx = velos_[index_x(i)]; */
/*       double vy = velos_[index_y(i)]; */

      double v = sqrt (vx * vx + vy * vy);
      double vxy = SQRT_2 * (vx + vy);

      if (snap == 0) velos_0[i] = v;
      else
	correl_velos = correl_velos + norm_correl * v * velos_0[i];

      velos_vx = velos_vx + norm * vx;
      velos_vx2 = velos_vx2 + norm * vx * vx;
      velos_vy = velos_vy + norm * vy;
      velos_vy2 = velos_vy2 + norm * vy * vy;
      velos_vxy = velos_vxy + norm * vxy;
      velos_vxy2 = velos_vxy2 + norm * vxy * vxy;
      velos_v = velos_v + norm * v;
      velos_v2 = velos_v2 + norm * v * v;
    }

  /* e_strain  **/  
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX + 1) * delta_x;//(NX - 0.5) * delta_x;
  //  x_length_0 = NX * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  return 0;
}

int
data_velos_print (FILE *pfile, const sys_info_t * p_sys_info,
		  const int snap, const double t)
{
  const char * format_velos = 
    "vel %d %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e\n";
    /*    1   2    3    4    5    6   7    8    9    10   11 **/
  if (pfile == 0) pfile = stderr;

  fprintf (pfile, format_velos,
	   snap, t, e_strain, velos_vx, velos_vx2, velos_vy, velos_vy2,
/*          1    2    3         4          5          6         7      */
	   velos_vxy, velos_vxy2, velos_v, velos_v2, correl_velos);
/*             8         9          10       11                        */

  return 0;
}
