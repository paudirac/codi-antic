/* data_energy.c  **/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "data_energy.h"
#include "macroutils.h"

int
data_energy_alloc (FILE *pfile, const sys_info_t * p_sys_info)
{
  fprintf(pfile,"# $ egrep ene data | cut -f2,3,4,5,6,7,8,9,10 -d' ' " \
	  "> ene.dat,\n");

  return 0;
}

int
data_energy_free (void)
{
  return 0;
}

static double energy_energy;
static double energy_energy2;
static double dW_per_particle;
static double e_strain;
static double dW;
static double sum_modul_per_particle;
static double sum_modul;

int
data_energy_extract (const sys_info_t * p_sys_info,
		     const double * sys, const double * f,
		     const int snap, const double t)
{
  energy_energy = 0.;
  energy_energy2 = 0.;
  dW_per_particle = 0.;
  dW = 0.;
  sum_modul_per_particle = 0.;
  sum_modul = 0.;

  int phys_size = p_sys_info->phys_size;
  double norm = 1. / ((double) phys_size);
  int i;
  int sys_size = p_sys_info->sys_size;
  for (i = 0; i < phys_size; i++)
  //for (i = 0; i < sys_size; i++)
    {
      double x_i = sys[index_x(i)];
      double y_i = sys[index_y(i)];

      double e_i = 0.;

      double fx_i = f[index_x(i)];
      double fy_i = f[index_y(i)];
      double vx_i = f[index_x(i)];
      double vy_i = f[index_y(i)];
      double dW_i = fx_i * vx_i + fy_i * vy_i; // v and f are the same, but...
      double mod_v_i = sqrt(vx_i * vx_i + vy_i * vy_i);

      int j;
      //for (j = 0; j < phys_size; j++)
      for (j = 0; j < sys_size; j++)
	{
	  /* avoid self  */
	  if (j == i) continue;

	  double x_j = sys[index_x(j)];
	  double y_j = sys[index_y(j)];
	  double r2 = (x_j - x_i) * (x_j - x_i) + (y_j - y_i) * (y_j - y_i);
	  double over_r2 = 1. / r2;
	  double over_r6 = over_r2 * over_r2 * over_r2;
	  double over_r12 = over_r6 * over_r6;
	  double e_ij = 4 * (over_r12 - over_r6);
	  e_i = e_i + e_ij;
	}

      // el 0.5 \sum_{i<j} = 0.5 \sum_{ij} !!!!!!!!!!!!!!
      energy_energy = energy_energy + 0.5 * e_i;
      energy_energy2 = energy_energy2 + 0.5 * e_i * e_i; //??? el 0.5 no ho sé
      dW_per_particle = dW_per_particle + norm * dW_i;
      dW = dW + dW_i;
      sum_modul_per_particle = sum_modul_per_particle + norm * mod_v_i;
      sum_modul = sum_modul + mod_v_i;
    }

  /* e_strain  **/
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX + 1) * delta_x;//(NX - 0.5) * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  return 0;
}

int
data_energy_print (FILE *pfile, const sys_info_t * p_sys_info,
		   const int snap, const double t)
{
  if (pfile == 0)
    pfile = stderr;
  fprintf (pfile, "ene %d %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e\n",
	   snap, t, e_strain, energy_energy, energy_energy2, dW_per_particle,
/*          1    2      3          4                5         6  */
	   dW, sum_modul_per_particle, sum_modul);
  /*        7     8                       9   */

  return 0;
}
