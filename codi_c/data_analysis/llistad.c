/* llistad.c  */
#include <stdlib.h> /* where qsort resides  */
#include <stdio.h>
#include "llistad.h"

#define FREE_POINTER(p) do { if ((p) != 0) { free((p)); (p) = 0;}} while (0)

static int ordre_normal (const void * a, const void * b);

/* default length  */
static const int LLISTA_MAX_LENGTH = 100;

/* el tipus es llistad_t, definit a llistad.h  */
struct llista_doubles {
  int length;
  int max_length;
  double * llista;
  int ordered_once;
};

int llistad_length (llistad_t * llista) { return llista->length; }
double * llistad_get_doubles (llistad_t * llista) { return llista->llista; }

llistad_t * 
llistad_alloc (void)
{
  return llistad_alloc_initial_length (LLISTA_MAX_LENGTH); 
}

llistad_t *
llistad_alloc_initial_length (const int initial_length)
{
  llistad_t * llista_ = (llistad_t *) malloc (sizeof (llistad_t));
  llista_->llista = (double *) malloc (initial_length * sizeof (double));
  if ((llista_ == NULL)||(llista_->llista == NULL)) return 0;
  else 
    {
      llista_->length = 0;
      llista_->max_length = initial_length;
      llista_->ordered_once = 0;
    }

  return llista_;
}

int
llistad_add (llistad_t * llista, const double d)
{
  if (llista->length < llista->max_length)
    {
      llista->llista[llista->length] = d;
      llista->length = llista->length + 1;
    }
  else 
    {
      llista->max_length = llista->max_length + LLISTA_MAX_LENGTH;
      llista->llista = (double *) realloc 
	(llista->llista, llista->max_length * sizeof (double));
      llistad_add (llista, d);
    }

  return 0;
}

int
llistad_free (llistad_t * llista)
{
  FREE_POINTER (llista->llista);
  FREE_POINTER (llista);

  return 0;
}

int
llistad_print (llistad_t * llista, const char * print_format)
{
  const char * default_format = "%.16f";
  FILE * pfile = stdout;

  if (print_format == 0) print_format = default_format;

  int i;
  fprintf (pfile, "[");
  for (i = 0; i < llista->length - 1; i++)
    {
      fprintf (pfile, print_format, llista->llista[i]);
      fprintf (pfile, ", ");
    }
  fprintf (pfile, print_format, llista->llista[llista->length - 1]);
  fprintf (pfile, "]\n");

  return 0;
}

int llistad_qsort (llistad_t * llista,
		   int (*compare_func)(const void * a, const void * b))
{
  if (compare_func == 0)
    compare_func = &ordre_normal;
  qsort (llista->llista, llista->length, sizeof (double), compare_func);
  llista->ordered_once = 1;
  return 0;
}

int ordre_normal (const void * a, const void * b)
{
  const double * da = (const double *) a;
  const double * db = (const double *) b;

  return (*da > *db) - (*da < *db);
}

double
llistad_min (llistad_t * llista)
{
  if (llista->ordered_once) return llista->llista[0];
  else
    {
      llistad_qsort (llista, 0);
      return llistad_min (llista);
    }
}

double
llistad_max (llistad_t * llista)
{
  if (llista->ordered_once) return llista->llista[llista->length - 1];
  else
    {
      llistad_qsort (llista, 0);
      return llistad_max (llista);
    }
}
