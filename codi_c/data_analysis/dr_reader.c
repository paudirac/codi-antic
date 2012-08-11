/* dr_reader.c  **/
/*
 * Serialitzador que escriu en format:
 * header
 * frame_0_header
 * dades_frame_0
 * frame_1_header
 * dades_frame_0
 * ...
 * frame_n_header
 * dades_frame_n
 *
 * On header: NX NY phys_size paret_size sys_size ny_extra
 *            V RATIO random_seed delta_x delta_y
 * On frame_header_i: snap t
 * On dades_frame_i: :x_0 y_0:x_1 y_1: ... :x_sys_size y_sys_size:
 **/
#define _GNU_SOURCE /* el necessita getline  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dr_reader.h"
#include "macroutils.h"

int read_line (FILE * pfile, char ** p2pline);
int read_header (FILE * pfile, sys_info_t * p_sys_info);
int read_frame_header (FILE * pfile, int * snap, double * t);
int read_particles (FILE * pfile, double * sys, sys_info_t * p_sys_info);
int adeu (void);

//static const char * new_line = "\n";
static const char * particle_delim = ":";

int
dr_reader_init (FILE * pfile, sys_info_t * p_sys_info)
{
  read_header (pfile, p_sys_info);

  return 0;
}

int
dr_reader_read_frame (FILE * pfile, double * sys, sys_info_t * p_sys_info,
		      int * snap, double * t)
{
  read_frame_header (pfile, snap, t);
  read_particles (pfile, sys, p_sys_info);

  return 0;
}

int
dr_reader_fin (FILE * pfile)
{
  return 0;
}

int
read_header (FILE * pfile, sys_info_t * p_sys_info)
{
  const char * header_format_in = "%d %d %d %d %d %d %le %le %ld %le %le";

  char * line;
  int NX, NY, phys_size, paret_size, sys_size, ny_extra;
  double V, RATIO, delta_x, delta_y;
  long int random_seed;
  if (read_line (pfile, &line) != -1)
    sscanf (line, header_format_in,
	    &NX, &NY, &phys_size, &paret_size, &sys_size, &ny_extra,
	    &V, &RATIO, &random_seed, &delta_x, &delta_y);

  p_sys_info->NX = NX;
  p_sys_info->NY = NY;
  p_sys_info->phys_size = phys_size;
  p_sys_info->paret_size = paret_size;
  p_sys_info->sys_size = sys_size;
  p_sys_info->ny_extra = ny_extra;
  p_sys_info->V = V;
  p_sys_info->RATIO = RATIO;
  p_sys_info->random_seed = random_seed;
  p_sys_info->delta_x = delta_x;
  p_sys_info->delta_y = delta_y;

  return 0;
}

int
adeu (void)
{
  fprintf (stderr, "S'ha acabat el fitxer, pallús!\n");
  exit (0);
  return 0;
}


int
read_frame_header (FILE * pfile, int * snap, double * t)
{
  const char * frame_header_format_in = "%d %le"; // i per l'strain?!

  char * line;
  if (read_line (pfile, &line) != -1)
    sscanf (line, frame_header_format_in, snap, t);

  //  fprintf (stderr, "snap: %d\n", *snap);

  return 0;
}

int
read_particles (FILE * pfile, double * sys, sys_info_t * p_sys_info)
     /* Oju, que el format d'emmagatzematge de sys sera:
      * sys[2 * i] = x_i (f_x_i)
      * sys[2 * i + 1] = y_i (f_y_i)  **/
{
  int sys_size = p_sys_info->sys_size;
  const char * particle_format_in = "%le %le";

  char * particles;
  if ((read_line (pfile, &particles) != -1))
    {
      char * particles_cp = strdup (particles);
      char * particle;
      particle = strtok (particles_cp, particle_delim);
      int i = 0;
      while ((particle != 0)&&(i < sys_size))
	{
	  double x,y;
	  sscanf (particle, particle_format_in, &x, &y);
/* 	  if (i > p_sys_info->phys_size) */
/* 	    fprintf (stderr, "%d \t(%f, %f)\n", i, x, y); */
	  sys[index_x(i)] = x;
	  sys[index_y(i)] = y;
	  i = i + 1;
	  particle = strtok (0, particle_delim);
	}
    }

  return 0;
}

int
read_line (FILE * pfile, char ** p2pline)
{
  int char_read = -1;
  size_t line_size = 0;
  char_read = getline (p2pline, &line_size, pfile);
  if (char_read == -1) adeu ();

  return char_read;
}

sys_info_t *
sys_info_alloc (void)
{
  sys_info_t * sys_info_ = (sys_info_t *) malloc (sizeof (sys_info_t));
  if (sys_info_ == 0) abort ();
  else
    {
      sys_info_->NX = 0;
      sys_info_->NY = 0;
      sys_info_->phys_size = 0;
      sys_info_->paret_size = 0;
      sys_info_->sys_size = 0;
      sys_info_->ny_extra = 0;
      sys_info_->V = 0.;
      sys_info_->RATIO = 0.;
      sys_info_->random_seed = 0;
      sys_info_->delta_x = 0.;
      sys_info_->delta_y = 0.;
    }

  return sys_info_;
}

int
sys_info_free (sys_info_t * p_sys_info)
{
  FREE_POINTER (p_sys_info);
  return 0;
}

int
sys_info_equal (sys_info_t * one, sys_info_t * other)
{
  return ((one->NX == other->NX) &&
	  (one->NY == other->NY) &&
	  (one->phys_size == other->phys_size) &&
	  (one->paret_size == other->paret_size) &&
	  (one->sys_size == other->sys_size)&&
	  (one->ny_extra == other->ny_extra)&&
	  (one->V == other->V)&&
	  (one->RATIO == other->RATIO)&&
	  (one->random_seed == other->random_seed)&&
	  (one->delta_x == other->delta_x)&&
	  (one->delta_y == other->delta_y));
}

int
sys_info_print (sys_info_t * p_sys_info)
{
  fprintf (stderr, "sys_info:    NX          = %d\n", p_sys_info->NX);
  fprintf (stderr, "             NY          = %d\n", p_sys_info->NY);
  fprintf (stderr, "             phys_size   = %d\n", p_sys_info->phys_size);
  fprintf (stderr, "             paret_size  = %d\n", p_sys_info->paret_size);
  fprintf (stderr, "             sys_size    = %d\n", p_sys_info->sys_size);
  fprintf (stderr, "             ny_extra    = %d\n", p_sys_info->ny_extra);
  fprintf (stderr, "             V           = %2.4f\n", p_sys_info->V);
  fprintf (stderr, "             RATIO (T)   = %2.4f\n", p_sys_info->RATIO);
  fprintf (stderr, "             random_seed = %ld\n", 
	   p_sys_info->random_seed);
  fprintf (stderr, "             delta_x     = %2.4f\n", p_sys_info->delta_x);
  fprintf (stderr, "             delta_y     = %2.4f\n", p_sys_info->delta_y);
  return 0;
}
