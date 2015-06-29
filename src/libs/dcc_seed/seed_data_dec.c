#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_seed.h>
#include <seed_data.h>
#include <dcc_misc.h>

/* 

   Internal SEED Codes 

   0 - ASCII Text
   1 - 16 bit integer
   2 - 24 bit integer
   3 - 32 bit integer
   4 - IEEE floating
   5 - IEEE double floating

   10 - Steim 1
   11 - Steim 2
   12 - Geoscope Multiplexed 24
   13 - Geoscope Multiplexed 16 bit 3 exp
   14 - Geoscope Multiplexed 16 bit 4 exp
   15 - USNSN compression
   16 - CDSN 16 bit gain range
   17 - Graefenberg 16 bit gain range
   18 - IPG - Strasbourg 16 bit gain
   19 - Steim 3

   30 - (A)SRO Format
   31 - HGLP Format
   32 - DWWSSN Gain Ranged Format
   33 - RSTN 16 bit gain ranged

*/


#define FORM_SRO 10
#define FORM_CDSN 20
#define FORM_16 30
#define FORM_32 40
#define FORM_SEED_S1 51
#define FORM_SEED_S2 52
#define FORM_SEED_S3 53
#define FORM_USNSN 15
#define FORM_ASCII 60

#define FORM_UNSUPPORTED 999

int data_swap_order = 1;

struct blk_begin {
  UWORD Blockette_Type;
  UWORD Next_Begin;
};

static char *_dispform(int form)
{

  switch(form) {
  case FORM_SRO: return("[SRO Gain Range]"); 
  case FORM_CDSN: return("[CDSN/RSTN Gain Range]"); 
  case FORM_16: return("[16 Bit Integers]"); 
  case FORM_32: return("[32 Bit Integers]"); 
  case FORM_SEED_S1: return("[Steim Compression 1]"); 
  case FORM_SEED_S2: return("[Steim Compression 2]"); 
  case FORM_SEED_S3: return("[Steim Compression 3]"); 
  case FORM_USNSN: return("[USNSN Compression]"); 
  case FORM_ASCII: return("[ASCII text]"); 
  case FORM_UNSUPPORTED: return("[Unsupported Format!]"); 
  default: return("[Illegal form type]");
  }

}

static void _dumprec(SEED_DATA *inrec)
{

  char buf[20];

  fprintf(stderr,"OFFENDING RECORD:");

  memcpy(buf, inrec->Seq_ID, 6);
  buf[6] = '\0';

  fprintf(stderr," Rec %s",buf);

  memcpy(buf, inrec->Network_ID, 2);
  buf[2] = '\0';

  fprintf(stderr," Net %s",buf);

  memcpy(buf, inrec->Station_ID, 5);
  buf[5] = '\0';

  fprintf(stderr," Stat %s",buf);

  memcpy(buf, inrec->Station_ID, 2);
  buf[2] = '\0';

  fprintf(stderr," Loc %s",buf);

  memcpy(buf, inrec->Channel_ID, 3);
  buf[3] = '\0';

  fprintf(stderr," Chan %s\n",buf);

}

static void _check_agreement(SEED_DATA *inrec, int form)
{

  void *blkst = (void *) inrec;
  int blkform = 0, blkencoding = 0;
  int blkt;

  for (blkt = SH_SWAP_WORD(inrec->First_Blockette);
       blkt != 0; ) {

    void *absblk = blkst + blkt;
    struct blk_begin *nblk = (struct blk_begin *) absblk;
    struct Data_only *newdol;
    
    if (SH_SWAP_WORD(nblk->Blockette_Type) != 1000) {
      blkt = SH_SWAP_WORD(nblk->Next_Begin);
      continue;
    }

    newdol = (struct Data_only *) nblk;

    blkencoding = newdol->Encoding;

    switch(blkencoding) {
    case 1:   blkform = FORM_16; break;
    case 2:   blkform = FORM_UNSUPPORTED; break;
    case 3:   blkform = FORM_32; break;
    case 4:   blkform = FORM_UNSUPPORTED; break;
    case 5:   blkform = FORM_UNSUPPORTED; break;

    case 10:   blkform = FORM_SEED_S1; break;
    case 11:   blkform = FORM_SEED_S2; break;
    case 12:   blkform = FORM_UNSUPPORTED; break;
    case 13:   blkform = FORM_UNSUPPORTED; break;
    case 14:   blkform = FORM_UNSUPPORTED; break;
    case 15:   blkform = FORM_USNSN; break;
    case 16:   blkform = FORM_CDSN; break;
    case 17:   blkform = FORM_UNSUPPORTED; break;
    case 18:   blkform = FORM_UNSUPPORTED; break;
    case 19:   blkform = FORM_SEED_S3; break;
      
    case 30:   blkform = FORM_SRO; break;
    case 31:   blkform = FORM_UNSUPPORTED; break;
    case 32:   blkform = FORM_16; break;
    case 33:   blkform = FORM_CDSN; break;

    default:
      bombout(EXIT_ABORT,"Saw unknown format SEED encoding %d in data (expecting %s)\n",
	      blkencoding,_dispform(form));
    }

    blkt = 0;

  }

  if (blkform==0) {
    /* There was no blockette 1000 in the data - go with default form */
    return;
  }

  if (form != blkform) {
    fflush(stderr); fflush(stdout);
    fprintf(stderr,"\n\nDiscrepancy between database and blockette 1000!\n");
    fprintf(stderr,"   Database format is %s\n",_dispform(form));
    fprintf(stderr,"   Blk 1000 encoding is %s (%d)\n",_dispform(blkform),
	    blkencoding);
    _dumprec(inrec);
    bombout(EXIT_ABORT,"\nCannot proceed - please fix me\n");
  }


}


_SUB int SH_Data_Decode(char *format, long data[], SEED_DATA *inrec,int swdata)
{

  int form=0;

  int sam = SH_Number_Samples(inrec);

  if (sam<=0) return(0);

  if (streq(format,"ASRO")) form=FORM_SRO;
  if (streq(format,"SRO")) form=FORM_SRO;

  if (streq(format,"RSTN")) form=FORM_CDSN;
  if (streq(format,"CDSN")) form=FORM_CDSN;

  if (streq(format,"DWWSSN")) form=FORM_16;
  if (streq(format,"16-BIT")) form=FORM_16;

  if (streq(format,"32-BIT")) form=FORM_32;

  if (streq(format,"HARV")) form=FORM_SEED_S1;
  if (streq(format,"SEED")) form=FORM_SEED_S1;
  if (streq(format,"SEEDS1")) form=FORM_SEED_S1;
  if (streq(format,"STEIM")) form=FORM_SEED_S1;
  if (streq(format,"STEIM1")) form=FORM_SEED_S1;

  if (streq(format,"SEEDS2")) form=FORM_SEED_S2;
  if (streq(format,"STEIM2")) form=FORM_SEED_S2;

  if (streq(format,"SEEDS3")) form=FORM_SEED_S3;
  if (streq(format,"STEIM3")) form=FORM_SEED_S3;

  if (streq(format,"NSN")) form=FORM_USNSN;
  if (streq(format,"USNSN")) form=FORM_USNSN;

  if (streq(format,"LOG")) form=FORM_ASCII;

  _check_agreement(inrec,form);

  switch (form) {
  case FORM_SRO:
    return(SH_Decode_SRO(data,inrec,swdata));
  case FORM_CDSN:
    return(SH_Decode_CDSN(data,inrec,swdata));
  case FORM_16:
    return(SH_Decode_16(data,inrec,swdata));
  case FORM_32: 
    return(SH_Decode_32(data,inrec,swdata));
  case FORM_SEED_S1:
    return(SH_Decode_SEED_S1(data,inrec,swdata));
  case FORM_SEED_S2:
    return(SH_Decode_SEED_S2(data,inrec,swdata));
  case FORM_SEED_S3:
    return(SH_Decode_SEED_S3(data,inrec,swdata));
  case FORM_USNSN:
    return(SH_Decode_SEED_USNSN(data,inrec,swdata));
  case FORM_ASCII:
    return(SH_Decode_ASCII(data,inrec,swdata));
  default:
    printf("Unknown format %s",format);
    return(-1);
  }
    
}

_SUB WORD SH_DEC_SWAP_WORD(WORD i,int swdata)
{

  if (swdata)
    return(LocGVAX_WORD(i));
  return(LocGM68_WORD(i));

}

_SUB LONG SH_DEC_SWAP_LONG(LONG i,int swdata)
{

  if (swdata)
    return(LocGVAX_LONG(i));
  return(LocGM68_LONG(i));

}

