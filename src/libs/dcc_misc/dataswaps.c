/* POSIX-Phase 1 29-May-92/SH */

#include <dcc_std.h>

_SUB LONG TwidLONG(LONG inlong)		/* Twiddle a long word from one to another */
{
  union {
    struct { BYTE orgbytes[4]; } byte_e;
    struct { LONG orglong;	   } long_e;
  } cvtlong;

  BYTE swp;

  cvtlong.long_e.orglong = inlong;

  swp = cvtlong.byte_e.orgbytes[0];
  cvtlong.byte_e.orgbytes[0] = cvtlong.byte_e.orgbytes[3];
  cvtlong.byte_e.orgbytes[3] = swp;
  swp = cvtlong.byte_e.orgbytes[1];
  cvtlong.byte_e.orgbytes[1] = cvtlong.byte_e.orgbytes[2];
  cvtlong.byte_e.orgbytes[2] = swp;

  return(cvtlong.long_e.orglong);
}

_SUB WORD TwidWORD(WORD inword)		/* Twiddle a word around */
{
  union {
    struct { BYTE orgbytes[2]; } byte_e;
    struct { WORD orgword;	   } word_e;
  } cvtword;

  BYTE swp;

  cvtword.word_e.orgword = inword;

  swp = cvtword.byte_e.orgbytes[0];
  cvtword.byte_e.orgbytes[0] = cvtword.byte_e.orgbytes[1];
  cvtword.byte_e.orgbytes[1] = swp;

  return(cvtword.word_e.orgword);
}

_SUB LONG LocGVAX_LONG(LONG inlong)	/* Local gets vax long */
{

#ifdef LITTLE_ENDIAN
  return(inlong);
#else
  return(TwidLONG(inlong));
#endif
}


_SUB LONG LocGM68_LONG(LONG inlong)	/* Local gets M68000 long */
{

#ifdef LITTLE_ENDIAN
  return(TwidLONG(inlong));
#else
  return(inlong);
#endif
}

_SUB WORD LocGVAX_WORD(WORD inword)	/* Local gets vax WORD */
{

#ifdef LITTLE_ENDIAN
  return(inword);
#else
  return(TwidWORD(inword));
#endif
}


_SUB WORD LocGM68_WORD(WORD inword)	/* Local gets M68000 WORD */
{

#ifdef LITTLE_ENDIAN
  return(TwidWORD(inword));
#else
  return(inword);
#endif
}

_SUB LONG VAXGLoc_LONG(LONG inlong)	/* Vax gets local long */
{

#ifdef LITTLE_ENDIAN
  return(inlong);
#else
  return(TwidLONG(inlong));
#endif
}


_SUB LONG M68GLoc_LONG(LONG inlong)	/* 68000 gets local long */
{

#ifdef LITTLE_ENDIAN
  return(TwidLONG(inlong));
#else
  return(inlong);
#endif
}

_SUB WORD VAXGLoc_WORD(WORD inword)	/* vax gets local word */
{

#ifdef LITTLE_ENDIAN
  return(inword);
#else
  return(TwidWORD(inword));
#endif
}

_SUB WORD M68GLoc_WORD(WORD inword)	/* 68000 gets local word */
{

#ifdef LITTLE_ENDIAN
  return(TwidWORD(inword));
#else
  return(inword);
#endif
}

/* Swo 0=LE 1=BE */

_SUB LONG Loc_LONG(LONG inlong,int swo)	/* Local gets swo long */
{

#ifdef LITTLE_ENDIAN
    if (swo==0)
	return(inlong);
    else
	return(TwidLONG(inlong));
#else
    if (swo==0)
	return(TwidLONG(inlong));
    else
	return(inlong);
#endif
}

_SUB WORD Loc_WORD(WORD inword,int swo)	/* Local gets swo word */
{

#ifdef LITTLE_ENDIAN
    if (swo==0)
	return(inword);
    else
	return(TwidWORD(inword));
#else
    if (swo==0)
	return(TwidWORD(inword));
    else
	return(inword);
#endif
}


