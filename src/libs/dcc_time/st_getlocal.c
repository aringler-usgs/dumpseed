#include <dcc_std.h>
#include <dcc_time.h>

UBYTE _dmlsize[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#if VMS

_SUB	STDTIME ST_GetLocalTime()
{

	ULONG sysret;
	UWORD numret[7];
	STDTIME rettime;
	WORD i,ct,mon,day;

	sysret = SYS$NUMTIM(numret,0);	/* Get current time */

	if (sysret!=EXIT_NORMAL) {
		printf("ST_GetCurrentTime SYS$NUMTIM failed\n");
	}

	rettime.year = numret[0];
	mon = numret[1];
	day = numret[2];
	rettime.hour = numret[3];
	rettime.minute = numret[4];
	rettime.second = numret[5];
	rettime.msec = numret[6] * 10;

	_dmlsize[1] = _tleap(rettime.year)?29:28;

	ct=0;
	for (i=0; i<(mon-1); i++) ct+=_dmlsize[i];
	ct += day;

	rettime.day = ct;

	return(rettime);

}

#else

STDTIME ST_GetLocalTime(void)
{

	struct tm *intime;
	time_t tloc;

	STDTIME rettime;
	WORD i,ct,mon,day;

	time(&tloc);
	intime = localtime(&tloc);

	rettime.year = intime->tm_year + 1900;
	mon = intime->tm_mon + 1;
	day = intime->tm_mday;
	rettime.hour = intime->tm_hour;
	rettime.minute = intime->tm_min;
	rettime.second = intime->tm_sec;
	rettime.msec = 0;

	_dmlsize[1] = _tleap(rettime.year)?29:28;

	ct=0;
	for (i=0; i<(mon-1); i++) ct+=_dmlsize[i];
	ct += day;

	rettime.day = ct;

	return(rettime);

}

#endif


