#ifndef BOUNDARIES_H
#define BOUNDARIES_H
int boundaries_alloc (void);
int boundaries_free (void);
int boundaries_resolve_boundary_conflicts (void);

enum {
  COUETTE                  = 1, /* y periodic  */
  BOTTOM_REFLEXIVE_UP_FREE = 2,
  FREE                     = 3,
  SHEAR                    = 4, /* shear  */
};

double xi_ab (const int a, const int b, const double sys[]);
double r2_ab (const int a, const int b, const double sys[]);

#endif /* BOUNDARIES_H  */
