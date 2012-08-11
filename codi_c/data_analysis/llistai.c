/* llistai.c  */
#include <stdlib.h> /* where qsort resides  */
#include <stdio.h>
#include "llistai.h"

#define FREE_POINTER(p) do {if ((p) != 0) { free ((p)); (p) = 0;}} while (0)

static int ordre_normal (const void * a, const void * b);

/* default length  */
static const int LLISTA_MAX_LENGTH = 100;

/* el tipus es llistai_t, definit a llistai.h  */
struct llista_ints {
  int length;
  int max_length;
  int * llista;
  int ordered_once;
};

int llistai_length (llistai_t * llista) { return llista->length; }
int * llistai_get_ints (llistai_t * llista) { return llista->llista; }

llistai_t *
llistai_alloc (void)
{
  return llistai_alloc_initial_length (LLISTA_MAX_LENGTH);
}

llistai_t *
llistai_alloc_initial_length (const int initial_length)
{
  llistai_t * llista_ = (llistai_t *) malloc (sizeof (llistai_t));
  llista_->llista = (int *) malloc (initial_length * sizeof (int));
  if ((llista_ == NULL) || (llista_->llista == NULL)) return 0;
  else
    {
      llista_->length = 0;
      llista_->max_length = initial_length;
      llista_->ordered_once = 0;
    }

  return llista_;
}

int
llistai_add (llistai_t * llista, const int i)
{
  if (llista->length < llista->max_length)
    {
      llista->llista[llista->length] = i;
      llista->length = llista->length + 1;
    }
  else
    {
      llista->max_length = llista->max_length + LLISTA_MAX_LENGTH;
      llista->llista = (int *) realloc 
	(llista->llista, llista->max_length * sizeof (int));
      llistai_add (llista, i);
    }

  return 0;
}

int
llistai_free (llistai_t * llista)
{
  FREE_POINTER (llista->llista);
  FREE_POINTER (llista);

  return 0;
}

int
llistai_print (llistai_t * llista, const char * print_format)
{
  const char * default_format = "%d";
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

int llistai_qsort (llistai_t * llista,
		   int (*compare_func) (const void * a, const void * b))
{
  if (compare_func == 0) compare_func = &ordre_normal;
  qsort (llista->llista, llista->length, sizeof (int), compare_func);
  llista->ordered_once = 1;
  return 0;
}

int
ordre_normal (const void * a, const void * b)
{
  const int * ia = (const int *) a;
  const int * ib = (const int *) b;

  return (*ia > *ib) - (*ia < *ib);
}

int
llistai_min (llistai_t * llista)
{
  if (llista->ordered_once) return llista->llista[0];
  else
    {
      llistai_qsort (llista, 0);
      return llistai_min (llista);
    }
}

int
llistai_max (llistai_t * llista)
{
  if (llista->ordered_once) return llista->llista[llista->length - 1];
  else
    {
      llistai_qsort (llista, 0);
      return llistai_max (llista);
    }
}
