#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_seed.h>
#include <seed_data.h>
#include <dcc_misc.h>

long STEIM_Decode(void *dataarray,long *userarr,
		  int numsam,int steimtype, int *actual);

_SUB int SH_Decode_SEED(long data[], SEED_DATA *inrec,int swdata,int level)
{

  int ret,retl,sam;

  sam = SH_Number_Samples(inrec);
  if (sam==0) return(sam);

  ret = STEIM_Decode(inrec, 
		     data, 
		     SH_Number_Samples(inrec),
		     level,
		     &retl);

  return(ret);

}

_SUB int SH_Decode_SEED_S2(long data[], SEED_DATA *inrec,int swdata)
{

  return(SH_Decode_SEED(data,inrec,swdata,2));

}

_SUB int SH_Decode_SEED_S3(long data[], SEED_DATA *inrec,int swdata)
{

  return(SH_Decode_SEED(data,inrec,swdata,3));

}

LOCAL LONG answer[4];

static struct harvformat {
  struct {
    ULONG	ctlflags;
    union {
      struct { BYTE byteval[4]; } type_1;
      struct { WORD wordval[2]; } type_2;
      struct { LONG longval;    } type_3;
    } chunkstore[15];
  } framestore[64];
} *multiword;

static struct harvheader1 {
  LONG	headbits;
  LONG	for_ic;
  LONG	rev_ic;
} *headerval;

static int CtlWordType(ULONG inlong,int pos)
{

  int sf,out;

  sf = (15-(pos+1))*2;
  if (sf>0) inlong >>= sf;

  out = inlong&0x03;
  return(out);
}

_SUB int SH_Decode_SEED_S1(long data[], SEED_DATA *inrec,int swdata)
{

  register BYTE i,frame,chunk,frtype;
  register LONG l_data,p_del;
  int ct,numans=0;
  LONG *oarray;
  BOOL fskip;
  int numsam;
  UBYTE *inbuf;
  int off;

  numsam = SH_Number_Samples(inrec);
  if (numsam==0) return(numsam);

  inbuf = (UBYTE *) inrec;
  off = SH_Start_Data(inrec);
  if (off==0) return(0);

  inbuf += off;

  ct = 0;
  oarray=(LONG *)data;

  multiword = (struct harvformat *) inbuf;
  headerval = (struct harvheader1 *) inbuf;

  l_data = Loc_LONG(headerval->for_ic,swdata);
  *oarray++ = l_data;
  ct++;
  numsam--;

  fskip = TRUE;			/* Toss the delta between records */

  for (frame=0;frame<64; frame++) { /* Usually only 63 */
    for (chunk=0; chunk<15; chunk++) {

      if (numsam<=0) break;	/* Out of samples */

      frtype = CtlWordType(Loc_LONG(multiword->framestore[frame].ctlflags,
				    swdata),chunk);

      if (frtype==0) continue;
      switch (frtype) {
      case 1:
	answer[0] = multiword->framestore[frame].
	  chunkstore[chunk].type_1.byteval[0];
	answer[1] = multiword->framestore[frame].
	  chunkstore[chunk].type_1.byteval[1];
	answer[2] = multiword->framestore[frame].
	  chunkstore[chunk].type_1.byteval[2];
	answer[3] = multiword->framestore[frame].
	  chunkstore[chunk].type_1.byteval[3];
	numans = 4;
	break;
      case 2:
	answer[0]=Loc_WORD(multiword->framestore[frame].
			       chunkstore[chunk].type_2.wordval[0],swdata);
	answer[1]=Loc_WORD(multiword->framestore[frame].
			       chunkstore[chunk].type_2.wordval[1],swdata);
	numans = 2;
	break;
      case 3:
	answer[0]=Loc_LONG(multiword->framestore[frame].
			       chunkstore[chunk].type_3.longval,swdata);
	numans = 1;
      }

      for (i=0; i<numans; i++) {
	if (fskip) fskip=FALSE;
	else {
	  p_del = l_data;
	  l_data += answer[i];
	  *oarray++ = l_data;
	  ct++;
	  numsam--;
	  if (numsam<=0) break;	/*14-Sep-90/SH*/
	}
      }
    }
  }

/*
  if (numsam!=0) 
    fprintf(stderr,"Numsam = %d??\n",numsam);

  if (p_del!=Loc_LONG(headerval->rev_ic)) {
    printf("Reverse integration constant not right!\n");
    printf("%ld (rev) is not equal to %ld (actual)\n",
	   Loc_LONG(headerval->rev_ic),p_del);
  }
*/

  return(ct);

}
