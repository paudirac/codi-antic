/*************************************************
 * File: main.c
 *
 * Description:
 *   - main.
 *   - versio overdamped.
 *
 * Status:
 *   - el programa funciona.
 *
 * Author: Pau Cervera
 * Date: 27.12.2005
 *
 *************************************************/
#include <string.h>
#include "output_settings.h"
#include "sistema.h"
#include "gsl_stuff.h"
#include "odedef.h"
#include "odedef_parets.h"
#include "random_generator.h"
#include "odedef_random.h"
#include "cells.h"
#include "ioutils.h"
#include "evolve.h"
#include "data.h"
#include "boundaries.h"
#include "energy.h"
#include "inicia_cristall.h"

int
main (void)
{
  output_settings_init ();
  log_to_logfile ("Comença l'execució del programa.", __FILE__, __LINE__);

  random_generator_alloc ();
  sys_alloc ();
  sys_init_configuration (); 

  gsl_alloc ();
  odedef_alloc ();
  odedef_parets_alloc ();
  odedef_random_alloc (); 
  energy_alloc ();
  cells_alloc ();
  boundaries_alloc ();
  evolve_setup ();// s'ha de cridar abans de data_alloc, sino no printare be V
  data_alloc ();

  evolve ();
  
  sys_free ();
  gsl_free ();
  random_generator_free ();
  odedef_free ();
  odedef_parets_free ();
  odedef_random_free ();
  energy_free ();
  cells_free ();
  data_free ();
  boundaries_free ();

  log_to_logfile ("Acabo l'execució.", __FILE__, __LINE__);
  return 0;
}
