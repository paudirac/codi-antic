#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_odeiv.h>
/* S'ha de retocar perquè inclogui un EPS_ABS i que si es supera
 * llavors pari.  */

typedef struct
{
  double h_euler_simple;
}
euler_control_state_t;

static void *
euler_simple_control_alloc (void)
     /* Retorno no null (no 0) perquè sinó el
      * gsl_odeiv_control_alloc em retorna un error.  */
{
  euler_control_state_t * s = 
    (euler_control_state_t *) malloc (sizeof (euler_control_state_t));

  if (s == 0)
    GSL_ERROR_NULL ("failed to allocate space for euler_control_state",
		    GSL_ENOMEM);

  return s;
}

static int
euler_simple_control_init (void *vstate,
			   double eps_abs, double eps_rel, 
			   double a_y, double a_dydt)
{
  if (eps_abs < 0.)
    GSL_ERROR ("eps_abs is negative, no importa per euler", GSL_EINVAL);
  else if (eps_rel < 0.)
    GSL_ERROR ("eps_rel is negative, no importa per euler", GSL_EINVAL);
  else if (a_y < 0.)
    GSL_ERROR ("a_y is negative, no importa per euler", GSL_EINVAL);
  else if (a_dydt < 0.)
    GSL_ERROR ("a_dydt is negative, no importa per euler", GSL_EINVAL);

  return GSL_SUCCESS;
}

static int
euler_simple_hadjust (void *vstate, size_t dim, unsigned int ord,
		      const double y[], const double yerr[],
		      const double yp[], double *h)
{
  euler_control_state_t * state = (euler_control_state_t *) vstate;
  *h = state->h_euler_simple;
  return GSL_ODEIV_HADJ_NIL;
}

static void
euler_simple_free (void *vstate)
{
  return;
}

static const gsl_odeiv_control_type euler_simple_type =
  { "euler simple",
    &euler_simple_control_alloc,
    &euler_simple_control_init,
    &euler_simple_hadjust,
    &euler_simple_free
  };

const gsl_odeiv_control_type 
*gsl_odeiv_control_euler_simple = &euler_simple_type;

gsl_odeiv_control *
gsl_odeiv_control_euler_simple_new (const double h_euler)
{
  gsl_odeiv_control *c =
    gsl_odeiv_control_alloc (gsl_odeiv_control_euler_simple);

  int status = gsl_odeiv_control_init (c, 0., 0., 0., 0.);
  if (status != GSL_SUCCESS)
    {
      gsl_odeiv_control_free (c);
      GSL_ERROR_NULL ("error trying to initialize control in euler", status);
    }

  euler_control_state_t * s = (euler_control_state_t *) c->state;
  s->h_euler_simple = h_euler;

  return c;
}
