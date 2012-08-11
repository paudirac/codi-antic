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
#include "sistema.h"
#include "globals.h"

int print_header (FILE * pfile);
int print_frame_header (FILE * pfile, const int snap);
int print_particles (FILE * pfile, const double * sys);

static const char * new_line = "\n";
static const char * particle_delim = ":";

int
dr_writer_init (FILE * pfile)
{
  print_header (pfile);
  return 0;
}

int
dr_writer_write_frame (FILE * pfile, const int snap, const double * sys)
     /* Nota: en cas que vulgui canviar-se el format del writter,
      * es pot sobreescriure aquest metode o be el print_particles. **/
{
  print_frame_header (pfile, snap);
  print_particles (pfile, sys);
  return 0;
}

int
dr_writer_fin (FILE * pfile)
{
  fprintf (pfile, new_line);
  return 0;
}

int
print_header (FILE * pfile)
{
  /* NX NY phys_size paret_size sys_size ny_extra  **/
  /* 1  2    3          4         5        6       **/
  /* V  RATIO random_seed delta_x delta_y          **/
  /* 7    8     9           10       11            **/
  const char * header_format = 
    "%d %d %d %d %d %d " \
    "%.5e %.5e %ld %.5e %.5e";
  fprintf (pfile, header_format, 
	   NX, NY, phys_size, paret_size, sys_size, ny_extra,
	   V, RATIO, random_seed, delta_x, delta_y);
  fprintf (pfile, new_line);
  return 0;
}

extern double t;

int
print_frame_header (FILE * pfile, const int snap)
{

  const char * frame_header_format = "%d %.5e";
  fprintf (pfile, frame_header_format, snap, t);
  fprintf (pfile, new_line);
  return 0;
}

int
print_particles (FILE * pfile, const double * sys)
     /* Nota: en cas que es vulgui escriure en un altre format,
      * es pot sobreescriure aquest metode.  **/
{
  const char * particle_format = "%.5e %.5e";
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
