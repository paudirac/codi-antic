/* memoria.c  */
#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"

void *
xmalloc (size_t size)
{
  register void *value = malloc (size);
  if (value == 0)
    {
      fprintf (stderr, "error d'allocatament\n");
      abort ();
    }
  return value;
}

static int call_n = 0;
void *
xmalloc_ (size_t size)
     /* Wrapper de malloc for debuggin purposes.  */
{
  call_n++;
  fprintf (stderr, "call_n: %d\n", call_n);
  fprintf (stderr, "vull allocatar %d bites\n", (int) size);
  register void * to_return = (void *) xmalloc_ (size);
  if (to_return == 0)
    {
      fprintf (stderr, "no s'han pogut allocatar els %d bites\n", (int) size);
      return 0;
    }
  fprintf (stderr, "%d bites allocatats satisfactoriament. addr: %p\n", 
	   (int) size, to_return);
  fflush (stderr);

  return to_return;
}
