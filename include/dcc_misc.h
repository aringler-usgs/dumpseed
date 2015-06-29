/* Protos for misclib */

#include <stationidx.h>

#define ISMAGOPEN(a) ((a)>=0)

void _bomb2(int, char *, ...);
void _bombdesc(char *, char *, int);

#include <seed/itemlist.h>

/* Handle protos for compatibility routines */

#ifndef HAVE_STRERROR
char *strerror(int errnum);
#endif

#include <dcc_misc_proto.h>
