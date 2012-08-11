/* data_r.h  */
#ifndef DATA_R_H
#define DATA_R_H
int data_r_alloc (FILE *pfile);
int data_r_free (void);
int data_r_extract (void);
int data_r_print (FILE *pfile, const int snap);
#endif /* DATA_R_H  */
