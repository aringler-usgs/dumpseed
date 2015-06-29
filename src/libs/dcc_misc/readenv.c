#include <dcc_std.h>

#ifdef VMS

#include iodef
#include ssdef
#include psldef
#include descrip
#include lnmdef
#include stdio

char retbuf[128];

char *readenv(environ)
char *environ;
{

	struct	_itmlst_rz_r {
		WORD	buflen;
		WORD	code;
		ULONG	bufadr;
		ULONG	retlenadr;
	} itmlst_rz_r[3];

	$DESCRIPTOR(tabnam_rt_ds, "LNM$JOB");
	$DESCRIPTOR(lognam_rt_ds, "TEMP");

	ULONG	attribsto = 0;
	LONG	reterr,retlen;

	lognam_rt_ds.dsc$w_length = strlen(environ);
	lognam_rt_ds.dsc$a_pointer= environ; 

	itmlst_rz_r[0].code = LNM$_ATTRIBUTES;
	itmlst_rz_r[0].buflen = sizeof (ULONG);
	itmlst_rz_r[0].bufadr = &attribsto;
	itmlst_rz_r[0].retlenadr = 0;

	itmlst_rz_r[1].code = LNM$_STRING;
	itmlst_rz_r[1].buflen = 126;
	itmlst_rz_r[1].bufadr = &retbuf[0];
	itmlst_rz_r[1].retlenadr = &retlen;

	itmlst_rz_r[2].code = 0;
	itmlst_rz_r[2].buflen = 0;
	itmlst_rz_r[2].bufadr = &retbuf[0];
	itmlst_rz_r[2].retlenadr = 0;

	reterr = SYS$TRNLNM(0,&tabnam_rt_ds,&lognam_rt_ds,0,&itmlst_rz_r[1]);

	if (retlen<0) retlen=0;
	if (retlen>126) retlen=126;
	retbuf[retlen]='\0';

	if (reterr == EXIT_NORMAL) return(retbuf);

	return(NULL);

}

#else

_SUB char *readenv(char *environ)
{

  return(getenv(environ));

}

#endif
