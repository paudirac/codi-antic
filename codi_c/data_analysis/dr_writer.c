/* dr_writer.c  **/
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
#include <stdio.h>
#include "dr_writer.h"

int print_header (FILE * pfile, const sys_info_t * p_sys_info);
int print_frame_header (FILE * pfile, const int snap, const double t);
int print_particles_default (FILE * pfile, const double * sys,
			     const sys_info_t * p_sys_info);


static writer_funcp_t print_particles = &print_particles_default;

int
dr_writer_set_writer (writer_funcp_t writer)
{
  if (writer != 0) print_particles = writer;
  else print_particles = &print_particles_default;

  return 0;
}

static const char * new_line = "\n";
static const char * particle_delim = ":";

int
dr_writer_init (FILE * pfile, const sys_info_t * p_sys_info)
{
  print_header (pfile, p_sys_info);

  return 0;
}

int
dr_writer_write_frame (FILE * pfile, const double * sys,
		       const sys_info_t * p_sys_info, const int snap,
		       const double t)
     /* Nota: en cas que vulgui canviar-se el format del writter,
      * es pot sobreescriure aquest metode o be el print_particles. **/
{
  print_frame_header (pfile, snap, t);
  print_particles (pfile, sys, p_sys_info);

  return 0;
}

int
dr_writer_fin (FILE * pfile)
{
  fprintf (pfile, new_line);
  return 0;
}

int
print_header (FILE * pfile, const sys_info_t * p_sys_info)
{
  /* NX NY phys_size paret_size sys_size ny_extra  **/
  /* 1  2    3          4         5        6       **/
  /* V  RATIO random_seed delta_x delta_y          **/
  /* 7    8     9           10       11            **/
  const char * header_format = 
    "%d %d %d %d %d %d " \
    "%.5e %.5e %ld %.5e %.5e";
  int NX, NY, phys_size, paret_size, sys_size, ny_extra;
  int random_seed;
  double V, RATIO, delta_x, delta_y;

  NX = p_sys_info->NX;
  NY = p_sys_info->NY;
  phys_size = p_sys_info->phys_size;
  paret_size = p_sys_info->paret_size;
  sys_size = p_sys_info->sys_size;
  ny_extra = p_sys_info->ny_extra;
  V = p_sys_info->V;
  RATIO = p_sys_info->RATIO;
  random_seed = p_sys_info->random_seed;
  delta_x = p_sys_info->delta_x;
  delta_y = p_sys_info->delta_y;

  fprintf (pfile, header_format, 
	   NX, NY, phys_size, paret_size, sys_size, ny_extra,
	   V, RATIO, random_seed, delta_x, delta_y);
  fprintf (pfile, new_line);

  return 0;
}

int
print_frame_header (FILE * pfile, const int snap, const double t)
{

  const char * frame_header_format = "%d %.5e";
  fprintf (pfile, frame_header_format, snap, t);
  fprintf (pfile, new_line);
  return 0;
}

int
print_particles_default (FILE * pfile, const double * sys,
			 const sys_info_t * p_sys_info)
     /* Nota: en cas que es vulgui escriure en un altre format,
      * es pot sobreescriure aquest metode.  **/
{
  const char * particle_format = "%.5e %.5e";
  int sys_size = p_sys_info->sys_size;
  int i;
  fprintf (pfile, particle_delim);
  for (i = 0; i < sys_size; i++)
    {
      fprintf (pfile, particle_format, sys[i], sys[index_y(i)]);
      fprintf (pfile, particle_delim);
    }
  fprintf (pfile, new_line);
  
  return 0;
}
