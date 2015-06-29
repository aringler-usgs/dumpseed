#include <dcc_std.h>
#include <dcc_misc.h>

_SUB FILE *bombopen(char *infile, char *inmode)
{

  FILE *ofile;

  ofile = fopen(infile,inmode);

  if (ofile==NULL) 
    bombout(EXIT_ABORT,"Cannot open file %s mode %s\n",
	    infile,inmode);

  return(ofile);

}
