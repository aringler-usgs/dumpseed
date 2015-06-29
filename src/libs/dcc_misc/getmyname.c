#include <dcc_std.h>

#ifdef VAXC

#include jpidef

static struct	_name_c {
	WORD	buflen;
	WORD	code;
	ULONG	bufadr;
	ULONG	retlenadr;
} name_c[3];

static char nbuf[256];
static int rlen;

char *getmyimage(int type)
{
	LONG reterr;
	char tbuf[160];
	char *a,*b;
	int i;

	name_c[0].code = JPI$_IMAGNAME;
	name_c[0].buflen = 150;
	name_c[0].bufadr = tbuf;
	name_c[0].retlenadr = &rlen;

	name_c[1].code = 0;
	name_c[1].buflen = 0;
	name_c[1].bufadr = 0;
	name_c[1].retlenadr = 0;

	reterr = SYS$GETJPIW(0,0,0,&name_c[0],0,0,0);

	if (reterr!=EXIT_NORMAL) return("Unknown");
	tbuf[rlen] = '\0';

	if (type==0) {
		strcpy(nbuf,tbuf);
		return(nbuf);
	}

	b = NULL;
	for (a=tbuf; *a!='\0'; a++) if (*a==']') b=a;
	if (b==NULL) b = tbuf;
	else b++;
	for (a = nbuf; *b!='\0'&&*b!='.'; a++,b++) *a= *b;
	*a='\0';
	if (nbuf[0]=='\0') strcpy(nbuf,"Unknown");

	return(nbuf);

}

char *getmyname()
{

	return(getmyimage(1));
}

#else

#include <pwd.h>

static char _tbuf[40];

_SUB char *getmyname()
{

	uid_t	myuid;
	struct	passwd *inpass;

	myuid = getuid();
	inpass = getpwuid(myuid);
	if (inpass==NULL) return(NULL);

	strcpy(_tbuf, inpass->pw_name);
	return(_tbuf);

}

#endif

