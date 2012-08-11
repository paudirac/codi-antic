#ifndef MACROUTILS_H
#define MACROUTILS_H
#include <stdlib.h>
/* Reemplaça el free i s'assegura de fer un null del pointer.  */
#define FREE_POINTER(p) do { if ((p) != 0) { free((p)); (p) = 0;}} while (0)
#endif /* MACROUTILS_H  */
