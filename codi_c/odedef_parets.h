#ifndef ODEDEF_PARETS_H
#define ODEDEF_PARETS_H
enum {
  LEFT_XU   = 1, /* modes xu  */
  RIGHT_XU  = 2,

  LEFT_Y    = 3, /* modes y  */
  RIGHT_Y   = 4
};

int odedef_parets_alloc (void);
int odedef_parets_forces (double t, const double sys[],
			  double f[], void *params);
char *odedef_parets_mode_name (const int mode);
//__inline__ int odedef_parets_esborra_forces_espuries (double f[]);
#define odedef_parets_esborra_forces_espuries(f) do { \
  int iii; \
  for (iii = sys_paret_L_init(); iii < sys_paret_L_fin(); iii++) \
    f[iii] = f[index_y(iii)] = 0.; \
  for (iii = sys_paret_R_init(); iii < sys_paret_R_fin(); iii++) \
    f[iii] = f[index_y(iii)] = 0.; \
}while (0)

/* int odedef_parets_evolve_to (const double t); */
int odedef_parets_free (void);
#endif /* ODEDEF_PARETS_H  */
