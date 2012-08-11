/** data_stress_molecular.h  */
#ifndef DATA_STRESS_MOLECULAR_H
#define DATA_STRESS_MOLECULAR_H
#include "dr_reader.h"
int data_stress_molecular_alloc (FILE * pfile, const sys_info_t * p_sys_info);
int data_stress_molecular_free (void);
int data_stress_molecular_extract (const sys_info_t * p_sys_info, 
				   const double * sys, const double * f,
				   const int snap, const double t);
int data_stress_molecular_print (FILE * pfile, const sys_info_t * p_sys_info,
				 const int snap, const double t);
int data_stress_molecular_set_stress_history (FILE * pfile,
					      sys_info_t * p_sys_info);
#endif /* DATA_STRESS_MOLECULAR_H  */
