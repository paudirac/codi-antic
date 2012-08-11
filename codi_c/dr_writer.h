/* dr_writer.h  */
#ifndef DR_WRITER_H
#define DR_WRITER_H
int dr_writer_init (FILE * pfile);
int dr_writer_write_frame (FILE * pfile, const int snap, const double * sys);
int dr_writer_fin (FILE * pflie);
#endif /* DR_WRITER_H  */
