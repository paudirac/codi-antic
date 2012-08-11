/* data_rug.h  */
#ifndef DATA_RUG_H
#define DATA_RUG_H
#include "dr_reader.h"
int data_rug_alloc (FILE *pfile, const sys_info_t * p_sys_info);
int data_rug_free (void);
int data_rug_extract (const sys_info_t * p_sys_info, const double * sys,
		      const double * f, const int snap, const double t);
int data_rug_print (FILE *pfile, const sys_info_t * p_sys_info,
		    const int snap, const double t);
#endif /* DATA_RUG_H  */
