/* dr_writer.h  */
#ifndef DR_WRITER_H
#define DR_WRITER_H
#include "dr_reader.h"
int dr_writer_init (FILE * pfile, const sys_info_t * p_sys_info);
int dr_writer_write_frame (FILE * pfile, const double * sys,
			   const sys_info_t * p_sys_info, const int snap,
			   const double t);
int dr_writer_fin (FILE * pfile);
typedef int (*writer_funcp_t) (FILE * pfile, const double * sys,
			       const sys_info_t * p_sys_info);
int dr_writer_set_writer (writer_funcp_t writer);
#endif /* DR_WRITER_H  */
