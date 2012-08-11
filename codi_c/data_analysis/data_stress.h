/** data_stress.h  */
#ifndef DATA_STRESS_H
#define DATA_STRESS_H
#include "dr_reader.h"
int data_stress_alloc (FILE * pfile, const sys_info_t * p_sys_info);
int data_stress_free (void);
int data_stress_extract (const sys_info_t * p_sys_info, const double * sys,
			 const double * f, const int snap, const double t);
int data_stress_print (FILE *pfile, const sys_info_t * p_sys_info,
		       const int snap, const double t);
#endif /* DATA_STRESS_H  */
