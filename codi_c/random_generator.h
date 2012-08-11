/* random_generator.h  */
int random_generator_alloc (void);
int random_generator_free (void);
gsl_rng * random_generator_get_generator (char * caller);
