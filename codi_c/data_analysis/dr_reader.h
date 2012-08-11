/* dr_reader.h  */
#include <stdio.h>
#ifndef DR_READER_H
#define DR_READER_H
typedef struct sys_info_tag sys_info_t;
/* sistem_info_t  **/
struct sys_info_tag {
  int NX;
  int NY;
  int phys_size;
  int paret_size;
  int sys_size;
  int ny_extra;
  double V;
  double RATIO;
  long int random_seed;
  double delta_x;
  double delta_y;
};

sys_info_t * sys_info_alloc (void);
int sys_info_free (sys_info_t * p_sys_info);
int sys_info_equal (sys_info_t * one, sys_info_t * other);
int sys_info_print (sys_info_t * p_sys_info);

int dr_reader_init (FILE * pfile, sys_info_t * p_sys_info);
int dr_reader_read_frame (FILE * pfile, double * sys, sys_info_t * p_sys_info,
			  int * snap, double * t);
int dr_reader_fin (FILE * pfile);
#define index_x(i) (2 * (i))
#define index_y(i) (2 * (i) + 1)
#endif /* DR_READER_H  */
