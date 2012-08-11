/** data_volums.c  */
#include <stdio.h>
#include <math.h>
#include <triangle.h>
#include <data_volums.h>
#include <globals.h>
#include <sistema.h>
#include <data_n.h>

int
data_volums_alloc (FILE * pfile)
{
  fprintf (pfile, "# $ egrep area data | cut -f2,3,4,5,6 -d' ' > area.dat\n");

  return 0;
}

int data_volums_free (void) { return 0; }

static double area = 0;
static double area2 = 0;

int 
data_volums_extract (void)
{
  area = 0;
  area2 = 0;

  struct triangulateio * in = data_n_get_triangulateio_in ();
  struct triangulateio * out = data_n_get_triangulateio_out ();
  int * coordinacio = data_n_get_coordinacio ();

  int n_triangles_phys = 0;
  int tri;
  for (tri = 0; tri < out->numberoftriangles; tri++)
    {
      int vertex_1 = out->trianglelist[3 * tri];
      int vertex_2 = out->trianglelist[3 * tri + 1];
      int vertex_3 = out->trianglelist[3 * tri + 2];

      if (vertex_1 > phys_size)	continue;
      if (vertex_2 > phys_size)	continue;
      if (vertex_3 > phys_size)	continue;
      /** En aquest punt no hi ha cap vertex de les parets.  */

      n_triangles_phys = n_triangles_phys + 1;

      double x1 = (double) in->pointlist[2 * vertex_1];
      double y1 = (double) in->pointlist[2 * vertex_1 + 1];

      double x2 = (double) in->pointlist[2 * vertex_2];
      double y2 = (double) in->pointlist[2 * vertex_2 + 1];

      double x3 = (double) in->pointlist[2 * vertex_3];
      double y3 = (double) in->pointlist[2 * vertex_3 + 1];

      double b1 = x2 - x1;
      double b2 = y2 - y1;

      double lambda = ((x3 - x1) * b1 + (y3 - y1) * b2
		       ) / (b1 * b1 + b2 * b2); 
	
      double xi = lambda * b1 + x1;
      double yi = lambda * b2 + y1;

      double h1 = x3 - xi;
      double h2 = y3 - yi;
      
      double area_tri = sqrt ((h1 * h1 + h2 * h2) * (b1 * b1 + b2 * b2)) / 2.;
      
      area = area + area_tri;
      area2 = area2 + area_tri * area_tri;
    }

  area = area / ((double) n_triangles_phys);
  area2 = area2 / ((double) n_triangles_phys);

  fprintf (stderr, "n_triangles_phys = %d\n", n_triangles_phys);

  return 0;
}

int 
data_volums_print (FILE * pfile, const int snap)
{
  /** snap t e_strain area area2  */
  /**  1   2     3      4    5    */
  char * format_area = "area %d %.5e %.5e %.5e %.5e\n";

  double e_strain = sys_engineering_strain ();
  if (pfile == 0)
    pfile = stderr;

  fprintf (pfile, format_area, 
	   snap, t, e_strain, area, area2);

  return 0;
}
