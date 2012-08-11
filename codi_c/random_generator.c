/* random_generator.c  */
#include <stdlib.h>
#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>
#include "globals.h"
#include "ioutils.h"

static const gsl_rng_type * rng_type = NULL;
static gsl_rng * rng = NULL;

int
no_random_seed (void)
{
  log_to_logfile ("random_generator NO ALLOCATAT.", __FILE__, __LINE__);
  log_to_logfile ("Per allocatar-lo, posar GSL_RNG_SEED != 0.", 
		  __FILE__, __LINE__);
  return 0;
}

int
no_rng_error_abortion (char * caller)
{
  char log_error[100];
  sprintf (log_error, "[Error] %s crida a un random generator NO ALLOCATAT.",
	   caller);
  log_to_logfile (log_error, __FILE__, __LINE__);
  log_to_logfile ("Per allocatar-lo, posar GSL_RNG_SEED != 0.",
		  __FILE__, __LINE__);
  log_to_logfile ("Abort imminent.", __FILE__, __LINE__);
  fprintf (stderr, "%s\nAbort imminent.\n", log_error);
  abort ();
  return 0;
}

int
random_generator_alloc (void)
{
  gsl_rng_env_setup ();
  random_seed = gsl_rng_default_seed;
  
  if (!gsl_rng_default_seed)
    {
      no_random_seed ();
      rng = NULL;
      return 0;
    }

  rng_type = gsl_rng_default;
  rng = gsl_rng_alloc (rng_type);

  log_to_logfile ("Allocatat generador random (gsl_rng).", 
		  __FILE__, __LINE__);

  char log_gsl_rng_name[100];
  sprintf (log_gsl_rng_name, "gsl_rng_name = %s", gsl_rng_name (rng));
  log_to_logfile (log_gsl_rng_name, __FILE__, __LINE__);

  char log_gsl_seed[100];
  sprintf (log_gsl_seed, "GSL_RNG_SEED = %d", (int) gsl_rng_default_seed);
  log_to_logfile (log_gsl_seed, __FILE__, __LINE__);

  return 0;
}

int
random_generator_free (void)
{
  if (rng)
    {
      gsl_rng_free (rng);
      log_to_logfile ("Alliberat l'espai de memòria del generador" \
		      " random.", __FILE__, __LINE__);
    }

  return 0;
}

gsl_rng * 
random_generator_get_generator (char * caller)
{ 
  if (!rng) no_rng_error_abortion (caller);
  return rng;
}


