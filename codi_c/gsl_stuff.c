#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>
#include "globals.h"
#include "odedef.h"
#include "ioutils.h"
#include "evolve.h"
#include "sistema.h"
#include "euler_simple.h"

/* Variables del gsl. */
static const gsl_odeiv_step_type *T;
static gsl_odeiv_step *s;
static gsl_odeiv_control *c;
static gsl_odeiv_evolve *e;
static gsl_odeiv_system gsl_system;

int
gsl_alloc (void)
{
  T = gsl_odeiv_step_rk4; // gsl_odeiv_step_rk8pd;
  s = gsl_odeiv_step_alloc (T, sys_array_size);
  c = gsl_odeiv_control_standard_new (EPS_ABS, EPS_REL, 0., 0.);
/*   c = gsl_odeiv_control_euler_simple_new (h); */
  e = gsl_odeiv_evolve_alloc (sys_array_size);

  gsl_system.function = odedef_get_ode_handle ();
  gsl_system.jacobian = 0;
  gsl_system.dimension = sys_array_size;
  gsl_system.params = 0;

  log_to_logfile ("Variables del gsl allocatades.", __FILE__, __LINE__);

  char log_step_name[100];
  sprintf (log_step_name, "step_name: %s", gsl_odeiv_step_name (s));
  log_to_logfile (log_step_name, __FILE__, __LINE__);

  char log_step_order[100];
  sprintf (log_step_order, "step_order: %d", gsl_odeiv_step_order (s));
  log_to_logfile (log_step_order, __FILE__, __LINE__);

  char log_control_name[100];
  sprintf (log_control_name, "control_name: %s", gsl_odeiv_control_name (c));
  log_to_logfile (log_control_name, __FILE__, __LINE__);

  return GSL_SUCCESS;
}

int
gsl_free (void) {
  gsl_odeiv_evolve_free (e);
  gsl_odeiv_control_free (c);
  gsl_odeiv_step_free (s);

  log_to_logfile ("Alliberat l'espai de memòria ocupat per les variables gsl.",
	       __FILE__, __LINE__);
  return GSL_SUCCESS;
}

int
gsl_evolve_to (const double ti)
{
  double *sys = sys_handle ();
  while (t < ti)
    {
      int status = gsl_odeiv_evolve_apply (e, c, s,
					   &gsl_system,
					   &t, ti,
					   &h, sys);
      if (log_gsl_status_to_stderr)
	fprintf (stderr, "gsl_evolve_to control: %f %f %e\n", t, ti, h);
      if (status != GSL_SUCCESS)
	{
	  fprintf (stderr, "******* GSL_SUCCESS != 0 *******\n");
	  fprintf (stderr,"gsl_evolve_to control: %f %f %f\n", t, ti, h);
	  break;
	}
      while (h > 0.01)
	h = h / 2.;
/*       if (h > 0.01) */
/* 	{ */
/* 	  fprintf (stderr,  */
/* 		   "***** el random pot estar no funcionant h = %.5e *****\n", */
/* 		   h); */
/* 	} */
    }

  return 0;
}
