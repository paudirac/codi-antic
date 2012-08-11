/** inicia_cristall.h  */
#ifndef INICIA_CRISTALL_H
#define INICIA_CRISTALL_H
extern int nx_paret;
extern int ny_paret;
int inicia_cristall_sistema_orientat_x (void);
int inicia_cristall_sistema_orientat_y (void);
int inicia_cristall_paret_commensurada_amb_x (void);
int inicia_cristall_paret_commensurada_amb_y (void);
int inicia_cristall_add_random_perturbation_if_necessary (void);
int inicia_cristall_add_vacancies (void);
int inicia_cristall_add_surface_vacancies (void);
int inicia_cristall_add_surface_steps (void);
#endif /** INICIA_CRISTALL_H  */

