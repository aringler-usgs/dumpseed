/* POSIX - Phase 1 - 29-May-92/SH */

#include <dcc_std.h>

_SUB FILE *open_ctx_file()
{

  char cfile[200];

#ifdef VMS
  sprintf(cfile,"sys$login:ctx_%x.tmp",getpid());
#else
  sprintf(cfile,"/tmp/ctx_%x",(int) getppid()); /* Get parent id */
#endif

  return(fopen(cfile,"w"));

}
