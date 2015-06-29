#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_seed.h>
#include <seed_data.h>
#include <dcc_misc.h>

int _local_endian = 1;
extern int data_swap_order;

_SUB void SH_Big_Endian()
{

  _local_endian = 1;
  
}

_SUB void SH_Little_Endian()
{

  _local_endian = 0;
  
}

_SUB WORD SH_SWAP_WORD(WORD i)
{

    return(Loc_WORD(i,_local_endian));

}

_SUB LONG SH_SWAP_LONG(LONG i)
{

    return(Loc_LONG(i,_local_endian));

}

_SUB WORD SH_SWAP_DATA_WORD(WORD i)
{

    return(Loc_WORD(i,data_swap_order));

}

_SUB LONG SH_SWAP_DATA_LONG(LONG i)
{

    return(Loc_LONG(i,data_swap_order));

}

_SUB STDFLT SH_Cvt_Float(NETFLT inflt)
{

  union {
    struct { NETFLT nflt; } net_e;
    struct { float fflt;  } flt_e;
  } cvtword;

  cvtword.net_e.nflt = inflt;
  return(cvtword.flt_e.fflt);

}

_SUB STDTIME SH_Cvt_Time(NETTIME intim)
{

  STDTIME blktim;

  blktim.year = SH_SWAP_WORD(intim.year);
  blktim.day = SH_SWAP_WORD(intim.day);
  blktim.hour = intim.hour;
  blktim.minute = intim.minute;
  blktim.second = intim.seconds;
  blktim.msec = SH_SWAP_WORD(intim.fracs) / 10;

  return(blktim);
}

_SUB STDTIME SH_Start_Time(SEED_DATA *pblk)
{

  STDTIME blktim;

  blktim.year = SH_SWAP_WORD(pblk->Start_Time.year);
  blktim.day = SH_SWAP_WORD(pblk->Start_Time.day);
  blktim.hour = pblk->Start_Time.hour;
  blktim.minute = pblk->Start_Time.minute;
  blktim.second = pblk->Start_Time.seconds;
  blktim.msec = SH_SWAP_WORD(pblk->Start_Time.fracs) / 10;

  return(blktim);
}

_SUB int SH_Number_Samples(SEED_DATA *pblk)
{
  return(SH_SWAP_WORD(pblk->Number_Samps));
}

_SUB int SH_Start_Data(SEED_DATA *pblk)
{

  int std = SH_SWAP_WORD(pblk->Data_Start);
  if (std<48 || std>4096) return(0);  /* ??? */

  return(std);
}

_SUB STDFLT SH_Record_Rate(SEED_DATA *pblk)
{

  LONG msdif;
  STDFLT samrat;
  WORD factor,rate;

  factor = SH_SWAP_WORD(pblk->Rate_Factor); 
  rate = SH_SWAP_WORD(pblk->Rate_Mult);

  if (rate==0) {		/* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = M68GLoc_WORD(rate);
  }			

  msdif = 0;
  samrat = 0.0;

  if (rate!=0&&factor!=0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else 	samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else	samrat /= ((STDFLT) (-msdif));
  }

  return(samrat);
}

_SUB LONG SH_Record_Time(SEED_DATA *pblk)
{

  LONG numsam,msdif;
  STDFLT samrat,samtim;
  WORD factor,rate;

  numsam = SH_SWAP_WORD(pblk->Number_Samps);

  factor = SH_SWAP_WORD(pblk->Rate_Factor); 
  rate = SH_SWAP_WORD(pblk->Rate_Mult);

  if (rate==0) {		/* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = M68GLoc_WORD(rate);
  }			

  msdif = 0;
  samrat = 0.0;
  samtim = 0.0;

  if (rate!=0&&factor!=0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else 	samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else	samrat /= ((STDFLT) (-msdif));

    samrat = 1.0/samrat;
    samrat*=1000.0;		/* Ms/Sam */
    samtim = samrat * ((STDFLT) numsam);
  }

  msdif = samtim;

  return(msdif);

}

_SUB LONG SH_Sample_Time(SEED_DATA *pblk)
{

  LONG msdif;
  STDFLT samrat,samtim;
  WORD factor,rate;

  factor = SH_SWAP_WORD(pblk->Rate_Factor); 
  rate = SH_SWAP_WORD(pblk->Rate_Mult);

  if (rate==0) {		/* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = M68GLoc_WORD(rate);
  }			

  msdif = 0;
  samrat = 0.0;
  samtim = 0.0;

  if (rate!=0&&factor!=0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else 	samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else	samrat /= ((STDFLT) (-msdif));

    samrat = 1.0/samrat;
    samrat*=1000.0;		/* Ms/Sam */
    samtim = samrat;
  }

  msdif = samtim;

  return(msdif);

}

_SUB STDFLT SH_Sample_FltTime(SEED_DATA *pblk)
{

  LONG msdif;
  STDFLT samrat,samtim;
  WORD factor,rate;

  factor = SH_SWAP_WORD(pblk->Rate_Factor); 
  rate = SH_SWAP_WORD(pblk->Rate_Mult);

  if (rate==0) {		/* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = M68GLoc_WORD(rate);
  }			

  msdif = 0;
  samrat = 0.0;
  samtim = 0.0;

  if (rate!=0&&factor!=0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else 	samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else	samrat /= ((STDFLT) (-msdif));

    samrat = 1.0/samrat;
    return(samrat);
  }

  return(0.0);

}

_SUB STDTIME SH_Cal_Samp_Time(SEED_DATA *inrec, int samn)
{

  STDFLT msint;
  long mstot;

  msint = SH_Sample_FltTime(inrec);
  msint *= samn;
  msint *= 1000.0;

  mstot = msint;

  return(ST_AddToTime(SH_Start_Time(inrec),0,0,0,0,mstot));

}

_SUB STDTIME SH_End_Time(SEED_DATA *pblk)
{

  return(ST_AddToTime(SH_Start_Time(pblk),0,0,0,0,SH_Record_Time(pblk)));

}

_SUB LONG SH_Record_Number(SEED_DATA *pblk)
{

  LONG sval;
  UBYTE tbuf[8];

  memcpy(tbuf,pblk->Seq_ID,6);
  tbuf[6] = '\0';

  sval = atol(tbuf);

  return(sval);
}

_SUB VOID SH_Get_Idents(SEED_DATA *pblk,
			char *net, char *stat, 
			char *loc, char *chan)
{

  char tbuf[8];

  memcpy(tbuf,pblk->Network_ID,2);
  tbuf[2] = '\0';
  strcpy(net, tbuf);

  memcpy(tbuf,pblk->Station_ID,5);
  tbuf[5] = '\0';
  strcpy(stat, tbuf);

  memcpy(tbuf,pblk->Location_ID,2);
  tbuf[2] = '\0';
  strcpy(loc, tbuf);

  memcpy(tbuf,pblk->Channel_ID,3);
  tbuf[3] = '\0';
  strcpy(chan, tbuf);

}

_SUB VOID SH_Get_Spankey(SEED_DATA *pblk, char *retspan)
{

  char net[3],stat[6],loc[3],chan[4];

  SH_Get_Idents(pblk,net,stat,loc,chan);

  strcpy(&retspan[0],net);
  strcpy(&retspan[2],stat);
  strcpy(&retspan[7],loc);
  strcpy(&retspan[9],chan);

}

/* This needs to incorporate the new endian bit settings 
 THIS OBVIOUSLY BREAKS OUTSIDE THE 1950-2060 DATE RANGE */

_SUB VOID SH_Guess_Endian(SEED_DATA *pblk)
{

    int year;

    _local_endian = 1;

    year = Loc_WORD(pblk->Start_Time.year,_local_endian);
    if (year>1950 && year<2060) return;

    _local_endian = 0;

    year = Loc_WORD(pblk->Start_Time.year,_local_endian);
    if (year>1950 && year<2060) return;

    /* Can't figure it out, but we're assuming BE */

    _local_endian = 1; 

    return;

}
