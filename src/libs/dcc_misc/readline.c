#include <dcc_std.h>

_SUB BOOL ReadLine(FILE *ifile,char *buffer,int leng)
{
	int a;

	if (feof(ifile)) return(FALSE);

	if (fgets(buffer,leng,ifile)==NULL) return(FALSE);
	a = strlen(buffer);
	if (a>0&&buffer[a-1]=='\n') buffer[a-1]='\0';

	return(TRUE);
}
