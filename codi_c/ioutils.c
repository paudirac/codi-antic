/* Utilitats d'E/S.  */

#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "output_settings.h"
#include "globals.h"
#include "ioutils.h"

int log_init_logfile ();
void log_close_logfile (void);


static FILE *plogfile = 0;

int
report (char *c)
/* Printa una la cadena c entre --*-- a l'stderr, i hi afegeix 
 * la terminació \n.  */
{
  fprintf(stderr,"--*-- %s --*--\n",c);
  return 0;
}

int
log_to_logfile (char *c, char *filename, int lineno)
{
  if (LOG)
    {
      time_t curtime;
      struct tm *loc;
      curtime = time (NULL);
      loc = localtime (&curtime);
      char tempo_[100];
      sprintf (tempo_,"%d/%d/%d %d:%d:%d",
	       loc->tm_mday, loc->tm_mon, loc->tm_year,
	       loc->tm_hour, loc->tm_min, loc->tm_sec);
      if (plogfile == 0)
	log_init_logfile ();
      fprintf (plogfile, "Log:[%s] %s:%d: %s\n",
	       tempo_, filename, lineno, c);
    }
  fflush(plogfile);
  return 0;
}

int
log_init_logfile ()
{
  plogfile = fopen (log_file_name, "a");
  if (plogfile == 0)
    {
      error_at_line 
	(0,errno,__FILE__,__LINE__,
	 "No és possible obrir el fitxer de logs %s.", 
	 log_file_name);
    }
  int i = atexit (&log_close_logfile);
  if (i != 0)
    {
      error_at_line 
	(0,errno,__FILE__,__LINE__,
	 "No és possible registrar log_close_logfile amb atexit.");// log_file_name);
    }
  
  fprintf 
    (plogfile, 
     "--*-- -------------------- Begin -------------------- --*--\n");

  return 0;
}

void
log_close_logfile (void)
{
  if (LOG)
    if (plogfile != 0)
      {
	fprintf 
	  (plogfile, 
	   "--*-- --------------------- end --------------------- --*--\n");
/* 	fflush(plogfile); */
	fclose(plogfile);
      }
}

static FILE *pfile;
static int pfile_locked;

FILE *
ioutils_get_pfile (const char *filename)
{
  if (pfile_locked) /* això no és thread safe  */
    ioutils_close_pfile (pfile);

  pfile = fopen(filename, "w");
  if (pfile == 0)
    error_at_line (0,errno,__FILE__,__LINE__,
		   "No és possible obrir el fitxer %s.", filename);
  pfile_locked = 1;
  return pfile;
}

int
ioutils_close_pfile (FILE *pfile)
{
  if (pfile != 0)
    {
/*       fflush (pfile); */
      fclose (pfile);
      pfile_locked = 0;
    }

  return 0;
}
