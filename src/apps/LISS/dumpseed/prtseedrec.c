/* Oct. 28, 2002 GTM - modified to support data quality indicator flags D, Q and R */
#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>
#include <seed_data_proto.h>
#include <dcc_misc.h>
#include <dcc_seed_select.h>

#include "dumpctx.h"

#include "dumpseed_proto.h"

extern int _local_endian;
int ultrashear=0;

#define LW(a) SH_SWAP_WORD(a)
#define LL(a) SH_SWAP_LONG(a)

extern int data_swap_order;

LOCAL char retv[100];

struct blkhdr {
  UWORD	blktype;
  UWORD	blkxref;
};

_SUB char *pullasc(char *inb, char len)
{

  int e=len-1;
  int c;

  while (e>0&&inb[e]==' ') e--;

  for (c=0; c<=e; c++) {
    if (inb[c]=='\0') break;
    if (isprint(inb[c]))
      retv[c] = inb[c];
    else 
      retv[c] = ' ';
  }

  retv[c] = '\0';
  if (retv[0]==' ') retv[0] = '\0';
  return(retv);
}

VOID Blk_DO(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct  Data_only *dat;

  dat = (struct Data_only *) prec;

  ultrashear = 1;

  fprintf(outfile,"1000-DATAONLY: ");
  switch(dat->Encoding) {
  case 0: fprintf(outfile,"NONE"); break;
  case 1: fprintf(outfile,"16-WORD"); break;
  case 2: fprintf(outfile,"24-WORD"); break;
  case 3: fprintf(outfile,"32-WORD"); break;
  case 4: fprintf(outfile,"IEEE-SP"); break;
  case 5: fprintf(outfile,"IEEE-DP"); break;
  case 10: fprintf(outfile,"STEIM-1"); break;
  case 11: fprintf(outfile,"STEIM-2"); break;
  case 12: fprintf(outfile,"GEOSCOPE-MPX24"); break;
  case 13: fprintf(outfile,"GEOSCOPE-16-3"); break;
  case 14: fprintf(outfile,"GEOSCOPE-16-4"); break;
  case 15: fprintf(outfile,"USNSN"); break;
  case 16: fprintf(outfile,"CDSN"); break;
  case 17: fprintf(outfile,"GRAEF"); break;
  case 18: fprintf(outfile,"IPG"); break;
  case 19: fprintf(outfile,"STEIM-3"); break;
  case 30: fprintf(outfile,"SRO/ASRO"); break;
  case 31: fprintf(outfile,"HGLP"); break;
  case 32: fprintf(outfile,"DWWSSN"); break;
  case 33: fprintf(outfile,"RSTN"); break;
  default: fprintf(outfile,"Encoding %d",dat->Encoding);
  }

  /*format = dat->Encoding;*/

  fprintf(outfile," Swap=%s",dat->Order?"BE":"LE");
  data_swap_order = dat->Order;

  fprintf(outfile," Len=%d\n",dat->Length);
}

VOID Blk_QE(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct  Data_ext *dat;

  dat = (struct Data_ext *) prec;

  fprintf(outfile,"1001-QTADEXT: ");
  fprintf(outfile,"Quality=%d%% ",dat->Timing);
  fprintf(outfile,"Usec=%d ",dat->Usec);
  fprintf(outfile,"Frames=%d\n",dat->Frames);

}

VOID Blk_GE(UBYTE *prec, int type, FILE *outfile) 
{ 
  fprintf(outfile,"Blockette %d seen\n",type); 

}
     
VOID Blk_ME(UBYTE *prec, int type, FILE *outfile) 
{ 
  struct murdock_detect *mur;
  int i;

  mur = (struct murdock_detect *) prec;

  fprintf(outfile,"201-MURDET: %s %d ",
	 mur->Event_Flags&MEVENT_DILAT?"d":"c",
	 mur->Lookback);
		
  for (i=0; i<5; i++) putchar('0'+mur->SNR_Qual[i]);

  fprintf(outfile," %s",ST_PrintDate(SH_Cvt_Time(mur->Onset_Time),TRUE));
  fprintf(outfile," %g",SH_Cvt_Float(mur->Signal_Amplitude));
  fprintf(outfile," %g",SH_Cvt_Float(mur->Signal_Period));
  fprintf(outfile," %g",SH_Cvt_Float(mur->Background_Estimate));

  fprintf(outfile," %c",'A'+mur->Pick);

  if (ultrashear) {
    fprintf(outfile," %s",pullasc(mur->Detector_Name,24));
  }

  fprintf(outfile,"\n");

}

VOID Blk_STC(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}

VOID Blk_SIC(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}


VOID Blk_PRC(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}


VOID Blk_GC(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}


VOID Blk_CA(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}


VOID Blk_BB(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}


VOID Blk_BD(UBYTE *prec, int type, FILE *outfile) 
{
  fprintf(outfile,"Blockette %d seen\n",type); 
}

_SUB void prtseedrec(SEED_DATA *srec,int dumpblockettes, FILE *outfile, 
		     long ms,long recnum)
{

  char buff[100];
  char chan[20];
  int a,b,c;
  STDTIME btim;
  STDFLT d;
  int anyact=0;
  char *membf;
  struct blkhdr *newblk;

  membf = (char *) srec;

  memcpy(buff, (void *) srec->Seq_ID,6);
  buff[6] = '\0';

  fprintf(outfile,"Record %s",buff);
  if (recnum>=0) 
    fprintf(outfile," (%ld)",recnum);
  fprintf(outfile," Type %c ",srec->Record_Type);
  if ( srec->Record_Type != 'D' && srec->Record_Type != 'Q' && srec->Record_Type != 'R' ) {
    fprintf(outfile,"\n");
    return;
  }

  memcpy(buff,srec->Network_ID,2);
  buff[2] = '\0';
  TrimString(buff);

  if (buff[0]!='\0')  fprintf(outfile,"Network %s ",buff);

  memcpy(buff,srec->Station_ID,5);
  buff[5] = '\0';
  TrimString(buff);

  fprintf(outfile,"Station %s ",buff);

  memcpy(buff,srec->Location_ID,2);
  buff[2] = '\0';
  TrimString(buff);

  if (buff[0]!='\0') fprintf(outfile,"Location %s ",buff);

  memcpy(buff,srec->Channel_ID,3);
  buff[3] = '\0';
  TrimString(buff);

  fprintf(outfile,"Channel %s\n",buff);
  strcpy(chan,buff);

  btim.year = LW(srec->Start_Time.year);
  btim.day = LW(srec->Start_Time.day);
  btim.hour = srec->Start_Time.hour;
  btim.minute = srec->Start_Time.minute;
  btim.second = srec->Start_Time.seconds;
  btim.msec = LW(srec->Start_Time.fracs);

  fprintf(outfile,"Time %04d,%03d,%02d:%02d:%02d.%04d ",
	  (int) btim.year,
	  (int) btim.day,
	  (int) btim.hour,
	  (int) btim.minute,
	  (int) btim.second,
	  (int) btim.msec);

  a = LW(srec->Rate_Factor);
  b = LW(srec->Rate_Mult);

  /* Ignore gap if beginning of event */
  if (srec->Activity_Flags&ACTFLAG_BEGEVT && ms >0) ms = 0;

  if (a && b && ms) 
    fprintf(stdout,"(%ldms %s) ",
	    ms,ms>0?"gap":"rev");

  fprintf(outfile,"Samples %d ",LW(srec->Number_Samps));

  fprintf(outfile,"Factor %d Mult %d",a,b);

  if (b==0) {
    if (a!=0)
      fprintf(outfile," (Rate Illegal!)");
  }
  else {
    if (a<0) d = 1.0/(-((STDFLT) a));
    else d = (STDFLT) a;
    if (b<0) d /= -((STDFLT) b);
    else d *= (STDFLT) b;

    fprintf(outfile," (%gsps)",d);

  }

  fprintf(outfile,"\n");

  anyact = 0;

  if (srec->Activity_Flags) {

    fprintf(outfile,"Activity: ");

    if (srec->Activity_Flags&ACTFLAG_CALSIG) fprintf(outfile,"CALSIG ");
    if (srec->Activity_Flags&ACTFLAG_CLKFIX) fprintf(outfile,"CLKFIX ");
    if (srec->Activity_Flags&ACTFLAG_BEGEVT) fprintf(outfile,"BEGEVT ");
    if (srec->Activity_Flags&ACTFLAG_ENDEVT) fprintf(outfile,"ENDEVT ");
    if (srec->Activity_Flags&ACTFLAG_EVTPRG) fprintf(outfile,"EVTPRG ");

    fprintf(outfile,"(0x%02x) ",srec->Activity_Flags);
    anyact = 1;
  }

  if (srec->IO_Flags) {

    fprintf(outfile,"IO: ");

    if (srec->IO_Flags&IOFLAG_ORGPAR) fprintf(outfile,"ORGPAR ");
    if (srec->IO_Flags&IOFLAG_LONGRC) fprintf(outfile,"LONGRC ");
    if (srec->IO_Flags&IOFLAG_SHORTR) fprintf(outfile,"SHORTR ");
    if (srec->IO_Flags&IOFLAG_GOODTM) fprintf(outfile,"GOODTM ");

    fprintf(outfile,"(0x%02x) ",srec->IO_Flags);
    anyact = 1;
  }

  if (srec->Qual_Flags) {

    fprintf(outfile,"Qual: ");

    if (srec->Qual_Flags&QULFLAG_AMPSAT) fprintf(outfile,"AMPSAT ");
    if (srec->Qual_Flags&QULFLAG_SIGCLP) fprintf(outfile,"SIGCLP ");
    if (srec->Qual_Flags&QULFLAG_SPIKES) fprintf(outfile,"SPIKES ");
    if (srec->Qual_Flags&QULFLAG_GLITCH) fprintf(outfile,"GLITCH ");
    if (srec->Qual_Flags&QULFLAG_PADDED) fprintf(outfile,"PADDED ");
    if (srec->Qual_Flags&QULFLAG_TLMSNC) fprintf(outfile,"TLMSNC ");
    if (srec->Qual_Flags&QULFLAG_CHARGE) fprintf(outfile,"CHARGE ");
    if (srec->Qual_Flags&QULFLAG_TIMERR) fprintf(outfile,"TIMERR ");

    fprintf(outfile,"(0x%02x) ",srec->Qual_Flags);
    anyact = 1;
  }

  if (anyact) 
    fprintf(outfile,"\n");

  a = LW(srec->First_Blockette);
  
  fprintf(outfile,"Blockettes %d Correction %d Data Start %d First Block %d Hdr Swap %s\n",
	 srec->Total_Blockettes,
	 LL(srec->Time_Correction),
	 LW(srec->Data_Start),a,
	  _local_endian?"BE":"LE");
  
  if (dumpblockettes) {
    FOREVER {
      if (a<=0 || a>4095) break;
      c = a;
      newblk = (struct blkhdr *) &membf[a];
      a = LW(newblk->blkxref);
      b = LW(newblk->blktype);
      switch (b) {
      case 200:	Blk_GE(&membf[c],b,outfile);	break;
      case 201:	Blk_ME(&membf[c],b,outfile);	break;
      case 300:	Blk_STC(&membf[c],b,outfile);	break;
      case 310:	Blk_SIC(&membf[c],b,outfile);	break;
      case 320:	Blk_PRC(&membf[c],b,outfile);	break;
      case 390:	Blk_GC(&membf[c],b,outfile);	break;
      case 395:	Blk_CA(&membf[c],b,outfile);	break;
      case 400:	Blk_BB(&membf[c],b,outfile);	break;
      case 405:	Blk_BD(&membf[c],b,outfile);	break;
      case 1000:	Blk_DO(&membf[c],b,outfile);	break;
      case 1001:	Blk_QE(&membf[c],b,outfile);	break;
      default:
	fprintf(outfile,"%%Unknown Blockette %d Seen\n",b);
      }
    }
  }

  fprintf(outfile,"\n");

}


_SUB int dataformat(SEED_DATA *srec)
{

  int a,b,c;
  struct blkhdr *newblk;
  struct  Data_only *dat;
  char *membf;

  membf = (char *) srec;
  
  a = LW(srec->First_Blockette);
  
  FOREVER {
    if (a<=0 || a>4095) break;
    c = a;
    newblk = (struct blkhdr *) &membf[a];
    a = LW(newblk->blkxref);
    b = LW(newblk->blktype);
    switch (b) {
    case 1000:	
      dat = (struct Data_only *) &membf[c];
      return(dat->Encoding);
      break;
    default:
      break;
    }
  }

  return( -1 );
}

