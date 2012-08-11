#ifndef ODEDEF_RANDOM_H
#define ODEDEF_RANDOM_H

int odedef_random_alloc (void);
int odedef_random_free (void);
/* Seguint arXiv:cond-mat/9707147, actualitza la part
 * estocàstica segons \dot x = x_D + h^(1/2) * noise
 */
int odedef_random_evolve_to (const double ti);

/* hook pel rng  */
gsl_rng * odedef_random_get_rng (void);

#endif /* ODEDEF_RANDOM_H  */
