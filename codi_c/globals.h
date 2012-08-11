/* Variables globals. Declarades com a extern en aquest header
 * perquè sigui inclós a tots els altres fitxers.  */
#ifndef GLOBALS_H
#define GOBALS_H

/* 0: no load 1: load from load_file.  */
extern int LOAD;
extern char *load_file;

/* Dimensions físiques  */
extern int NX;
extern int NY;
extern int ny_extra;
//extern int sys_size;
extern int sys_array_size;

extern double dist_a;
extern double range_factor;
extern double GAMMA;
extern int cristall_dist_a_direction;


/* Temps.  */
/* Si e_strain_final != 0., llavors mana sobre T_FINAL.  */
extern double e_strain_final;
extern double t;
extern double T_INICIAL;
extern double T_FINAL;

extern int SNAPSHOTS;
extern double DELTA_T_SNAP;

/** Precisió de l'integrador.  */
extern double EPS_ABS;
extern double EPS_REL;
extern double h;

/* Parets.  */
extern int cells_geometry;
extern double V;
extern int paret_L_mode;
extern int paret_R_mode;

extern char * DIRECTORI;
/* Logfile.  */
extern int LOG;
extern char *log_file_name;
extern int log_gsl_status_to_stderr;
extern int log_print_snapshots;

/* Arxius dades.  */
extern int n_zfill;
extern int print_parets;
extern int histogram_velocities;
extern int histogram_energy;
extern int histogram_stress;
extern int histogram_r;
extern int histogram_n;
extern int histograms_energy_n5n6n7;
extern int historia;
extern int historia_defectes;
extern int historia_velos;

/* Evolució random.  */
extern long int random_seed;
extern int amb_temperatura;
extern double DIFFUSION_CONSTANT;
extern double RATIO;

#endif /* GLOBALS_H  */
