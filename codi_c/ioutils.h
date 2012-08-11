/* Utilitats d'E/S.  */
#ifndef IOUTILS_H
#define IOUTILS_H

/* Printa una la cadena c entre --*-- a l'stderr, i hi afegeix
 * la terminació \n.  */
int report (char *c);

/* Printa al fitxer apuntat per plogfile.  */
int
log_to_logfile (char *c, char* filename, int lineno);

FILE *ioutils_get_pfile (const char *filename);
int ioutils_close_pfile (FILE *pfile);

#endif /* IOUTILS_H  */
