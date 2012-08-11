/* data_velos.h  **/
#ifndef DATA_VELOS_H
#define DATA_VELOS_H
#include "dr_reader.h"
int data_velos_alloc (FILE *pfile, const sys_info_t * p_sys_info);
int data_velos_free (void);
int data_velos_extract (const sys_info_t * p_sys_info, const double * sys,
			const double * f, const int snap, const double t);
int data_velos_print (FILE *pfile, const sys_info_t * p_sys_info, 
		      const int snap, const double t);
#endif /* DATA_VELOS_H  */
