/*************************************************
 * File: cells.c
 *
 * Description:
 *   - celles de Verlett amb condicions
 *     de contorn de Couette: periòdiques
 *     en l'eix y i normals en l'eix x.
 *
 * Author: Pau Cervera
 * Date: 31.12.2005
 *
 *************************************************/
#include <stdlib.h>
#include <math.h>
#include <error.h>
#include <gsl/gsl_errno.h>
#include "globals.h"
#include "ioutils.h"
#include "sistema.h"
#include "odedef.h"
#include "boundaries.h"
#include "cells.h"
#include "macroutils.h"
#include "memoria.h"

static int I_SIZE = 0;
static int J_SIZE = 0;
int N_CELLS;

int *head = 0;//[I_SIZE * J_SIZE];
int *list = 0;//[sys_array_size + 1];
int *neighbour_cell = 0;//[4 * I_SIZE * J_SIZE];
static int initiated_neighbour_cell_map = 0;

/***********************************
 * Com fer els loops sobre cells.
 ***********************************
 *    for (cell = 0; cell < N_CELLS; cell++)
 *     {
 *       int particle_index = head[cell];
 *       while (particle != -1)
 * 	  {
 * 	    //... do anything
 * 	    particle = list[particle];
 * 	  }
 *     }
 * i així.  */

/* Les neighbour_cells contenen les veines d'una cell donada.
 * La cel·la cell té 4 veines (2D, en 3D serien 13),
 *   neighbour_cell[cell]
 *   neighbour_cell[cell + 1]
 *   neighbour_cell[cell + 2]  i
 *   neighbour_cell[cell + 3].
 * Si no existeixen veins neighbour_cell[cell] = -1.
 * La disposició geomètrica és:
 *   ---------------------------------
 *   |       |  0th  |  1st  |       |
 *   |       | i,j+1 |i+1,j+1|       |
 *   ---------------------------------
 *   |       |  cell |  2nd  |       |
 *   |       |  i,j  | i+1,j |       |
 *   ---------------------------------
 *   |       |       |  3d   |       |
 *   |       |       |i+1,j-1|       |
 *   ---------------------------------  */

/* Les llistes funcionen de la forma següent:
 * head[cell] conté l'índex de la posició x de la primera partícula 
 * que hi haga a la cel·la cell.
 *
 * index_particula = head[cell]
 * 
 * Al mateix temps, list[index_particula] conté l'índex de la posició x
 * d'una altra partícula continguda a la cel·la cell.
 * I així fins que no queden partícules. Si l'index de la última partícula
 * és index_ultima_particula, llavors:
 *
 * list[index_ultima_particula] = -1,
 *
 * d'aquesta forma s'acaba la iteració.
 * Veure: Computer Simulation of Liquids. 
 *        M.P. Allen & D.J. Tildesley, p.151  */

int (*inicia_neighbour_cell_map) (void);
int inicia_neighbour_cell_map_couette_geometry (void);
int inicia_neighbour_cell_map_basic_geometry (void);
int cells_alloc_memory (void);
int cells_update_cells_ (void);

int
head_a_fi_de_llista (void)
     /* Posa head[i] a -1.  */
{
  int i;
  for (i = 0; i < N_CELLS; i++)
    head[i] = -1;
  return 0;
}

int
list_a_fi_de_llista (void)
     /* Posa list[i] = -1, que és fi_de_llista.  */
{
  int i;
  for (i = 0; i < (sys_array_size + 1); i++)
    list[i] = -1;
  return 0;
}

int
neighbour_cell_a_no_neighbour (void)
     /* Posa neighbour_cell[i] = -1, que és no_neighbour.  */
{
  int i;
  for (i = 0; i < 4 * N_CELLS; i++)
    neighbour_cell[i] = -1;
  return 0;
}

int
cells_calc_sizes_free (void) // no m'agrada el nom acabat en free indueix a err
{
  double x_distance = sys_x_distance ();
  double y_distance = sys_y_distance ();
  double cell_distance = range_factor * dist_a;

  I_SIZE = (int) (x_distance / cell_distance) + 1;
  J_SIZE = (int) (y_distance / cell_distance) + 1;

  if (I_SIZE < 3) I_SIZE = 3; /* Degut a la geometria de veins, 3 mínim.  */
  if (J_SIZE < 3) J_SIZE = 3; /* Degut a la geometria de veins, 3 mínim.  */

  N_CELLS = I_SIZE * J_SIZE;

  return 0;
}

int
cells_calc_sizes_couette (void)
{
  dimensions *dim = sys_initial_dimensions_handle ();
  double x_distance = (dim->x_max - dim->x_min);
  double y_distance = (dim->y_max - dim->y_min);
  y_distance = y_distance + delta_y;
  double cell_distance = range_factor * dist_a;

/*   I_SIZE = (int) ceil (x_distance / cell_distance); */
/*   J_SIZE = (int) ceil (y_distance / cell_distance); */

/*   I_SIZE = (int) (x_distance / cell_distance + 1e-6) + 1; */
/*   J_SIZE = (int) (y_distance / cell_distance + 1e-6) + 1; */

  I_SIZE = (int) (x_distance / cell_distance) + 1;
  J_SIZE = (int) (y_distance / cell_distance) + 1;

  if (I_SIZE < 3) I_SIZE = 3; /* Degut a la geometria de veins, 3 mínim.  */
  if (J_SIZE < 3) J_SIZE = 3; /* Degut a la geometria de veins, 3 mínim.  */

  N_CELLS = I_SIZE * J_SIZE;
  
/*   fprintf (stderr, "cells: I_SIZE = %d J_SIZE = %d\n", I_SIZE, J_SIZE); */
  return 0;
}

int
cells_alloc (void)
{
  list = (int *) malloc ((sys_array_size + 1) * sizeof (int));
  switch (cells_geometry)
    {
    case COUETTE:
      inicia_neighbour_cell_map = 
	&inicia_neighbour_cell_map_couette_geometry;
      break;
    case FREE:
      inicia_neighbour_cell_map =
	&inicia_neighbour_cell_map_basic_geometry;
      break;
    case SHEAR:
      inicia_neighbour_cell_map =
	&inicia_neighbour_cell_map_couette_geometry;
      break;
    default:
      inicia_neighbour_cell_map = 
	&inicia_neighbour_cell_map_basic_geometry;
      break;
    }

  cells_alloc_memory ();
  inicia_neighbour_cell_map ();
  cells_update_cells_ ();

/*   cells_calculate (); */

  return 0;
}

int cells_alloc_memory (void)
{
  switch (cells_geometry)
    {
    case COUETTE:
      cells_calc_sizes_couette ();
      break;
    case FREE:
      cells_calc_sizes_free ();
      break;
    case SHEAR:
      cells_calc_sizes_couette ();
      break;
    default:
      cells_calc_sizes_free ();
      break;
    }

  FREE_POINTER (head);
  head = (int *) xmalloc (N_CELLS * sizeof (int));

/*   int * temp_head = (int *) realloc (head, N_CELLS * sizeof (int)); */
/*   FREE_POINTER (head); */
/*   *head = *temp_head; */
  if (head == 0)
    {
      error_at_line (0, errno, __FILE__, __LINE__,
		     "No puc allocatar head[%dx%d].\n",
		     I_SIZE, J_SIZE);
      return GSL_ENOMEM;
    }

  FREE_POINTER (neighbour_cell);
  neighbour_cell = (int *) xmalloc (4 * I_SIZE * J_SIZE * sizeof (int));
/*   neighbour_cell = (int *) realloc (neighbour_cell, */
/* 				    4 * I_SIZE * J_SIZE * sizeof (int)); */
  if (neighbour_cell == 0)
    {
      error_at_line (0, errno, __FILE__, __LINE__,
		     "No puc allocatar neighbour_cell[%dx%dx%d].\n",
		     4, I_SIZE, J_SIZE);
      return GSL_ENOMEM;
    }

  list_a_fi_de_llista ();
  head_a_fi_de_llista ();
  neighbour_cell_a_no_neighbour ();

/*   log_to_logfile ("Variables del les cel·les de Verlett allocatades.", */
/* 		  __FILE__, __LINE__); */

/*   char log_i_size[100]; */
/*   sprintf (log_i_size, "I_SIZE = %d", I_SIZE); */
/*   log_to_logfile (log_i_size, __FILE__, __LINE__); */

/*   char log_j_size[100]; */
/*   sprintf (log_j_size, "J_SIZE = %d", J_SIZE); */
/*   log_to_logfile (log_j_size, __FILE__, __LINE__); */

/*   char log_n_cells[100]; */
/*   sprintf (log_n_cells, "N_CELLS = %d", N_CELLS); */
/*   log_to_logfile (log_n_cells, __FILE__, __LINE__); */

  return 0;
}

int
cells_free (void)
{
/*   free (head); */
/*   free (list); */
/*   free (neighbour_cell); */
  FREE_POINTER (head);
  FREE_POINTER (list);
  FREE_POINTER (neighbour_cell);
  log_to_logfile ("Alliberat l'espai de memòria ocupat per les cel·les de Verlett.", __FILE__, __LINE__);
  return 0;
}

int
inicia_neighbour_cell_map_couette_geometry (void)
     /* La subrutina funciona i implementa condicions periòdiques
      * de Couette: són períodiques en la direcció vertical (índex j).
      *
      * Pot comprovar-se amb el pinta_les_neighbour_cell ().  */
{
  int i, j;
  int zeroth_i,first_i,second_i,third_i,zeroth_j,first_j,second_j,third_j;
  for (j = 0; j < J_SIZE; j++) {
    for (i = 0; i < I_SIZE; i++) {
      zeroth_i = i;
      zeroth_j = j + 1;
      first_i = i + 1;
      first_j = j + 1;
      second_i = i + 1;
      second_j = j;
      third_i = i + 1;
      third_j = j - 1;
      neighbour_cell[4*(j * I_SIZE + i)] = zeroth_j * I_SIZE + zeroth_i;
      neighbour_cell[4*(j * I_SIZE + i) + 1] = first_j * I_SIZE + first_i;
      neighbour_cell[4*(j * I_SIZE + i) + 2] = second_j * I_SIZE + second_i;
      neighbour_cell[4*(j * I_SIZE + i) + 3] = third_j * I_SIZE + third_i;

      if (zeroth_j == J_SIZE)
	    neighbour_cell[4*(j * I_SIZE + i)] = 0 * I_SIZE + zeroth_i;
      if (first_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 1] = -1;
      if (first_j == J_SIZE)
	{
	  if (first_i == I_SIZE)
	    neighbour_cell[4*(j * I_SIZE + i) + 1] = -1;
	  else
	    neighbour_cell[4*(j * I_SIZE + i) + 1] = 0 * I_SIZE + first_i;
	}
      if (second_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 2] = -1;
      if (third_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 3] = -1;
      if (third_j < 0)
	{
	  if (third_i == I_SIZE)
	    neighbour_cell[4*(j * I_SIZE + i) + 3] = -1;
	  else
	    neighbour_cell[4*(j * I_SIZE + i) + 3] = 
	      (J_SIZE - 1) * I_SIZE + third_i;
	}
    }
  }

  initiated_neighbour_cell_map = 1;
/*   log_to_logfile  */
/*     ("Inicialitzat el neighbour_cell map amb geometria de Couette.", */
/*      __FILE__, __LINE__); */

  return 0;
}

int
inicia_neighbour_cell_map_basic_geometry (void)
     /* No hi ha condicions periòdiques de contorn.
      * Pot comprovar-se amb el pinta_les_neighbour_cell ().  */
{
  int i, j;
  int zeroth_i,first_i,second_i,third_i,zeroth_j,first_j,second_j,third_j;
  for (j = 0; j < J_SIZE; j++) {
    for (i = 0; i < I_SIZE; i++) {
      zeroth_i = i;
      zeroth_j = j + 1;
      first_i = i + 1;
      first_j = j + 1;
      second_i = i + 1;
      second_j = j;
      third_i = i + 1;
      third_j = j - 1;
      neighbour_cell[4*(j * I_SIZE + i)] = zeroth_j * I_SIZE + zeroth_i;
      neighbour_cell[4*(j * I_SIZE + i) + 1] = first_j * I_SIZE + first_i;
      neighbour_cell[4*(j * I_SIZE + i) + 2] = second_j * I_SIZE + second_i;
      neighbour_cell[4*(j * I_SIZE + i) + 3] = third_j * I_SIZE + third_i;

      if (zeroth_j >= J_SIZE) neighbour_cell[4*(j * I_SIZE + i)] = -1;
      if (first_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 1] = -1;
      if (first_j >= J_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 1] = -1;
      if (second_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 2] = -1;
      if (third_i >= I_SIZE) neighbour_cell[4*(j * I_SIZE + i) + 3] = -1;
      if (third_j < 0) neighbour_cell[4*(j * I_SIZE + i) + 3] = -1;
    }
  }

  initiated_neighbour_cell_map = 1;
/*   log_to_logfile  */
/*     ("Inicialitzat el neighbour_cell map amb geometria de bàsica.", */
/*      __FILE__, __LINE__); */
  return 0;
}

  /*
   * Potser s'ha de refer update_cells, que no recordo
   * com estan posats els índexs, però funcionava bé.
   */



int
cells_calculate (void)
{
  cells_alloc_memory ();
  inicia_neighbour_cell_map ();
  cells_update_cells_ ();

  return 0;  
}

/* S'ha de canviar el mètode cells_update_cells_,
 * que no funciona, i mirar que funcioni, fins i tot quan
 * s'ha de posar I_SIZE i J_SIZE a 3 perquè hi ha massa
 * poques partícules.  */

int
cells_update_cells_ (void)
{
  list_a_fi_de_llista ();
  head_a_fi_de_llista ();

  double *sys = sys_handle();

  dimensions *dim = sys_dimensions_handle ();

  double long_x = (dim->x_max - dim->x_min);
  double long_y = (dim->y_max - dim->y_min) + delta_y;

  if (long_x == 0.) long_x = 1;  
  if (long_y == 0.) long_y = 1;

/*   double x_min = dim->x_min; */
  double y_min = dim->y_min - delta_y / 2.;

  int i;
  for (i = 0; i < sys_size; i++) {
    double x = (sys[i] - dim->x_min);
    double y = (sys[index_y(i)] - y_min);

    if (x < 0.)
      error_at_line (0, errno, __FILE__, __LINE__,
		     "Error amb el cells_update_cells_: x < 0.\n");
    if (y < 0.)
      error_at_line (0, errno, __FILE__, __LINE__,
		     "Error amb el cells_update_cells_: y < 0.\n");

    x = x / long_x; // Això va de [0, I_SIZE]
    y = y / long_y; // aquest de [0, J_SIZE]

/*     int i_index = (int) (x + 1e-6); // això és un - o un +? */
/*     int j_index = (int) (y + 1e-6); */

    int i_index = ((int) x) * I_SIZE;
    int j_index = ((int) y) * J_SIZE;

    int cell = j_index * I_SIZE + i_index;

    if (cell > N_CELLS)
      fprintf (stderr, "%f/%f = %f %f/%f = %f %d/%d %d/%d %d/%d\n",
	       sys[i], long_x, x,
	       sys[index_y(i)], long_y, y,
	       i_index, I_SIZE,
	       j_index, J_SIZE,
	       cell, N_CELLS);
    
    if (cell > N_CELLS)
      error_at_line (0, errno, __FILE__, __LINE__,
		     "Error amb el cells_update_cells_: cell > N_CELLS.\n");
    if (cell < 0)
      error_at_line (0, errno, __FILE__, __LINE__,
		     "Error amb el cells_update_cells_: cell <0.\n");

    if (head[cell] > sys_array_size + 1)
      error_at_line (0, errno, __FILE__, __LINE__,
		     "Error perquè head[cell] > sys_array_size + 1.\n");

    list[i] = head[cell];
    head[cell] = i;
  }

  return 0;
}

int
cells_loop_force (double t, const double sys[],
		  double f[], void *params,
		  forca_function_pointer pf)
{
  int cell;
  for (cell = 0; cell < N_CELLS; cell++)
    /* Loop sobre cells. */
    {
      int a_atom = head[cell];    
      while (a_atom != -1)
	/* Loop dins de la cel·la.  */
	{
	  /* Forca a_atom amb b_atom els dos dins la mateixa cel·la.  */
	  int b_atom = list[a_atom];
	  while (b_atom != -1)
	    {
	      pf (t, f, sys, a_atom, b_atom);
	      b_atom = list[b_atom];
	    }

	  /* Forca amb els àtoms de les cel·les veïnes.  */
	  int neigh_index;
	  for (neigh_index = 0; neigh_index < 4; neigh_index++)
	    /* Loop sobre cel·les veïnes.  */
	    {
	      int neigh_cell = neighbour_cell[4 * cell + neigh_index];
	      if (neigh_cell != -1)  /* otherwise there's no neighbour_cell  */
		{
		  /* Forca a_atom amb b_atom on b_atom pertany a una cel·la 
		   * veina.  */
		  int b_atom = head[neigh_cell];
		  while (b_atom != -1)
		    {
		      pf (t, f, sys, a_atom, b_atom);
		      b_atom = list[b_atom];
		    }
		}
	    }
	  a_atom = list[a_atom];
	}
    }

  return 0;
}

int
cells_loop_function (double t, const double sys[],
		     double f[], void *other,
		     function_to_loop_pointer pf)
{
  int cell;
  for (cell = 0; cell < N_CELLS; cell++)
    /* Loop sobre cells. */
    {
      int a_atom = head[cell];    
      while (a_atom != -1)
	/* Loop dins de la cel·la.  */
	{
	  /* Forca a_atom amb b_atom els dos dins la mateixa cel·la.  */
	  int b_atom = list[a_atom];
	  while (b_atom != -1)
	    {
	      pf (t, sys, f, other, a_atom, b_atom);
	      b_atom = list[b_atom];
	    }

	  /* Forca amb els àtoms de les cel·les veïnes.  */
	  int neigh_index;
	  for (neigh_index = 0; neigh_index < 4; neigh_index++)
	    /* Loop sobre cel·les veïnes.  */
	    {
	      int neigh_cell = neighbour_cell[4 * cell + neigh_index];
	      if (neigh_cell != -1)  /* otherwise there's no neighbour_cell  */
		{
		  /* Forca a_atom amb b_atom on b_atom pertany a una cel·la 
		   * veina.  */
		  int b_atom = head[neigh_cell];
		  while (b_atom != -1)
		    {
		      pf (t, sys, f, other, a_atom, b_atom);
		      b_atom = list[b_atom];
		    }
		}
	    }
	  a_atom = list[a_atom];
	}
    }

  return 0;
}


/********************** Funcions de test ******************/
int
cells_pinta_neighbour_cells (void)
{
  fprintf (stderr, "**** Pinto índex de les cel·les *****\n");
  int i, j;
  for (j = 0; j < J_SIZE; j++) {
    for (i = 0; i < I_SIZE; i++) {
      int index = j * I_SIZE + i;
      fprintf (stderr,
	       "%d \t%d %d %d %d\n",
	       index,
	       neighbour_cell[4*index],
	       neighbour_cell[4*index + 1],
	       neighbour_cell[4*index + 2],
	       neighbour_cell[4*index + 3]);
    }
  }
  fprintf (stderr, "******************************\n");

  fprintf (stderr, "Pinto cells i partícules.\n");
  double *sys = sys_handle ();
  int cell;
  for (cell = 0; cell < N_CELLS; cell++)
    {
      fprintf (stderr, "cell: %d\n", cell);
      int particle = head[cell];
      //      if (particle < sys_size)
      //	{
	  while (particle != -1)
	    {
	      fprintf(stderr,"\tparticle: %d", particle);
	      double x = sys[particle];
	      double y = sys[index_y(particle)];
	      fprintf (stderr, "\t(%.5e, %.5e)\n", x, y);
	      particle = list[particle];
	    }
	  //	}
    }
  fprintf (stderr, "head ");
  for (i = 0; i < N_CELLS; i++)
    fprintf (stderr, "%d ", head[i]);
  fprintf (stderr, "\nlist ");
  for (i = 0; i < sys_size + 1; i++)
    fprintf (stderr, "%d ", list[i]);
  fprintf (stderr, "\n");

  return 0;
}

/***********************************
 * Com fer els loops sobre cells.
 ***********************************
 *    for (cell = 0; cell < N_CELLS; cell++)
 *     {
 *       int particle_index = head[cell];
 *       while (particle != -1)
 * 	  {
 * 	    //... do anything
 * 	    particle = list[particle];
 * 	  }
 *     }
 * i així.  */
