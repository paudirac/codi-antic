#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

/** Solució que he trobat a:
 *  http://c-faq.com/malloc/retaggr.920804.html
 *  Com que no poden retornar-se strings allocatats dinàmicament dins
 *  un mètode, perquè són globals, i com que passant el punter no funciona
 *  perquè pot ser que el caller torni a cridar la funció abans que s'hagi
 *  pogut completar, utilitzo un buffer d'strings. En aquest cas el buffer és
 *  de tres strings i funciona, com a mínim fins a 20000 crides molt seguides.
 *  La mida dels strings del bufer poden canviar-se, però per noms normals
 *  hauria de valer de sobra.
 */

#define ZFILL_NRETBUFS 3
#define ZFILL_RETBUFSIZE 100 /** hauria de ser suficient  */

char *
string_utils_zfill (const int n, const int digits)
{
  static char zfill_retbuf[ZFILL_NRETBUFS][ZFILL_RETBUFSIZE];
  static int zfill_whichret = 0;

  char *ret;
  ret = zfill_retbuf[zfill_whichret];
  zfill_whichret = (zfill_whichret + 1) % ZFILL_NRETBUFS;

  char format[10];
  sprintf(format, "%%0.%dd", digits);
  sprintf(ret, format, n);

  return ret;
}

#define FILENAME_NRETBUFS 3
#define FILENAME_RETBUFSIZE 100 /** hauria de ser suficient  */

char *
string_utils_filename (const char *prefixe, const int n,
		       const int digits, const char *sufixe)
{
  static char filename_retbufs[FILENAME_NRETBUFS][FILENAME_RETBUFSIZE];
  static int filename_whichret = 0;

  char *ret;
  ret = filename_retbufs[filename_whichret];
  filename_whichret = (filename_whichret + 1) % FILENAME_NRETBUFS;

  char *zfill_return = string_utils_zfill (n, digits);
  sprintf (ret, "%s%s%s", prefixe, zfill_return, sufixe);

  return ret;
}
