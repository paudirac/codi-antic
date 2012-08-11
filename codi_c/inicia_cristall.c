/** inicia_cristall.c  */

#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "globals.h"
#include "sistema.h"
#include "inicia_cristall.h"
#include "random_generator.h"
#include "ioutils.h"

int
inicia_cristall_sistema_orientat_x (void)
{
  double * sys = sys_handle ();

  double delta_x = dist_a;
  double delta_y = dist_a * sqrt(3.) / 2.;

  int i;
  int j;

  for (j = 0; j < NY; j++)
    {
      for (i = 0; i < NX; i++)
	{
	  if (j % 2 == 0)
	    {
	      double x = i * delta_x;
	      double y = j * delta_y;
	      int index = i + NX * j;
	      sys[index] = x;
	      sys[index_y(index)] = y;
	    }
	  else
	    {
	      double x = i * delta_x + delta_x / 2.;
	      double y = j * delta_y;
	      int index = i + NX * j;
	      sys[index] = x;
	      sys[index_y(index)] = y;
	    }
	}
    }

  return 0;
}

int
inicia_cristall_paret_commensurada_amb_x (void)
{
  double * sys = sys_handle ();

  double delta_x = dist_a;
  double delta_y = dist_a * sqrt(3.) / 2.;

  double y_min = -ny_extra * delta_y;
  double x_min = -nx_paret * delta_x;

  double x = x_min;
  double y = y_min;
  int index;
  for (index = sys_paret_L_init (); index < sys_paret_L_fin (); index++)
    {
      if (index % nx_paret == 0)
	{
	  x = x_min;
	  y = y + delta_y;
	}
      sys[index] = x;
      sys[index_y(index)] = y;
      if (index / nx_paret % 2 == 0)
	sys[index] = sys[index] + delta_x / 2.;

      x = x + delta_x;
    }

  double x_max = NX * delta_x;
  x = x_max;
  y = y_min;
  for (index = sys_paret_R_init (); index < sys_paret_R_fin (); index++)
    {
      if (index % nx_paret == 0)
	{
	  x = x_max;
	  y = y + delta_y;
	}
      sys[index] = x;
      sys[index_y(index)] = y;
      if (index / nx_paret % 2 == 0)
	sys[index] = sys[index] + delta_x / 2.;

      x = x + delta_x;
    }
  
  return 0;
}

int
inicia_cristall_sistema_orientat_y (void)
{
  double * sys = sys_handle ();

  double delta_x = dist_a * sqrt(3.) / 2.;
  double delta_y = dist_a;

  int i;
  int j;

  for (i = 0; i < NX; i++)
    {
      for (j = 0; j < NY; j++)
	{
	  if (i % 2 == 0)
	    {
	      double x = i * delta_x;
	      double y = j * delta_y;
	      int index = i + NX * j;
	      sys[index] = x;
	      sys[index_y(index)] = y;
	    }
	  else
	    {
	      double x = i * delta_x;
	      double y = j * delta_y + delta_y / 2.;
	      int index = i + NX * j;
	      sys[index] = x;
	      sys[index_y(index)] = y;
	    }
	}
    }

  return 0;
}

int
inicia_cristall_paret_commensurada_amb_y (void)
{
  double * sys = sys_handle ();

  double delta_x = dist_a * sqrt(3.) / 2.;
  double delta_y = dist_a;

  double y_min = -ny_extra * delta_y;
  double y_minR = y_min;
  double x_min = -nx_paret * delta_x;
  double x_max = NX * delta_x;

  int paretLi = sys_paret_L_init ();
  int paretRi = sys_paret_R_init ();

  if (nx_paret % 2 == 1)
      y_min = y_min - delta_y / 2.;

  int i;
  int j;
  for (i = 0; i < nx_paret; i++)
    {
      for (j = 0; j < ny_paret; j++)
	{
	  if (i % 2 == 0)
	    {
	      double x = x_min + i * delta_x;
	      double y = y_min + j * delta_y;
	      double yr = y_minR + j * delta_y;
	      int indexL = i + nx_paret * j + paretLi;
	      int indexR = i + nx_paret * j + paretRi;
	      sys[indexL] = x;
	      sys[index_y(indexL)] = y;
	      sys[indexR] = x + x_max - x_min;
	      sys[index_y(indexR)] = yr;
	    }
	  else
	    {
	      double x = x_min + i * delta_x;
	      double y = y_min + j * delta_y + delta_y / 2.;
	      double yr = y_minR + j * delta_y + delta_y / 2.;
	      int indexL = i + paretLi + nx_paret * j;
	      int indexR = i + paretRi + nx_paret * j;
	      sys[indexL] = x;
	      sys[index_y(indexL)] = y;
	      sys[indexR] = x + x_max - x_min;
	      sys[index_y(indexR)] = yr;
	    }
	}
    }
  
  return 0;
}

int
abort_no_random ()
{
  fprintf (stderr, "Abort imminent.\n");
  abort ();
  return 0;
}

const double MINIMUM_APART = 0.5;

int
inicia_cristall_add_random_perturbation_if_necessary (void)
{
  char *SIGMA_ = getenv ("SIGMA");
  if (SIGMA_ == 0)
    {
      log_to_logfile ("Cristall inicial perfecte. Sinó posar SIGMA != 0.",
		      __FILE__, __LINE__);
      return 0;
    }

  double SIGMA = strtod (SIGMA_, NULL);

  char log_SIGMA[100];
  sprintf (log_SIGMA, "SIGMA = %.5e, és la fracció [0,1) del delta_x(y)" \
	   " que es pertorben les posicions inicials.", SIGMA);
  log_to_logfile (log_SIGMA, __FILE__, __LINE__);

  double * sys = sys_handle ();
  gsl_rng * rng = random_generator_get_generator 
    ("inicia_cristall_add_random_perturbation");

  if (rng == 0) abort_no_random ();

  dimensions * dim = sys_initial_dimensions_handle ();
  int valid_new_x (double new_x)
    {
      return ((new_x > dim->x_min)&&(new_x < dim->x_max));
    }
  int valid_new_y (double new_y)
    {
      return 1;
    }
  int can_be_changed (int i, double new_x, double new_y)
    {
      int j;
      for (j = 0; j < i; j++)
	{
	  double x_j = sys[j];
	  double y_j = sys[index_y(j)];
	  double r2 = (new_x - x_j) * (new_x - x_j) 
	    + (new_y - y_j) * (new_y - y_j);
	  if (r2 < MINIMUM_APART * MINIMUM_APART) return 0;
	}
      return 1;
    }
  
  double sigma_x = delta_x * SIGMA;
  double sigma_y = delta_y * SIGMA;
  int i;
  for (i = 0; i < phys_size; i++)
    {
      double random_x = gsl_ran_gaussian (rng, sigma_x);
      double random_y = gsl_ran_gaussian (rng, sigma_y);
      double new_x = sys[i] + random_x;
      double new_y = sys[index_y(i)] + random_y;
      if (valid_new_x(new_x) && valid_new_y(new_y))
	{
	  if (can_be_changed (i, new_x, new_y))
	    {
	      sys[i] = new_x;
	      sys[index_y(i)] = new_y;
	    }
	}
    }

  return 0;
}

int
inicia_cristall_add_vacancies (void)
{
  char * VACANCIES_ = getenv ("VACANCIES");
  if (VACANCIES_ == 0)
    {
      log_to_logfile ("Cristall perfecte. Sinó posar VACANCIES != 0.",
		      __FILE__, __LINE__);
      return 0;
    }

  int VACANCIES = atoi (VACANCIES_);

  char log_VACANCIES[100];
  sprintf (log_VACANCIES, "Hi ha vacants. VACANCIES = %d.", VACANCIES);
  log_to_logfile(log_VACANCIES, __FILE__, __LINE__);

  /* Trec les vacants.  */
  double * sys = sys_handle ();
  gsl_rng * rng = random_generator_get_generator 
    ("inicia_cristall_add_vacancies");

  if (rng == 0) abort_no_random ();

  int choices[VACANCIES], possibilities[phys_size];
  int i;
  for (i = 0; i < phys_size; i++) possibilities[i] = i;

  gsl_ran_choose (rng, choices, (size_t) VACANCIES,
		  possibilities, (size_t) phys_size, sizeof (int));

  for (i = 0; i < VACANCIES; i++)
    /* Move the last ones to the vacancy indexs.  */
    {
      int vacancy = choices[i];
      int replacement = phys_size - i - 1;
      sys[vacancy] = sys[replacement];
      sys[index_y(vacancy)] = sys[index_y(replacement)];
    }

  /* Renormalize system.  */
  sys_renormalize_for_vacancies (VACANCIES);

  return 0;
}

int
inicia_cristall_add_surface_vacancies (void)
{
  char * SVACANCIES_ = getenv ("SVACANCIES");
  if (SVACANCIES_ == 0)
    {
      log_to_logfile ("Cristall perfecte. Sinó posar SVACANCIES != 0.",
		      __FILE__, __LINE__);
      return 0;
    }

  int SVACANCIES = atoi (SVACANCIES_);

  char log_SVACANCIES[100];
  sprintf (log_SVACANCIES, "Hi ha vacants, a la superfície.\n" \
	   "SVACANCIES = %d.", SVACANCIES);
  log_to_logfile(log_SVACANCIES, __FILE__, __LINE__);

  /* Trec les vacants.  */
  double * sys = sys_handle ();
  gsl_rng * rng = random_generator_get_generator 
    ("inicia_cristall_add_surface_vacancies");

  if (rng == 0) abort_no_random ();

  int choices[SVACANCIES], possibilities[phys_size];
  int i;
  int n_choices = 0;
  for (i = 0; i < phys_size; i++) 
    {
      //      int fila = i / NX;
      int columna = i % NX;
      if ((columna == 0)||(columna == (NX - 1)))
	{
	  possibilities[n_choices] = i;
	  n_choices++;
	}
    }

  gsl_ran_choose (rng, choices, (size_t) SVACANCIES,
		  possibilities, (size_t) n_choices, sizeof (int));

  for (i = 0; i < SVACANCIES; i++)
    /* Move the last ones to the vacancy indexs.  */
    {
      int vacancy = choices[i];
      int replacement = phys_size - i - 1;
      sys[vacancy] = sys[replacement];
      sys[index_y(vacancy)] = sys[index_y(replacement)];
    }

  /* Renormalize system.  */
  sys_renormalize_for_vacancies (SVACANCIES);

  return 0;
}

int
inicia_cristall_add_surface_steps_pero_nomes_un (void)
{
  char * STEPS_ = getenv("STEPS");
  if (STEPS_ == 0)
    {
      log_to_logfile ("Cristall sense steps. Sinó posar STEPS != 0.",
		      __FILE__, __LINE__);
      return 0;
    }

  int STEPS = atoi (STEPS_);
  
  char log_STEPS[100];
  sprintf (log_STEPS, "Hi ha %d steps.\nSTEPS = %d.", STEPS, STEPS);
  log_to_logfile(log_STEPS, __FILE__, __LINE__);

  double * sys = sys_handle ();
  gsl_rng * rng = random_generator_get_generator
    ("inicia_cristall_add_surface_steps");

  if (rng == 0) abort_no_random ();

  int nx = gsl_rng_uniform_int (rng, NX); // potser hi va un cast (int)
  
  int i;
  for (i = 0; i < phys_size; i++)
    {
      int columna = i % NX;
      if (columna > nx) sys[index_y(i)] = sys[index_y(i)] + delta_y;
    }

  char log_step_position[100];
  sprintf (log_step_position, "Posició de l'step nx = %d.", nx);
  log_to_logfile(log_step_position, __FILE__, __LINE__);

  return 0;
}

int
inicia_cristall_add_surface_steps (void)
{
  char * STEPS_ = getenv("STEPS");
  if (STEPS_ == 0)
    {
      log_to_logfile ("Cristall sense steps. Sinó posar STEPS != 0.",
		      __FILE__, __LINE__);
      return 0;
    }

  int STEPS = atoi (STEPS_);
  
  char log_STEPS[100];
  sprintf (log_STEPS, "Hi ha %d steps.\nSTEPS = %d.", STEPS, STEPS);
  log_to_logfile(log_STEPS, __FILE__, __LINE__);

  gsl_rng * rng = random_generator_get_generator
    ("inicia_cristall_add_surface_steps");

  if (rng == 0) abort_no_random ();

  int choices[STEPS], possibilites[NX];
  int i;
  for (i = 0; i < NX; i++) possibilites[i] = i;
  gsl_ran_choose (rng, choices, (size_t) STEPS,
		  possibilites, (size_t) NX, sizeof(int));

  double signes[4] = {-1., 0., 1., 0.};
  double signe_step (int columna)
    {
      int counter = 0;
      int i = 0;
      while (i < STEPS)
	{
	  if (choices[i] < columna) counter++;
	  i++;
	}
      return signes[counter % 4];
    }
    
  double * sys = sys_handle ();

  for (i = 0; i < phys_size; i++)
    {
      int columna = i % NX;
      double signe = signe_step(columna);
      sys[index_y(i)] = sys[index_y(i)] + signe * delta_y;
    }

  for (i = 0; i < STEPS; i++)
    {
      char log_step_position[100];
      sprintf (log_step_position, "Posició de l'step %d nx = %d.", 
	       i, choices[i]);
      log_to_logfile(log_step_position, __FILE__, __LINE__);
    }

  return 0;
}
