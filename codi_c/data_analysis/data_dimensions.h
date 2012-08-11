/** data_dimensions.h  */
#ifndef DATA_DIMENSIONS_H
#define DATA_DIMENSIONS_H
#include "dr_reader.h"
int data_dimensions_alloc (FILE * pfile, const sys_info_t * p_sys_info);
int data_dimensions_free (void);
int data_dimensions_extract (const sys_info_t * p_sys_info, const double * sys,
			     const double * f, const int snap, const double t);
int data_dimensions_print (FILE * pfile, const sys_info_t * p_sys_info,
			   const int snap, const double t);
#endif /* DATA_DIMENSIONS_H  */
