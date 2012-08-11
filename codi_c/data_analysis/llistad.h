/* llistad.h  */
#ifndef LLISTAD_H
#define LLISTAD_H
#include <stdlib.h>
typedef struct llista_doubles llistad_t;
llistad_t * llistad_alloc (void);
llistad_t * llistad_alloc_initial_length (const int inital_length);
int llistad_length (llistad_t * llista);
double * llistad_get_doubles (llistad_t * llista);
int llistad_add (llistad_t * llista, const double d);
int llistad_free (llistad_t * llista);
int llistad_print (llistad_t * llista, const char * print_format);
int compare_func (const void * a, const void * b);
int llistad_qsort (llistad_t * llista, 
		   int (*compare_func)(const void * a, const void * b));
double llistad_min (llistad_t * llista);
double llistad_max (llistad_t * llista);
#endif /* LLISTAD_H  */
