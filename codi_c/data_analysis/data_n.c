/* data_n.c  */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_histogram.h>
#include "data_n.h"
#include "dr_reader.h"
#include "llistad.h"
#include "triangle.h"
#include "macroutils.h"

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

void *
xmalloc (size_t size)
{
  register void *value = malloc (size);
  if (value == 0)
    {
      fprintf (stderr, "error d'allocatament\n");
      abort ();
    }
  return value;
}

static int * coordinacio = 0;
static REAL * in_pointlist = NULL;

static char * triflags = "zqenBPcQ";
struct triangulateio * in;
struct triangulateio * out;
/* struct triangulateio * vorout; */

int data_triangle_alloc (const sys_info_t * p_sys_info, const double * sys);
int data_triangle_run_triangle (void);
int data_triangle_extract_triangles_returned_data (const sys_info_t * 
						   p_sys_info);
int data_triangle_free (void);
int data_n_print_stuff (FILE *pfile, const sys_info_t * p_sys_info, 
			const int snap, const double t);

int detect_and_print_dislocations (const sys_info_t * p_sys_info,
				   const int snap, const double t);

static int historia_defectes = 0;
static FILE * pfile_historia_defectes = 0;

int
data_n_set_defect_history (FILE * pfile)
{
  historia_defectes = 1;
  pfile_historia_defectes = pfile;

  return 0;
}

static int historia_coordinacio = 0;
static FILE * pfile_historia_coordinacio = 0;

int
data_n_set_coordination_history (FILE * pfile)
{
  historia_coordinacio = 1;
  pfile_historia_coordinacio = pfile;

  return 0;
} 

int
data_n_alloc (FILE *pfile, const sys_info_t * p_sys_info)
{
  fprintf (stderr, "allocated data_n\n");

  fprintf (pfile, "# $ egrep n5n6n7 data | " \
	   "cut -f2,3,4,5,6,7,8,9,10,11,12,13,14,15" \
	   " -d' ' > n5n6n7.dat\n");

  int phys_size = p_sys_info->phys_size;
  coordinacio = (int *) malloc (phys_size * sizeof (int));
  if (coordinacio == 0)
    {
      fprintf (stderr, "No hi ha memòria. Canviar això.\n");
      abort ();
    }

  int sys_size = p_sys_info->sys_size;
  in_pointlist = (REAL *) malloc ((sys_size + 2) * sizeof (REAL));
  if (in_pointlist == 0)
    {
      fprintf (stderr, "No hi ha memòria. Canviar això.\n");
      abort ();
    }

  return 0;
}

int
data_n_free (void)
{
  FREE_POINTER (coordinacio);
  FREE_POINTER (in_pointlist);
  return 0;
}

static double e_strain = 0.;

int
data_n_extract (const sys_info_t * p_sys_info, const double * sys,
		const double * f, const int snap, const double t)
{
  data_triangle_alloc (p_sys_info, sys);
  data_triangle_run_triangle ();
  data_triangle_extract_triangles_returned_data (p_sys_info);
/*   data_triangle_free (); */

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
data_n_print (FILE *pfile, const sys_info_t * p_sys_info, 
	      const int snap, const double t)
{
  data_n_print_stuff (pfile, p_sys_info, snap, t);
  data_triangle_free ();
  return 0;
}

int print_historia_coordinacio_header (const sys_info_t * p_sys_info,
				       const int snap, const double t)
{
  const char * format_header = "%d %.5e\n";
  fprintf (pfile_historia_coordinacio, format_header, snap, t);
  fprintf (pfile_historia_coordinacio, ":");
  return 0;
}

int print_historia_coordinacio_particle (const double x, const double y,
					 const int coord_i)
{
  static const char * format_coordinacio = "%.5e %.5e %d";
  static const char * particle_delim = ":";
  fprintf (pfile_historia_coordinacio, format_coordinacio,
	   x, y, coord_i);
  fprintf (pfile_historia_coordinacio, particle_delim);

  return 0;
}

int print_historia_coordinacio_final_frame (void)
{
  fprintf (pfile_historia_coordinacio, ":\n");
  return 0;
}

int
data_n_print_stuff (FILE *pfile, const sys_info_t * p_sys_info, 
		    const int snap, const double t)
{
  if (historia_coordinacio) 
    print_historia_coordinacio_header (p_sys_info, snap, t);

  int n3,n4,n5,n6,n7,n8,n9;
  n3 = n4 = n5 = n6 = n7 = n8 = n9 = 0;
  int n_dislos = detect_and_print_dislocations (p_sys_info, snap, t);

  const int phys_size = p_sys_info->phys_size;
  const int NX = p_sys_info->NX;
  const int NY = p_sys_info->NY;
  int i;
  for (i = 0; i < phys_size; i++)
    {
      int coord_i = coordinacio[i];
      int fila = i / NX;

      /** Soluciono fluctuacions en els n's  */
      /** Amb això pretenc treure les dislocacions falses (de les parets).  */
      if (fila == 0)
	coord_i = 69;
      else if (fila == NY - 1)
	coord_i = 69;
      /** Dislos falses tretes.  */

      switch (coord_i)
	{
	case 3: n3++; break;
	case 4: n4++; break;
	case 5: n5++; break;
	case 6: n6++; break;
	case 7: n7++; break;
	case 8: n8++; break;
	case 9: n9++; break;
	default: break;
	}

      if (historia_coordinacio)
	{
	  double x = out->pointlist[index_x(i)];
	  double y = out->pointlist[index_y(i)];
	  print_historia_coordinacio_particle (x, y, coord_i);
	}
    }

  if (historia_coordinacio) 
    print_historia_coordinacio_final_frame ();

  int n_tot = n3 + n4 + n5 + n6 + n7 + n8 + n9; // n'hi ha prou amb n4
  fprintf (pfile, "n5n6n7 %d %.5e %.5e %d %d %d %d %d %d %d %d %d %d\n",
	   snap, t, e_strain, n4, n5, n6, n7, n8, n9, n3,
/*           1   2     3       4   5   6   7   8   9  10  */
	   n_tot, phys_size, n_dislos);
/*          11       12        13                         */

  return 0;
}

int
data_triangle_alloc (const sys_info_t * p_sys_info, const double * sys)
{
  int sys_size = p_sys_info->sys_size;
  int in_numberofpoints = sys_size + 2;

  /* Preparo in  */
  in = (struct triangulateio *) xmalloc (sizeof (struct triangulateio));
  in->numberofpoints = in_numberofpoints;
  in->pointlist = in_pointlist;
  in->pointattributelist = (REAL *) NULL;
  in->numberofpointattributes = 0;
  in->pointmarkerlist = (int *) NULL;
  in->trianglelist = (int *) NULL;
  in->triangleattributelist = (REAL *) NULL;
  in->trianglearealist = (REAL *) NULL;
  in->neighborlist = (int *) NULL;
  in->segmentlist = (int *) NULL;
  in->segmentmarkerlist = (int *) NULL;
  in->holelist = (REAL *) NULL;
  in->regionlist = (REAL *) NULL;
  in->edgelist = (int *) NULL;
  in->edgemarkerlist = (int *) NULL;
  in->normlist = (REAL *) NULL;

  int phys_size = p_sys_info->phys_size;
  
  llistad_t * ys = llistad_alloc_initial_length (phys_size);
  llistad_t * xs = llistad_alloc_initial_length (phys_size);

  int i;
  for (i = 0; i < phys_size; i++)
    {
      double x = sys[index_x(i)];
      double y = sys[index_y(i)];
      
      llistad_add (xs, x);
      llistad_add (ys, y);

      in->pointlist[2 * i] = (REAL) x;
      in->pointlist[2 * i + 1] = (REAL) y;
    }
  for (i = phys_size; i < sys_size; i++)
    {
      in->pointlist[2 * i] = (REAL) sys[index_x(i)];
      in->pointlist[2 * i + 1] = (REAL) sys[index_y(i)];
    }

  /* Afegeixo dos punts als extrems superiors i inferiors per tal que
   * la trinangulació doni bé o similar a bé.  */

  double x_min = llistad_min (xs);
  double x_max = llistad_max (xs);
  double y_min = llistad_min (ys);
  double y_max = llistad_max (ys);

  llistad_free (xs);
  llistad_free (ys);

  double delta_y = p_sys_info->delta_y;

  in->pointlist[2 * sys_size] = (x_max - x_min) / 2.;
  in->pointlist[2 * sys_size + 1] = y_max + 30 * delta_y;
  in->pointlist[2 * (sys_size + 1)] = (x_max - x_min) / 2.;
  in->pointlist[2 * (sys_size + 1) + 1] = y_min - 30 * delta_y;

  /* Preparo out  */
  out = (struct triangulateio *) xmalloc (sizeof (struct triangulateio));
  out->pointlist = (REAL *) NULL;
  out->numberofpoints = in_numberofpoints;
  out->pointattributelist = (REAL *) NULL;
  out->pointmarkerlist = (int *) NULL;
  out->trianglelist = (int *) NULL;
  out->triangleattributelist = (REAL *) NULL;
  out->trianglearealist = (REAL *) NULL;
  out->neighborlist = (int *) NULL;
  out->segmentlist = (int *) NULL;
  out->segmentmarkerlist = (int *) NULL;
  out->holelist = (REAL *) NULL;
  out->regionlist = (REAL *) NULL;
  out->edgelist = (int *) NULL;
  out->edgemarkerlist = (int *) NULL;
  out->normlist = (REAL *) NULL;

  return 0;
}

int
data_triangle_run_triangle (void)
{
  triangulate (triflags, in, out, (struct triangulateio *) 0);
  return 0;
}

int 
data_triangle_extract_triangles_returned_data (const sys_info_t * p_sys_info)
{
  int phys_size = p_sys_info->phys_size;
  int i;
  for (i = 0; i < phys_size; i++) // out->numberofpoints; i++)
    coordinacio[i] = 0;

  for (i = 0; i < out->numberoftriangles; i++)
    {
      int t1 = out->trianglelist[3 * i];
      int t2 = out->trianglelist[3 * i + 1];
      int t3 = out->trianglelist[3 * i + 2];
      if (t1 < phys_size) coordinacio[t1]++;
      if (t2 < phys_size) coordinacio[t2]++;
      if (t3 < phys_size) coordinacio[t3]++;
    }

  return 0;
}

/* int */
/* initialize_triangulateio_structure (struct triangulateio *trian) */
/* { */
/*   trian->pointlist = (REAL *) 0; */
/*   trian->pointattributelist = (REAL *) 0; */
/*   trian->pointmarkerlist = (int *) 0; */
/*   trian->trianglelist = (int *) 0; */
/*   trian->triangleattributelist = (REAL *) 0; */
/*   trian->trianglearealist = (REAL *) 0; */
/*   trian->neighborlist = (int *) 0; */
/*   trian->segmentlist = (int *) 0; */
/*   trian->segmentmarkerlist = (int *) 0; */
/*   trian->holelist = (REAL *) 0; */
/*   trian->regionlist = (REAL *) 0; */
/*   trian->edgelist = (int *) 0; */
/*   trian->edgemarkerlist = (int *) 0; */
/*   trian->normlist = (REAL *) 0; */

/*   return 0; */
/* } */

int
data_triangle_free (void)
{
  /* allibero in  */
/*   FREE_POINTER (in->pointlist); */
  in->pointlist = (REAL *) 0;
  FREE_POINTER (in->pointattributelist);
  FREE_POINTER (in->pointmarkerlist);
  FREE_POINTER (in->trianglelist);
  FREE_POINTER (in->triangleattributelist);
  FREE_POINTER (in->trianglearealist);
  FREE_POINTER (in->neighborlist);
  FREE_POINTER (in->segmentlist);
  FREE_POINTER (in->segmentmarkerlist);
  FREE_POINTER (in->holelist);
  FREE_POINTER (in->regionlist);
  FREE_POINTER (in->edgelist);
  FREE_POINTER (in->edgemarkerlist);
  FREE_POINTER (in->normlist);
  FREE_POINTER (in);

  /* alliber out  */
  FREE_POINTER (out->pointlist);
  FREE_POINTER (out->pointattributelist);
  FREE_POINTER (out->pointmarkerlist);
  FREE_POINTER (out->trianglelist);
  FREE_POINTER (out->triangleattributelist);
  FREE_POINTER (out->trianglearealist);
  FREE_POINTER (out->neighborlist);
  FREE_POINTER (out->segmentlist);
  FREE_POINTER (out->segmentmarkerlist);
  FREE_POINTER (out->holelist);
  FREE_POINTER (out->regionlist);
  FREE_POINTER (out->edgelist);
  FREE_POINTER (out->edgemarkerlist);
  FREE_POINTER (out->normlist);
  FREE_POINTER (out);

  return 0;
}

static int n_dislos = 0;
static int * n5_index = 0;
static int * n7_index = 0;

int
dislo_list_alloc (const int base_size)
{
  n_dislos = 0;
  n5_index = (int *) malloc (base_size * sizeof (int));
  n7_index = (int *) malloc (base_size * sizeof (int));

  return 0;
}

int
dislo_list_add (const int n5i, const int n7i, const sys_info_t * p_sys_info)
     /** Mètode de suport per la llista de dislos.  */
{
  const int NX = p_sys_info->NX;
  const int NY = p_sys_info->NY;

  int fila5 = n5i / NX;
  int fila7 = n7i / NX;

  if (fila5 == 0)
    return 0;
  else if (fila5 >= NY - 1)
    return 0;
  if (fila7 == 0)
    return 0;
  else if (fila7 >= NY - 1)
    return 0;

  n5_index[n_dislos] = n5i;
  n7_index[n_dislos] = n7i;
  n_dislos++;

  return 0;
}

int
dislo_list_free (void)
{
  FREE_POINTER (n5_index);
  FREE_POINTER (n7_index);

  return 0;
}

int
dislo_list_print (const int snap, const double t)
{
  /** snap t e_strain xd yd 7n x7 y7 5n x5 y5 */
  /**  1   2     3     4  5  6 7  8  9  10 11 */
  const char * dislo_hist_format = 
    "%d %.5e %.5e %.5e %.5e %d %.5e %.5e %d %.5e %.5e\n";

  static int n_real_dislos;
  n_real_dislos = 0;

  if (n_dislos == 0)
      return 0;

  int k;
  for (k = 0; k < n_dislos; k++)
    {
      int n5i = n5_index[k];
      int n7i = n7_index[k];
      int multiplicity = 1;
      int l;
      for (l = k + 1; l < n_dislos; l++)
	{
	  if ((n5_index[l] == n5i)&(n7_index[l] == n7i))
	    multiplicity++;
	}
      switch (multiplicity)
	{
	case 2:
	  n_real_dislos++;
	  double x7 = (double) in->pointlist[2 * n7i];
	  double y7 = (double) in->pointlist[2 * n7i + 1];

	  double x5 = (double) in->pointlist[2 * n5i];
	  double y5 = (double) in->pointlist[2 * n5i + 1];

	  double xd = 0.5 * (x7 + x5);
	  double yd = 0.5 * (y7 + y5);
	  
	  if (historia_defectes)
	    fprintf (pfile_historia_defectes, dislo_hist_format,
		     snap, t, e_strain, xd, yd,
		     n7i, x7, y7, n5i, x5, y5);
	  break;
	default: 
	  break;
	}
    }

  return n_real_dislos;
}

int
detect_and_print_dislocations (const sys_info_t * p_sys_info, 
			       const int snap, const double t)
{
  /** snap t e_strain xd yd 7n x7 y7 5n x5 y5 */
  /**  1   2     3     4  5  6 7  8  9  10 11 */
  const char * dislo_hist_format = 
    "%d %.5e %.5e %.5e %.5e %d %.5e %.5e %d %.5e %.5e\n";

  if (historia_defectes)
    {
      char name[1000];
      sprintf (name, "def%04d.dat", snap);
      fprintf (pfile_historia_defectes, "pystart %s\n", name);
    }

  int sys_size = p_sys_info->sys_size;
  dislo_list_alloc (sys_size);
  int tri;
  for (tri = 0; tri < out->numberoftriangles; tri++)
    {
      int vertex_1 = out->trianglelist[3 * tri];
      int vertex_2 = out->trianglelist[3 * tri + 1];
      int vertex_3 = out->trianglelist[3 * tri + 2];

      int coord_v1 = coordinacio[vertex_1];
      int coord_v2 = coordinacio[vertex_2];
      int coord_v3 = coordinacio[vertex_3];

      int product_v1_v2 = coord_v1 * coord_v2;
      int product_v2_v3 = coord_v2 * coord_v3;
      int product_v3_v1 = coord_v3 * coord_v1;

      int dislocacio = 0;
      int i,j;
      /** Utilitzant el fet que són primers...  */
      /** i és el vertex de 7 i j el de 5.  */
      if (product_v1_v2 == 35)
	/** Sabem que és una parella 5-7 unida per un segment s.  */
	{
	  dislocacio++;
	  if (coord_v1 == 7)
	    {
	      i = vertex_1;
	      j = vertex_2; // ojú!!! j = vertex_1;
	      dislo_list_add (j, i, p_sys_info);
	    }
	  else /** coord_v1 = 5  */
	    {
	      i = vertex_2;
	      j = vertex_1;
	      dislo_list_add (j, i, p_sys_info);
	    }
	}
      if (product_v2_v3 == 35)
	{
	  dislocacio++;
	  if (coord_v2 == 7)
	    {
	      i = vertex_2;
	      j = vertex_3;
	      dislo_list_add (j, i, p_sys_info);
	    }
	  else
	    {
	      i = vertex_3;
	      j = vertex_2;
	      dislo_list_add (j, i, p_sys_info);
	    }
	}
      if (product_v3_v1 == 35)
	{
	  dislocacio++;
	  if (coord_v3 == 7)
	    {
	      i = vertex_3;
	      j = vertex_1;
	      dislo_list_add (j, i, p_sys_info);
	    }
	  else
	    {
	      i = vertex_1;
	      j = vertex_3;
	      dislo_list_add (j, i, p_sys_info);
	    }
	}
    }

  int n_real_dislos = dislo_list_print (snap, t);
  dislo_list_free ();

  const int sys_paret_L_init = p_sys_info->phys_size;
  const int sys_paret_L_fin  = p_sys_info->phys_size + p_sys_info->paret_size;
  const int sys_paret_R_init = sys_paret_L_fin;
  const int sys_paret_R_fin  = p_sys_info->sys_size;

  if (historia_defectes)
    {
      int i;
      for (i = sys_paret_L_init; i < sys_paret_L_fin; i++)
	{
	  double x = (double) in->pointlist[2 * i];
	  double y = (double) in->pointlist[2 * i + 1];
	  
	  fprintf (pfile_historia_defectes, dislo_hist_format,
		   snap, t, e_strain, x, y,
		   0, 0., 0., 0, 0., 0.);
	}
      for (i = sys_paret_R_init; i < sys_paret_R_fin; i++)
	{
	  double x = (double) in->pointlist[2 * i];
	  double y = (double) in->pointlist[2 * i + 1];
	  
	  fprintf (pfile_historia_defectes, dislo_hist_format,
		   snap, t, e_strain, x, y,
		   0, 0., 0., 0, 0., 0.);
	}
      fprintf (pfile_historia_defectes, "pyend\n");
    }

  return n_real_dislos;
}
