#include <stdio.h>
/* Mètodes d'allocatament inicial i finalització.  */
#ifndef SISTEMA_H
#define SISTEMA_H

/* Sistema.  */
//extern double *sys;
extern double delta_x;
extern double delta_y;
enum {
  X = 1,
  Y = 2,
};

/* Allocata espai per al sistema.  */
int sys_alloc (void);
/* Allibera espai de memòria ocupat pel sistema.  */
int sys_free (void);

double *sys_handle (void);
extern int sys_size;
extern int paret_size;
#define index_y(index_x) ((index_x) + sys_size)
int index_i (const int i, const int j);

extern int phys_size;
#define no_es_paret(index) ((index) < phys_size)
#define sys_paret_L_init() ((phys_size))
#define sys_paret_L_fin()  ((phys_size + paret_size))
#define sys_paret_R_init() ((phys_size + paret_size))
#define sys_paret_R_fin()  ((phys_size + 2 * paret_size))
/* int sys_paret_L_init (void); */
/* int sys_paret_L_fin (void); */
/* int sys_paret_R_init (void); */
/* int sys_paret_R_fin (void); */

/* Inicialitza la configuració del sistema.  */
int sys_init_configuration (void);


/* Distancies. */
double sys_x_distance (void);
double sys_y_distance (void);

struct dimensions_ {
  double x_phys_max;
  double x_phys_min;
  double y_phys_max;
  double y_phys_min;
  double x_max;
  double x_min;
  double y_max;
  double y_min;
};
typedef struct dimensions_ dimensions;
dimensions *sys_dimensions_handle (void);
dimensions *sys_initial_dimensions_handle (void);
int sys_calc_dimensions (void);
int sys_resolve_pbc (void);

double sys_engineering_strain (void);
int inicia_sys_new_crystall (double *sys_);
int sys_renormalize_for_vacancies (int vacancies);

#endif /* SISTEMA_H  */

/** Deprecated methods.  */
/* Pinta el sistema.  */
/* int sys_print (const int snap); */
