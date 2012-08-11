/** output_settings.c  */
#include <string.h>
#include <stdlib.h>
#include "globals.h"

char *log_file_name;
char *out_prefixe;
char *out_sufixe;
char *fitxer_data;
char *histograms_file;
char *historia_file;
char *historia_defectes_file;
char *historia_velos_file;

#define LOG_FILENAME_ "test.log"
#define PREFIXE_ "atoms"
#define SUFIXE_ ".dat"
#define FITXER_DATA_ "data"
#define HISTOGRAMS_ "histogrames"
#define HISTORIA_ "posicions" //"historia"
#define HISTORIA_DEFECTES_ "historia_defectes"
#define HISTORIA_VELOS_ "forces" // "historia_velos"

int
output_settings_init (void)
{
  char *env_dir = getenv("DIRECTORI");
  if (env_dir == 0)
    env_dir = "./none_directory/";

  int dir_len = 1000; //strlen(env_dir);

  log_file_name = (char *) malloc 
    ((strlen(LOG_FILENAME_) + dir_len) * sizeof (char));
  out_prefixe = (char *) malloc 
    ((strlen(PREFIXE_) + dir_len) * sizeof (char));
  out_sufixe = (char *) malloc 
    ((strlen(SUFIXE_) + dir_len) * sizeof (char));
  fitxer_data = (char *) malloc 
    ((strlen(FITXER_DATA_) + dir_len) * sizeof (char));
  histograms_file = (char *) malloc 
    ((strlen(HISTOGRAMS_) + dir_len) * sizeof (char));
  historia_file = (char *) malloc 
    ((strlen(HISTORIA_) + dir_len) * sizeof (char));
  historia_defectes_file = (char *) malloc 
    ((strlen(HISTORIA_DEFECTES_) + dir_len) * sizeof (char));
  historia_velos_file = (char *) malloc 
    ((strlen(HISTORIA_VELOS_) + dir_len) * sizeof (char));

  log_file_name = strcat (log_file_name, env_dir);
  log_file_name = strcat (log_file_name, LOG_FILENAME_);
  
  out_prefixe = strcat (out_prefixe, env_dir);
  out_prefixe = strcat (out_prefixe, PREFIXE_);

  out_sufixe = strcat (out_sufixe, SUFIXE_);

  fitxer_data = strcat (fitxer_data, env_dir);
  fitxer_data = strcat (fitxer_data, FITXER_DATA_);

  histograms_file = strcat (histograms_file, env_dir);
  histograms_file = strcat (histograms_file, HISTOGRAMS_);

  historia_file = strcat (historia_file, env_dir);
  historia_file = strcat (historia_file, HISTORIA_);

  historia_defectes_file = strcat (historia_defectes_file, env_dir);
  historia_defectes_file = strcat (historia_defectes_file, HISTORIA_DEFECTES_);

  historia_velos_file = strcat (historia_velos_file, env_dir);
  historia_velos_file = strcat (historia_velos_file, HISTORIA_VELOS_);

  return 0;
}

