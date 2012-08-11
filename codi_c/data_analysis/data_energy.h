/* data_energy.h  **/
#ifndef DATA_ENERGY_H
#define DATA_ENERGY_H
#include "dr_reader.h"
int data_energy_alloc (FILE * pfile, const sys_info_t * p_sys_info);
int data_energy_free (void);
int data_energy_extract (const sys_info_t * p_sys_info,
			 const double * sys, const double * f,
			 const int snap, const double t);
int data_energy_print (FILE *pfile, const sys_info_t * p_sys_info,
		       const int snap, const double t);
#endif /* DATA_ENERGY_H  */
