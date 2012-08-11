/** data_volums.h  */
#ifndef DATA_VOLUMS_H
#define DATA_VOLUMS_H
int data_volums_alloc (FILE * pfile);
int data_volums_free (void);
int data_volums_extract (void);
int data_volums_print (FILE * pfile, const int snap);
#endif /* DATA_VOLUMS_H  */
