/* Oct. 28, 2002 GTM - modified to support data quality indicator flags D, Q and R */
#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>
#include <seed_data_proto.h>
#include <dcc_misc.h>
#include <dcc_seed_select.h>

#include "dumpctx.h"

#include "dumpseed_proto.h"

#define LW(a) SH_SWAP_WORD(a)
#define LL(a) SH_SWAP_LONG(a)

extern int ultrashear;
extern int caldelay;
extern int data_swap_order;

struct blkhdr {
  UWORD	blktype;
  UWORD	blkxref;
};

VOID BDF_Blk_DO(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct  Data_only *dat;

  dat = (struct Data_only *) prec;

  ultrashear = 1;

  data_swap_order = dat->Order;

  /*format = dat->Encoding;*/

}

VOID BDF_Blk_QE(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct  Data_ext *dat;

  dat = (struct Data_ext *) prec;

  fprintf(outfile,"QTAD%d, %d, %d\n",
	  dat->Timing,
	  dat->Usec,
	  dat->Frames);


}

VOID BDF_Blk_GE(UBYTE *prec, int type, FILE *outfile) 
{ 
  fprintf(outfile,"Blockette %d seen\n",type); 

}

VOID BDF_Blk_ME(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct murdock_detect *mur;
  int i;

  mur = (struct murdock_detect *) prec;

  fprintf(outfile,"PICK%s, %d, ",
	  mur->Event_Flags&MEVENT_DILAT?"d":"c",
	  mur->Lookback);
		
  for (i=0; i<5; i++) putchar('0'+mur->SNR_Qual[i]);

  fprintf(outfile,", %s",ST_PrintDate(SH_Cvt_Time(mur->Onset_Time),TRUE));
  fprintf(outfile,", %g",SH_Cvt_Float(mur->Signal_Amplitude));
  fprintf(outfile,", %g",SH_Cvt_Float(mur->Signal_Period));
  fprintf(outfile,", %g",SH_Cvt_Float(mur->Background_Estimate));

  fprintf(outfile,", %c",'A'+mur->Pick);
  if (ultrashear) {
    fprintf(outfile,", %s",pullasc(mur->Detector_Name,24));
  }
     
  fprintf(outfile,"\n");

}


VOID BDF_Blk_STC(UBYTE *prec, int type, FILE *outfile) 
{

}

VOID BDF_Blk_SIC(UBYTE *prec, int type, FILE *outfile) 
{

}


VOID BDF_Blk_PRC(UBYTE *prec, int type, FILE *outfile) 
{
}


VOID BDF_Blk_GC(UBYTE *prec, int type, FILE *outfile) 
{

}


VOID BDF_Blk_CA(UBYTE *prec, int type, FILE *outfile) 
{
}


VOID BDF_Blk_BB(UBYTE *prec, int type, FILE *outfile) 
{

}


VOID BDF_Blk_BD(UBYTE *prec, int type, FILE *outfile) 
{

}

_SUB void prtseedbdf(SEED_DATA *srec, int dumpblockettes, FILE *outfile)
{

  char buff[100];
  char chan[20];
  int a,b,c;
  STDTIME btim;
  STDFLT d;
  char *membf;
  struct blkhdr *newblk;

  membf = (char *) srec;

  if ( srec->Record_Type != 'D' &&  srec->Record_Type != 'Q' &&  srec->Record_Type != 'R') {
    return;
  }

  memcpy(buff,srec->Station_ID,5);
  buff[5] = '\0';
  TrimString(buff);

  fprintf(outfile,"STA %s\n",buff);

  memcpy(buff,srec->Network_ID,2);
  buff[2] = '\0';
  TrimString(buff);

  if (buff[0]!='\0')  fprintf(outfile,"NET %s\n",buff);

  memcpy(buff,srec->Location_ID,2);
  buff[2] = '\0';
  TrimString(buff);

  if (buff[0]!='\0') fprintf(outfile,"LOC %s\n",buff);

  memcpy(buff,srec->Channel_ID,3);
  buff[3] = '\0';
  TrimString(buff);

  fprintf(outfile,"COMP%s\n",buff);
  strcpy(chan,buff);

  a = LW(srec->Rate_Factor);
  b = LW(srec->Rate_Mult);

  if (b==0) fprintf(outfile,"RATE0.0\n");
  else {
    if (a<0) d = 1.0/(-((STDFLT) a));
    else d = (STDFLT) a;
    if (b<0) d /= -((STDFLT) b);
    else d *= (STDFLT) b;

    fprintf(outfile,"RATE%.4g\n",d);

  }

  btim.year = LW(srec->Start_Time.year);
  btim.day = LW(srec->Start_Time.day);
  btim.hour = srec->Start_Time.hour;
  btim.minute = srec->Start_Time.minute;
  btim.second = srec->Start_Time.seconds;
  btim.msec = LW(srec->Start_Time.fracs);

  fprintf(outfile,"TIME%04d,%03d,%02d:%02d:%02d.%04d\n",
	  (int) btim.year,
	  (int) btim.day,
	  (int) btim.hour,
	  (int) btim.minute,
	  (int) btim.second,
	  (int) btim.msec);

  if (caldelay) {
      STDFLT curtim;
      STDFLT begflt,endflt;

      curtim = (STDFLT) time(NULL);  /* Note - second level accuracy only */
      begflt = (STDFLT) ST_GetDblUnix(SH_Start_Time(srec));
      endflt = (STDFLT) ST_GetDblUnix(SH_End_Time(srec));
      begflt = curtim - begflt;
      endflt = curtim - endflt;

      fprintf(outfile,"DLAY%.2f,%.2f\n",begflt,endflt);
  }

  fprintf(outfile,"NSAM%d\n",LW(srec->Number_Samps));

  a = LW(srec->First_Blockette);
  
  if (dumpblockettes) {
    FOREVER {
      if (a<=0 || a>4095) break;
      c = a;
      newblk = (struct blkhdr *) &membf[a];
      a = LW(newblk->blkxref);
      b = LW(newblk->blktype);
      switch (b) {
      case 200:	BDF_Blk_GE(&membf[c],b,outfile);	break;
      case 201:	BDF_Blk_ME(&membf[c],b,outfile);	break;
      case 300:	BDF_Blk_STC(&membf[c],b,outfile);	break;
      case 310:	BDF_Blk_SIC(&membf[c],b,outfile);	break;
      case 320:	BDF_Blk_PRC(&membf[c],b,outfile);	break;
      case 390:	BDF_Blk_GC(&membf[c],b,outfile);	break;
      case 395:	BDF_Blk_CA(&membf[c],b,outfile);	break;
      case 400:	BDF_Blk_BB(&membf[c],b,outfile);	break;
      case 405:	BDF_Blk_BD(&membf[c],b,outfile);	break;
      case 1000:	BDF_Blk_DO(&membf[c],b,outfile);	break;
      case 1001:	BDF_Blk_QE(&membf[c],b,outfile);	break;
      default:
	break;
      }
    }
  }
}

