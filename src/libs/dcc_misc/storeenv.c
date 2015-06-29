#include <dcc_std.h>

#ifdef VMS

#include iodef
#include ssdef
#include psldef
#include descrip
#include lnmdef
#include stdio

char *storeenv(environ,content)
char *environ,*content;
{

	struct	_itmlst_rz_r {
		WORD	buflen;
		WORD	code;
		ULONG	bufadr;
		ULONG	retlenadr;
	} itmlst_rz_r[3];

	$DESCRIPTOR(tabnam_rt_ds, "LNM$JOB");
	$DESCRIPTOR(lognam_rt_ds, "TEMP");

	char	acmode_rbu_r = PSL$C_USER;
	ULONG	attribsto = LNM$M_TERMINAL /*| LNM$M_CASE_BLIND*/;
	LONG	reterr;

	char	tbuff[LNM$C_NAMLENGTH+1];

	strncpy(&tbuff[0],content,LNM$C_NAMLENGTH);
	tbuff[LNM$C_NAMLENGTH]='\0';

	lognam_rt_ds.dsc$w_length = strlen(environ);
	lognam_rt_ds.dsc$a_pointer= environ; 

	itmlst_rz_r[0].code = LNM$_ATTRIBUTES;
	itmlst_rz_r[0].buflen = sizeof (ULONG);
	itmlst_rz_r[0].bufadr = &attribsto;
	itmlst_rz_r[0].retlenadr = 0;

	itmlst_rz_r[1].code = LNM$_STRING;
	itmlst_rz_r[1].buflen = strlen(&tbuff[0]);
	itmlst_rz_r[1].bufadr = &tbuff[0];
	itmlst_rz_r[1].retlenadr = 0;

	itmlst_rz_r[2].code = 0;
	itmlst_rz_r[2].buflen = 0;
	itmlst_rz_r[2].retlenadr = 0;

	reterr = SYS$CRELNM(0,&tabnam_rt_ds,&lognam_rt_ds,0,&itmlst_rz_r[0]);

	if (reterr == SS$_NORMAL) return(content);
	if (reterr == SS$_SUPERSEDE) return(content);

	return(NULL);

}

char *storeenvproc(environ,content)
char *environ,*content;
{

	struct	_itmlst_rz_r {
		WORD	buflen;
		WORD	code;
		ULONG	bufadr;
		ULONG	retlenadr;
	} itmlst_rz_r[3];

	$DESCRIPTOR(tabnam_rt_ds, "LNM$PROCESS_TABLE");
	$DESCRIPTOR(lognam_rt_ds, "TEMP");

	char	acmode_rbu_r = PSL$C_USER;
	ULONG	attribsto = LNM$M_TERMINAL /*| LNM$M_CASE_BLIND*/;
	LONG	reterr;

	char	tbuff[LNM$C_NAMLENGTH+1];

	strncpy(&tbuff[0],content,LNM$C_NAMLENGTH);
	tbuff[LNM$C_NAMLENGTH]='\0';

	lognam_rt_ds.dsc$w_length = strlen(environ);
	lognam_rt_ds.dsc$a_pointer= environ; 

	itmlst_rz_r[0].code = LNM$_ATTRIBUTES;
	itmlst_rz_r[0].buflen = sizeof (ULONG);
	itmlst_rz_r[0].bufadr = &attribsto;
	itmlst_rz_r[0].retlenadr = 0;

	itmlst_rz_r[1].code = LNM$_STRING;
	itmlst_rz_r[1].buflen = strlen(&tbuff[0]);
	itmlst_rz_r[1].bufadr = &tbuff[0];
	itmlst_rz_r[1].retlenadr = 0;

	itmlst_rz_r[2].code = 0;
	itmlst_rz_r[2].buflen = 0;
	itmlst_rz_r[2].retlenadr = 0;

	reterr = SYS$CRELNM(0,&tabnam_rt_ds,&lognam_rt_ds,0,&itmlst_rz_r[0]);

	if (reterr == SS$_NORMAL) return(content);
	if (reterr == SS$_SUPERSEDE) return(content);

	return(NULL);

}

#else

_SUB char *storeenv(char *environ,char *content)
{
  char tbuf[1024];

  strcpy(tbuf,environ);
  strcat(tbuf,"=");
  strcpy(tbuf,content);

  if (putenv(tbuf)!=NULL) return(NULL);

  return(content);
}
#endif
