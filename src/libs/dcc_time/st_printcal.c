#include <dcc_std.h>
#include <dcc_time.h>

static char _ddata[60];

char *FormNames[12] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December" };

char *ShortNames[12] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec" };

_SUB	char *ST_PrintCalDate(STDTIME odate,BOOL longfmt)
{

	LONG jdate;
	WORD mon,day,jday,jyr;

	jdate = ST_GetJulian(odate);

	ST_CnvJulToCal(jdate,&jyr,&mon,&day,&jday);

	if (longfmt)
		sprintf(_ddata,"%s %d, %d",FormNames[mon-1],day,jyr);
	else	sprintf(_ddata,"%2d-%s-%04d",day,ShortNames[mon-1],jyr);

	return(_ddata);

}

_SUB	char *ST_PrintFullDate(STDTIME odate)
{

	LONG jdate;
	char tdate[40];
	WORD mon,day,jday,jyr;
	int hrf=0,mnf=0,sef=0,msf=0;

	jdate = ST_GetJulian(odate);

	ST_CnvJulToCal(jdate,&jyr,&mon,&day,&jday);

	_ddata[0] = '\0';

	sprintf(tdate,"%d-%s-%04d",day,ShortNames[mon-1],jyr);	
	strcat(_ddata,tdate);
	sprintf(tdate," (%d)",jday);
	strcat(_ddata,tdate);

	if (odate.hour) hrf=mnf=1;
	if (odate.minute) hrf=mnf=1;
	if (odate.second) hrf=mnf=sef=1;
	if (odate.msec) hrf=mnf=sef=msf=1;

	if (hrf) {
	  sprintf(tdate," %d",odate.hour);
	  strcat(_ddata,tdate);
	}
	if (mnf) {
	  sprintf(tdate,":%02d",odate.minute);
	  strcat(_ddata,tdate);
	}
	if (mnf) {
	  sprintf(tdate,":%02d",odate.second);
	  strcat(_ddata,tdate);
	}
	if (msf) {
	  if (odate.msec%10) 
	    sprintf(tdate,".%03d",odate.msec);
	  else 
	    if (odate.msec%100) 
	      sprintf(tdate,".%02d",odate.msec/10);
	  else
	    sprintf(tdate,".%01d",odate.msec/100);
	  strcat(_ddata,tdate);
	}

	return(_ddata);

}

