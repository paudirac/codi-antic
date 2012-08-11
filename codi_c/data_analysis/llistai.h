/* llistai.h  */
#ifndef LLISTAI_H
#define LLISTAI_H
typedef struct llista_ints llistai_t;
llistai_t * llistai_alloc (void);
llistai_t * llistai_alloc_initial_length (const int initial_length);
int llistai_length (llistai_t * llista);
int * llistai_get_ints (llistai_t * llista);
int llistai_add (llistai_t * llista, const int i);
int llistai_free (llistai_t * llista);
int llistai_print (llistai_t * llista, const char * print_format);
int compare_func (const void * a, const void * b);
int llistai_qsort (llistai_t * llista,
		   int (*compare_func)(const void * a, const void * b));
int llistai_min (llistai_t * llista);
int llistai_max (llistai_t * llista);
#endif /* LLISTAI_H  */
