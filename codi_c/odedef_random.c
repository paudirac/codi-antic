/* odedef_random.c  */
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "ioutils.h"
#include "odedef_random.h"
#include "sistema.h"
#include "globals.h"
#include "random_generator.h"

static int hi_ha_evolucio_estocastica = 0;
static double t_ant;

int random_evolve (double sys[], const double t);

int
odedef_random_evolve_to (const double ti)
{
  if (hi_ha_evolucio_estocastica)
    random_evolve (sys_handle(), t);
  return 0;
}

int
odedef_random_alloc (void)
{
  t_ant = T_INICIAL;

  char *RATIO_ = getenv ("RATIO");
  if (RATIO_ == 0) return 0;

  RATIO = strtod (RATIO_, NULL);
  hi_ha_evolucio_estocastica = 1;

  char log_RATIO[100];
  sprintf (log_RATIO, "RATIO = %.5e", RATIO);
  log_to_logfile (log_RATIO, __FILE__, __LINE__);
  log_to_logfile ("Nota: ara RATIO és el que en les meves notes és \\chi.",
		  __FILE__, __LINE__); 

  log_to_logfile ("Allocatada l'evolució estocàstica.", 
		  __FILE__, __LINE__);
    
  return 0;
}

int
odedef_random_free (void)
     /* metode que es mante per consistencia i és una reminiscencia  */
{
  return 0;
}

int
random_evolve (double sys[], const double t)
     /** Versió bona, suposo.  */
     /* Seguint arXiv:cond-mat/9707147, actualitza la part
      * estocàstica segons \dot x = x_D + h^(1/2) * noise.
      */
{
  gsl_rng * rng = random_generator_get_generator ("random_evolve");
  double delta_t_ = t - t_ant;
  double sqrt_h = sqrt(RATIO * delta_t_);

  dimensions * dim = sys_dimensions_handle ();
  double xl = dim -> x_phys_min;
  double xr = dim -> x_phys_max;
  
  int i = 0;
  for (i = 0; i < phys_size; i++) {
    double x = sys[i];
    if ((xl < x) && (x < xr)) {
      double ran_x = gsl_ran_gaussian (rng, 1.);
      double ran_y = gsl_ran_gaussian (rng, 1.);
      sys[i] = sys[i] + sqrt_h * ran_x;
      sys[index_y(i)] = sys[index_y(i)] + sqrt_h * ran_y;
    }
  }

  t_ant = t;
  
  return 0;
}

