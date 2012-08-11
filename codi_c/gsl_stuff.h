/* Funcions relacionades amb el gsl que poden veures
 * des del main.  */
#ifndef GSL_STUFF_H
#define GSL_STUFF_H

/* Allocata espai per al gsl.  */
int gsl_alloc (void);
/* Allibera espai del gsl un cop acabat.  */
int gsl_free (void);

int gsl_evolve_to (const double ti);
#endif /* GSL_STUFF_H  */
