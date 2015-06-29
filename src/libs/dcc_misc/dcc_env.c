#include <dcc_std.h>
#include <dcc_misc.h>

_SUB char *dcc_env(char *inenv)
{

  char *getval;

  getval = getenv(inenv);

  if (getval==NULL)
    bombout(EXIT_ABORT,"Required DCC Environment Variable %s not set!",
	    inenv);

  return(getval);
}
