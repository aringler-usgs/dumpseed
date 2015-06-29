#ifndef TMLB_DEFINED
#define TMLB_DEFINED

struct	__sjtim	{		/* Day of year pseudo-julian date form */
	WORD	year;
	WORD	day;
	BYTE	hour;
	BYTE	minute;
	BYTE	second;
	WORD	msec;
};

typedef struct __sjtim STDTIME;

struct	__deltatime	{	/* Difference of a time amount */
	LONG	nday;
	UBYTE	nhour;
	UBYTE	nmin;
	UBYTE	nsec;
	UWORD	nmsecs;
};

typedef struct __deltatime DELTA_T;

typedef long JULIAN;

#define zerotime(x) (x.year == 0)

#define tpr(x) tfpr(stdout,x)
LONG	timsb();
BOOL	tleap();
STDTIME	*timall();

typedef double FLTTIME;    	/* Number of seconds since julian instant */

#define TM_MILEN 1000
#define TM_CENT 100
#define TM_DECADE 10

#define TIM_LIM (20L*86400L*1000L)	/* Time limit (fits in 31 bits) */
					/* 20 Days */

#define ST_SPAN_NIL	0	/* No intersection */
#define ST_SPAN_ACB	1	/* A contains B */
#define ST_SPAN_BCA	2	/* B contains A */
#define ST_SPAN_ALB	3	/* A less than B */
#define ST_SPAN_BLA	4	/* B less than A */

#include <dcc_time_proto.h>

#endif

