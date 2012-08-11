/* data_n.h  */
#ifndef DATA_N_H
#define DATA_N_H
#include "dr_reader.h"
int data_n_alloc (FILE *pfile, const sys_info_t * p_sys_info);
int data_n_free (void);
int data_n_extract (const sys_info_t * p_sys_info, const double * sys,
		    const double * f, const int snap, const double t);
int data_n_print (FILE *pfile, const sys_info_t * p_sys_info,
		  const int snap, const double t);
int data_n_set_defect_history (FILE * pfile);
int data_n_set_coordination_history (FILE * pfile);
#endif /* DATA_N_H  */



/* struct triangulateio * data_n_get_triangulateio_out (void); */
/* struct triangulateio * data_n_get_triangulateio_in (void); */
/* int * data_n_get_coordinacio (void); */
