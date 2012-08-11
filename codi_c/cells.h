/* cells.h  */
#include "odedef.h"
#ifndef CELLS_H
#define CELLS_H

/* extern int *head; */
/* extern int *list; */
/* extern int *neighbour_cell; */

extern int N_CELLS;

/* Allocata les cells. Aborta si no hi ha prou espai.  */
int cells_alloc (void);
int cells_free (void);

int cells_calculate ();
int cells_pinta_neighbour_cells (void); /* For test purposes only.  */
int cells_loop_force (double t, const double sys[], double f[], 
		      void *params, forca_function_pointer pf);

/* Nota: forca_function_pointer s'hauria de definir aquí perquè
 * és utilitzada per cells_loop_force i aquest mètode el pot cridar
 * algú que no estigui a odedef.c o que no importi odedef.h
 * Està mal encapsulat. Possiblement canviar-li el nom de forca, com
 * per funcio_a_loopejar.
 * cells_loop_function i el típus function_to_loop_pointer solucionen
 * aquest problema.
 * A més, function_to_loop_pointer conté un punter a void, other, que
 * pot servir per passar histogrames o altres típus a la funció que
 * es lopeja.  */

typedef int (*function_to_loop_pointer)
     (double t, const double sys[], double f[], void *other,
      const int a_index, const int b_index);
int cells_loop_function (double t, const double sys[],
			 double f[], void *other,
			 function_to_loop_pointer pf);
#endif /* CELLS_H  */
