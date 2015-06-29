#include <dcc_std.h>
#include <dcc_time.h>

_SUB	FLTTIME	ST_TimeToFLT(STDTIME intime)
{

	LONG jul1,secpd;
	FLTTIME tmpacum;

	jul1 = _julday(intime.year,1,intime.day);

	tmpacum = jul1;
        /* #define SECONDS_PER_DAY 24 * 60 * 60  = 86400 */
	tmpacum *= 86400.0;

	secpd = intime.hour * 3600;
	secpd += intime.minute * 60;
	secpd += intime.second;
	tmpacum += ((FLTTIME) secpd);
	tmpacum += ((FLTTIME) intime.msec) / 1000.0;

	return(tmpacum);

}
