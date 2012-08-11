/** data_dimensions.c  */
#include <stdio.h>
#include <data_dimensions.h>
#include "data_dimensions.h"
#include "dr_reader.h"
#include "llistad.h"

static double x_min = 0.;
static double x_max = 0.;
static double y_min = 0.;
static double y_max = 0.;

static double height = 0.;
static double width = 0.; /** que és com l'e_strain  */

static double e_strain = 0.;

int 
data_dimensions_alloc (FILE * pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_dimensions\n");
  fprintf (pfile, "# $ egrep dim data | cut -f2,3,4,5,6,7,8,9,10 -d' '" \
	   " > dimensions.dat\n");
  return 0;
}

int data_dimensions_free (void) { return 0; }

int 
data_dimensions_extract (const sys_info_t * p_sys_info, const double * sys,
			 const double * f, const int snap, const double t)
{

  llistad_t * xs = llistad_alloc ();
  llistad_t * ys = llistad_alloc ();

  const int phys_size = p_sys_info->phys_size;
  int i;
  for (i = 0; i < phys_size; i++)
    {
      double x = sys[index_x(i)];
      double y = sys[index_y(i)];
      llistad_add (xs, x);
      llistad_add (ys, y);
    }

  x_min = llistad_min (xs);
  x_max = llistad_max (xs);
  y_min = llistad_min (ys);
  y_max = llistad_max (ys);

  height = y_max - y_min;
  width = x_max - x_min;

  /* e_strain  **/  
  double V = p_sys_info->V;
  double delta_x = p_sys_info->delta_x;
  int NX = p_sys_info->NX;
  double x_length_0 = (NX + 1) * delta_x;//(NX - 0.5) * delta_x;
  double vt = 2 * V * t;
  e_strain = vt / x_length_0;

  llistad_free (xs);
  llistad_free (ys);

  return 0;
}

int 
data_dimensions_print (FILE * pfile, const sys_info_t * p_sys_info, 
		       const int snap, const double t)
{
  /**  1     2     3     4    5     6      7   8      9     */
  /** snap t e_strain x_min x_max y_min y_max width height  */
  const char * dim_format = "dim %d %.5e %.5e %.5e %.5e %.5e %.5e %.5e %.5e\n";
  fprintf (pfile, dim_format,
	   snap, t, e_strain, x_min, x_max, y_min, y_max, width, height);

  return 0;
}
