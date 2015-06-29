#include <dcc_std.h>

#ifndef HAVE_STRERROR

/* Compatibility routines */

extern char *sys_errlist[];
extern int sys_nerr;

static char rets[100];

char *strerror(int errnum)
{

  if (errnum<0 || errnum>sys_nerr) {
    sprintf(rets,"Error out of range (value=%d)",errnum);
    return(rets);
  }

  return(sys_errlist[errnum]);

}

#endif

    
