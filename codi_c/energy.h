/* energy.h  */
#ifndef ENERGY_H
#define ENERGY_H
int energy_alloc (void);
int energy_free (void);
int energy_func (double t, const double sys[], 
		 double ene[], void *params);
#endif /* ENERGY_H  */
