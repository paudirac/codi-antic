/* data_stress_molecular.c  */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "data_stress_molecular.h"
#include "macroutils.h"
#include "llistad.h"
#include "llistai.h"
#include "dr_writer.h"

static double * stress_tensor = 0;

static double * mean_stress;
static double * mean_stress2;
static double * mean_stress_L;
static double * mean_stress_R;
static double * mean_stress2_L;
static double * mean_stress2_R;
static double * forca_L;
static double * forca_R;

int
data_stress_molecular_alloc (FILE * pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_stress_molecular\n");
  fprintf (pfile, "#$ egrep mol data | cut -f2,3,4,5,6,7,8,9,10,11," \
	   "12,13,14,15,16,17 -d' ' > stressm.dat\n");

  int sys_size = p_sys_info->sys_size;
  int stress_tensor_size = 3 * sys_size; // xx, yy, xy = yx
  
  stress_tensor = (double *) malloc (stress_tensor_size * sizeof (double));
  if (stress_tensor == 0)
    {
      fprintf (stderr, "no s'ha pogut allocatar stress_tensor\n");
      abort ();
    }

  mean_stress = (double *) malloc (3 * sizeof (double));
  mean_stress2 = (double *) malloc (3 * sizeof (double));
  mean_stress_L = (double *) malloc (3 * sizeof (double));
  mean_stress_R = (double *) malloc (3 * sizeof (double));
  mean_stress2_L = (double *) malloc (3 * sizeof (double));
  mean_stress2_R = (double *) malloc (3 * sizeof (double));
  forca_L = (double *) malloc (2 * sizeof (double));
  forca_R = (double *) malloc (2 * sizeof (double));

  return 0;
}

int
data_stress_molecular_free (void) 
{
  FREE_POINTER (stress_tensor);
  FREE_POINTER (mean_stress);
  FREE_POINTER (mean_stress2);
  FREE_POINTER (mean_stress_L);
  FREE_POINTER (mean_stress2_L);
  FREE_POINTER (mean_stress_R);
  FREE_POINTER (mean_stress2_R);
  FREE_POINTER (forca_L);
  FREE_POINTER (forca_R);

  return 0; 
}

int historia_stress_writer (FILE * pfile, const double * sys,
			    const sys_info_t * p_sys_info);

static int historia_stress_b = FALSE;
static FILE * stress_history_pfile = 0;
int
data_stress_molecular_set_stress_history (FILE * pfile, 
					  sys_info_t * p_sys_info)
{
  if (pfile != 0) 
    {
      stress_history_pfile = pfile;
      dr_writer_set_writer (&historia_stress_writer);
      dr_writer_init (pfile, p_sys_info);
    }
  else stress_history_pfile = stderr;
  historia_stress_b = TRUE;

  return 0;
}

#define index_xx(i) (3 * (i))
#define index_xy(i) (3 * (i) + 1)
#define index_yy(i) (3 * (i) + 2)

static const double range2 = 3 * 1.122462048309373 * 3 * 1.122462048309373;
static double e_strain;

int
data_stress_molecular_extract (const sys_info_t * p_sys_info,
			       const double * sys, const double * f,
			       const int snap, const double t)
{
  mean_stress[index_xx(0)] = 0.;
  mean_stress[index_xy(0)] = 0.;
  mean_stress[index_yy(0)] = 0.;

  mean_stress2[index_xx(0)] = 0.;
  mean_stress2[index_xy(0)] = 0.;
  mean_stress2[index_yy(0)] = 0.;

  mean_stress_L[index_xx(0)] = 0.;
  mean_stress_L[index_xy(0)] = 0.;
  mean_stress_L[index_yy(0)] = 0.;

  mean_stress2_L[index_xx(0)] = 0.;
  mean_stress2_L[index_xy(0)] = 0.;
  mean_stress2_L[index_yy(0)] = 0.;

  mean_stress_R[index_xx(0)] = 0.;
  mean_stress_R[index_xy(0)] = 0.;
  mean_stress_R[index_yy(0)] = 0.;

  mean_stress2_R[index_xx(0)] = 0.;
  mean_stress2_R[index_xy(0)] = 0.;
  mean_stress2_R[index_yy(0)] = 0.;

  forca_L[index_x(0)] = 0.;
  forca_L[index_y(0)] = 0.;
  forca_R[index_x(0)] = 0.;
  forca_R[index_y(0)] = 0.;

  int sys_size = p_sys_info->sys_size;
  int i;
  for (i = 0; i < sys_size; i++) 
    {
      stress_tensor[index_xx(i)] = 0.;
      stress_tensor[index_xy(i)] = 0.;
      stress_tensor[index_yy(i)] = 0.;
    }

  for (i = 0; i < sys_size; i++)
    {
      double x_i = sys[index_x(i)];
      double y_i = sys[index_y(i)];
      
      double s_xx_i = 0.;
      double s_xy_i = 0.;
      double s_yy_i = 0.;

      int jotes = 0;
      int j;
      for (j = 0; j < sys_size; j++)
	{
	  /* avoid self  */
	  if (i == j) continue;

	  double x_j = sys[index_x(j)];
	  double y_j = sys[index_y(j)];

	  /* La i es l'origen, la f actua sobre la j.  */
	  double x_ij = x_j - x_i;
	  double y_ij = y_j - y_i;
	  double r2_ij = x_ij * x_ij + y_ij * y_ij;

	  /* cut off  */
	  if (r2_ij > range2) continue;
	  
	  double f_x_ij = 0.;
	  double f_y_ij = 0.;
	  /* caluculo la forca  entre _i_ i _j_*/
	  double over_r2 = 1./r2_ij;
	  double over_r6 = over_r2 * over_r2 * over_r2;
	  double factor;
	  factor = 24. * over_r2 * over_r6 * (2 * over_r6 - 1);

	  f_x_ij = factor * x_ij;
	  f_y_ij = factor * y_ij;

	  s_xx_i = s_xx_i + f_x_ij * x_ij;
	  s_xy_i = s_xy_i + f_x_ij * y_ij;
	  s_yy_i = s_yy_i + f_y_ij * y_ij;

	  /* valors propis  */
/* 	  double suma = 0.5 * (s_xx_i + s_yy_i); */
/* 	  double resta = 0.5 * (s_xx_i - s_yy_i); */
/* 	  double arrel = sqrt (resta * resta + s_xy_i * s_xy_i); */

/* 	  double lambda_menys = suma - arrel; */
/* 	  double lambda_mes = suma + arrel; */

	  jotes++;
	}
      double norm = 1./ ((double) jotes);
      s_xx_i = norm * s_xx_i;
      s_xy_i = norm * s_xy_i;
      s_yy_i = norm * s_yy_i;

      stress_tensor[index_xx(i)] = s_xx_i;
      stress_tensor[index_xy(i)] = s_xy_i;
      stress_tensor[index_yy(i)] = s_yy_i;

      //      fprintf (stderr, "%d %f %f %f\n", i, s_xx_i, s_xy_i, s_yy_i);
    }

  /* calculo les mitjanes, que haurien de coincidir  */
  int phys_size = p_sys_info->phys_size;
  double norm = 1./ ((double) phys_size);
  for (i = 0; i < phys_size; i++)
    {
      double s_xx_i = stress_tensor[index_xx(i)];
      double s_xy_i = stress_tensor[index_xy(i)];
      double s_yy_i = stress_tensor[index_yy(i)];

      mean_stress[index_xx(0)] = mean_stress[index_xx(0)] + norm * s_xx_i;
      mean_stress[index_xy(0)] = mean_stress[index_xy(0)] + norm * s_xy_i;
      mean_stress[index_yy(0)] = mean_stress[index_yy(0)] + norm * s_yy_i;

      mean_stress2[index_xx(0)] = mean_stress2[index_xx(0)] 
	                        + norm * s_xx_i * s_xx_i;
      mean_stress[index_xy(0)] = mean_stress[index_xy(0)] 
                               + norm * s_xy_i * s_xy_i;
      mean_stress[index_yy(0)] = mean_stress[index_yy(0)] 
                               + norm * s_yy_i * s_yy_i;
    }

  /* e_strain  **/  
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX + 1) * delta_x;//(NX - 0.5) * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  int paret_size = p_sys_info->paret_size;
  norm = 1. / ((double) paret_size);
  int NY = p_sys_info->NY;
  double delta_y = p_sys_info->delta_y;
  double norm_f = 1. / (delta_y * ((double) NY));
  //  fprintf (stdout, "norm: %f\n", norm_f);
  //forca total, no normalitzem / ((double) NY);
  /* paretL  */
  for (i = phys_size; i < phys_size + paret_size; i++)
    {
      double xx = stress_tensor[index_xx(i)];
      double xy = stress_tensor[index_xy(i)];
      double yy = stress_tensor[index_yy(i)];

      mean_stress_L[index_xx(0)] = mean_stress_L[index_xx(0)] + norm * xx;
      mean_stress_L[index_xy(0)] = mean_stress_L[index_xy(0)] + norm * xy;
      mean_stress_L[index_yy(0)] = mean_stress_L[index_yy(0)] + norm * yy;

      mean_stress2_L[index_xx(0)] = mean_stress2_L[index_xx(0)] 
	                          + norm * xx * xx;
      mean_stress2_L[index_xy(0)] = mean_stress2_L[index_xy(0)] 
                                  + norm * xy * xy;
      mean_stress2_L[index_yy(0)] = mean_stress2_L[index_yy(0)] 
                                  + norm * yy * yy;

      forca_L[index_x(0)] = forca_L[index_x(0)] + norm_f * f[index_x(i)];
      forca_L[index_y(0)] = forca_L[index_y(0)] + norm_f * f[index_y(i)];
    }

  norm = 1. / ((double) paret_size);
  //  norm_f = 1.; // forca total, no normalitzem / ((double) NY);
  /* paretR */
  for (i = phys_size + paret_size; i < phys_size + 2 * paret_size; i++)
    {
      double xx = stress_tensor[index_xx(i)];
      double xy = stress_tensor[index_xy(i)];
      double yy = stress_tensor[index_yy(i)];

      mean_stress_R[index_xx(0)] = mean_stress_R[index_xx(0)] + norm * xx;
      mean_stress_R[index_xy(0)] = mean_stress_R[index_xy(0)] + norm * xy;
      mean_stress_R[index_yy(0)] = mean_stress_R[index_yy(0)] + norm * yy;

      mean_stress2_R[index_xx(0)] = mean_stress2_R[index_xx(0)] 
	                          + norm * xx * xx;
      mean_stress2_R[index_xy(0)] = mean_stress2_R[index_xy(0)] 
                                  + norm * xy * xy;
      mean_stress2_R[index_yy(0)] = mean_stress2_R[index_yy(0)] 
                                  + norm * yy * yy;

      forca_R[index_x(0)] = forca_R[index_x(0)] + norm_f * f[index_x(i)];
      forca_R[index_y(0)] = forca_R[index_y(0)] + norm_f * f[index_y(i)];
    }

  if (historia_stress_b) 
    dr_writer_write_frame (stress_history_pfile, sys,
			   p_sys_info, snap, t);

  return 0;
}

int
historia_stress_writer (FILE * pfile, const double * sys,
			const sys_info_t * p_sys_info)
     /* implementacio de writer_funcp_t  **/
{
  static const char * new_line = "\n";
  static const char * particle_delim = ":";

  /* historia stressos:       i    x    y   xx   xy   yy  **/
  const char * format_file = "%d %.5e %.5e %.5e %.5e %.5e";
  int phys_size = p_sys_info->phys_size;
  int i;
  fprintf (pfile, particle_delim);
  for (i = 0; i < phys_size; i++)
    {
      fprintf (pfile, format_file,
	       i,                           // 1. i
	       sys[index_x(i)],             // 2. x
	       sys[index_y(i)],             // 3. y
	       stress_tensor[index_xx(i)] - mean_stress[index_xx(0)],  // 4. xx
	       stress_tensor[index_xy(i)] - mean_stress[index_xy(0)],  // 5. xy
	       stress_tensor[index_yy(i)] - mean_stress[index_yy(0)]); // 6. yy
      fprintf (pfile, particle_delim);
    }
  fprintf (pfile, new_line);  
  
  return 0;
}

int
data_stress_molecular_print (FILE * pfile, const sys_info_t * p_sys_info,
			     const int snap, const double t)
{
/*1    2    3   4  5  6   7   8   9  10  11  12  13  14  15  16  */
/*snap t e_str xx xy yy Lxx Lxy Lyy Rxx Rxy Ryy fxL fyL fxR fyR  */

  fprintf (pfile, "mol " 
	   "%d %.5e %.5e " 
	   "%.5e %.5e %.5e "  // mean
	   "%.5e %.5e %.5e "  // L
	   "%.5e %.5e %.5e "  // R
	   "%.5e %.5e %.5e %.5e\n", // forces

	   snap, t, e_strain,
 
	   mean_stress[index_xx(0)],
	   mean_stress[index_xy(0)], 
	   mean_stress[index_yy(0)],

	   mean_stress_L[index_xx(0)],
	   mean_stress_L[index_xy(0)],
	   mean_stress_L[index_yy(0)],

	   mean_stress_R[index_xx(0)],
	   mean_stress_R[index_xy(0)],
	   mean_stress_R[index_yy(0)],

	   forca_L[index_x(0)], forca_L[index_y(0)],
	   forca_R[index_x(0)], forca_R[index_y(0)]);

  return 0;
}
