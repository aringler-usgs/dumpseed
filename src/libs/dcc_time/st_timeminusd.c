#include <dcc_std.h>
#include <dcc_time.h>

#define _tdays(x) (_tleap(x)?366:365)

_SUB	STDTIME	ST_TimeMinusDelta(STDTIME intime,DELTA_T indelta)
{

	STDTIME outtime;
	LONG dy,dd,dh,dm,ds,dms;

	dy = intime.year;
	dd = intime.day - 1 - indelta.nday;
	dh = intime.hour - indelta.nhour;
	dm = intime.minute - indelta.nmin;
	ds = intime.second - indelta.nsec;
	dms = intime.msec - indelta.nmsecs;	

/*-------------Normalize the date-------------*/

	timenorm(&dy,&dd,&dh,&dm,&ds,&dms);

	outtime.year = dy;
	outtime.day = dd + 1;
	outtime.hour = dh;
	outtime.minute = dm;
	outtime.second = ds;
	outtime.msec = dms;

	return(outtime);		/* Returning a structure! */

}
