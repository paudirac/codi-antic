#include "odedef_parets.h"
#include "boundaries.h"
#include "sistema.h"

/* Variables globals.  */

/* Load method.
 * 0: no load.
 * 1: load from load_file.  */
int LOAD = 0; /* De moment només pot ser 0, NOT_IMPLEMENTED_YET.  */
char *load_file = 0;

/* Dimensions.  */ /* Mida màxima provada NX = 1000, NY = 1000.  */
int NX = 16; // i un 64 per forçar la màquina què, eh? ;P
int NY = 16;
int ny_extra = 5;
/* Dimensions del sys, que té en compte les
 * files extra que hi pugui haber.  */
//int sys_size;
int sys_array_size;

double dist_a = 1.12246;
double range_factor = 3.;
double GAMMA = 1.;
int cristall_dist_a_direction = Y;

/* Temps.  */
/* Si e_strain_final != 0., llavors mana sobre T_FINAL.  */
double e_strain_final = 0.30;
double t = 0.;
double T_INICIAL = 0.;
double T_FINAL = 0.0001;//35.00;

int SNAPSHOTS = 1200;
double DELTA_T_SNAP = 0.;//005;
int n_zfill = 4;

/* Precisió de l'integrador.  */ // crec que amb 1.e-5 és suficient, pero ara ja els he fet tots amb 1.e-6, provar-ho? i dir que tot està a 1e-5?
#define DEFAULT_PRECISION 1e-5
double EPS_ABS = DEFAULT_PRECISION;//1e-6;//1.0e-6; amb 1e-3 peta
double EPS_REL = DEFAULT_PRECISION;//1e-6;//1.0e-6;
double h = 0.001;//1.0e-6;

/* Parets.  */
int cells_geometry = FREE;
double V = 0.0;
int paret_L_mode = LEFT_XU;
int paret_R_mode = RIGHT_XU;

/* Logfile.  */
int LOG = 1;
int log_gsl_status_to_stderr = 0;
int log_print_snapshots = 0;

/* Flags arxius dades.  */
int print_parets = 1;
int histogram_velocities = 0;
int histogram_energy = 0;
int histogram_stress = 0;
int histogram_r = 0;
int histogram_n = 0;
int histograms_energy_n5n6n7 = 0;
int historia = 1;
int historia_defectes = 1;
int historia_velos = 0;

/* Evolució random.  */
/** Per tal que hi hagi evolucio random la llavor ha de ser
 *  no nul.la i s'ha de passar en el GSL_RNG_SEED.  */
long int random_seed = 0; /** 0 es el default */
double DIFFUSION_CONSTANT = 1.0e-4;
double RATIO = 0.000; /* Ratio entre k_B T / epsion  */
