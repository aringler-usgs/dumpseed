/* Oct. 28, 2002 GTM - modified to support data quality indicator flags D, Q and R */
#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_misc.h>
#include <seed_data.h>

#include <unistd.h>

#include "dumpctx.h"

int steim=0;
int osec=1;

int blk1001=0;
int blk1001_timing = 0;
int blk1001_usec = 0;

int inrec=0;
int outrec=0;

int blocksize;

FILE *opf;

struct stframes {
  unsigned long hdnibs;
  long data[15];
};

NETTIME recstart,nowrec;
int sectime = 0;
int wrtsams = 0;

int lasthole=0;
int mainsam = 0;

int dumpsamps = 0;

struct _frmsto {
  struct stframes iframe;
  long ic,rc;
  long delta[7*15];
  int numdeltas;
  int hole;
} frmsto[63];

void flushblks(char *recbuf,int pass)
{

  SEED_DATA *seedr = (SEED_DATA *) recbuf;
  int blkwlk;
  int any=0;
  int dsteim=1;

  blk1001=0;

  /* See if we've got anything to do here */

  for (blkwlk = LocGM68_WORD(seedr->First_Blockette);
       blkwlk!=0; ) {
    struct gen_blk_hdr *blk;
    int type;

    blk = (struct gen_blk_hdr *) &recbuf[blkwlk];
    type = LocGM68_WORD(blk->Blockette_Type);

    if (type==BLK_DATAONLY) {
      struct Data_only *mini = (struct Data_only *) &recbuf[blkwlk];

      switch(mini->Encoding) {
      case 10:
	dsteim = 1;
	break;
      case 11:
	dsteim = 2;
	break;
#if 0
      case 19:
	dsteim = 3;
	break;
#endif
      default:
	dsteim = 0;
	printf("Unknown format %d\n",mini->Encoding);
      }
    }

    if (type==BLK_DATAEXT) {
      struct Data_ext *ext = (struct Data_ext *) &recbuf[blkwlk];

      blk1001 = 1;
      blk1001_timing = ext->Timing;
      blk1001_usec = ext->Usec;
    }

    switch(type) {
    case BLK_DATAONLY:
    case BLK_DATAEXT:
      break;
    default:
      any = 1;
    }

    blkwlk = LocGM68_WORD(blk->Next_Begin);
  }

  if (pass)
    printf("[Steim %d]\n",dsteim);
  steim = dsteim;

}

int ctlhdr(unsigned long inlong,int pos)
{

  int sf,out;
  unsigned long lpr = LocGM68_LONG(inlong);

  sf = (15-(pos+1))*2;
  if (sf>0) lpr >>= sf;

  out = lpr&0x03;
  return(out);
}

unsigned long masks[32] = {
  (0x1L-1),
  (0x2L-1),
  (0x4L-1),
  (0x8L-1),

  (0x10L-1),
  (0x20L-1),
  (0x40L-1),
  (0x80L-1),

  (0x100L-1),
  (0x200L-1),
  (0x400L-1),
  (0x800L-1),

  (0x1000L-1),
  (0x2000L-1),
  (0x4000L-1),
  (0x8000L-1),

  (0x10000L-1),
  (0x20000L-1),
  (0x40000L-1),
  (0x80000L-1),

  (0x100000L-1),
  (0x200000L-1),
  (0x400000L-1),
  (0x800000L-1),

  (0x1000000L-1),
  (0x2000000L-1),
  (0x4000000L-1),
  (0x8000000L-1),

  (0x10000000L-1),
  (0x20000000L-1),
  (0x40000000L-1),
  (0x80000000L-1) };

unsigned long bit[32] = {
  (0x1L),
  (0x2L),
  (0x4L),
  (0x8L),

  (0x10L),
  (0x20L),
  (0x40L),
  (0x80L),

  (0x100L),
  (0x200L),
  (0x400L),
  (0x800L),

  (0x1000L),
  (0x2000L),
  (0x4000L),
  (0x8000L),

  (0x10000L),
  (0x20000L),
  (0x40000L),
  (0x80000L),

  (0x100000L),
  (0x200000L),
  (0x400000L),
  (0x800000L),

  (0x1000000L),
  (0x2000000L),
  (0x4000000L),
  (0x8000000L),

  (0x10000000L),
  (0x20000000L),
  (0x40000000L),
  (0x80000000L) };

unsigned long extens[32] = {
  ~(0x1L-1),
  ~(0x2L-1),
  ~(0x4L-1),
  ~(0x8L-1),

  ~(0x10L-1),
  ~(0x20L-1),
  ~(0x40L-1),
  ~(0x80L-1),

  ~(0x100L-1),
  ~(0x200L-1),
  ~(0x400L-1),
  ~(0x800L-1),

  ~(0x1000L-1),
  ~(0x2000L-1),
  ~(0x4000L-1),
  ~(0x8000L-1),

  ~(0x10000L-1),
  ~(0x20000L-1),
  ~(0x40000L-1),
  ~(0x80000L-1),

  ~(0x100000L-1),
  ~(0x200000L-1),
  ~(0x400000L-1),
  ~(0x800000L-1),

  ~(0x1000000L-1),
  ~(0x2000000L-1),
  ~(0x4000000L-1),
  ~(0x8000000L-1),

  ~(0x10000000L-1),
  ~(0x20000000L-1),
  ~(0x40000000L-1),
  ~(0x80000000L-1) };

void procbuf(char *recbuf,int pass,struct steimctx *inctx,int *sts)
{

  SEED_DATA *seedr = (SEED_DATA *) recbuf;
  int blk,fm;
  int hdr[16],subhdr[16];
  long fic=0,rval=0,frc=0;
  int ics=0;
  int fr,del,fst=0;
  int lasth=0			/*,first=0*/;

  int numsam=0;
  int totdel=0;
  lasthole = 0;

  if (strncmp(seedr->Channel_ID,"LOG",3)==0) return;

  nowrec = recstart = seedr->Start_Time;
  sectime = 0;
  wrtsams = 0;
  /*printf("Blockettes = %d\n",LocGM68_WORD(seedr->First_Blockette));*/

  mainsam = numsam = LocGM68_WORD(seedr->Number_Samps);
  /*printf("Number samples %d\n",numsam);*/

  /* Flush out any blockettes as separate records */
  flushblks(recbuf,pass);
  if (steim==0) return;

  for (blk = LocGM68_WORD(seedr->Data_Start),fm=0;
       blk < blocksize; blk += 64,fm++) {

    void *iptr;
    struct stframes *mpt;
    int i;

    if (pass)
      printf("\nFrame = %d\n",fm);

    iptr = (void *) &recbuf[blk];
    mpt = iptr;

    for (i=0; i<16; i++) { 
      hdr[i] = ctlhdr(mpt->hdnibs,i);
      subhdr[i] = 0;
    }
	     
    memcpy((void *) &frmsto[fm].iframe, iptr, sizeof(struct _frmsto));

    /* Retreive the integrating constants */

    frmsto[fm].hole=0;
    frmsto[fm].ic=0;
    frmsto[fm].rc=0;

    if (hdr[0]==0 && hdr[1]==0) {
      frmsto[fm].hole=1;
      lasthole = fm;
      frmsto[fm].ic=LocGM68_LONG(mpt->data[0]);
      frmsto[fm].rc=LocGM68_LONG(mpt->data[1]);
      /*printf("IC Storage FC=%ld RC=%ld\n",
	frmsto[fm].ic,
	frmsto[fm].rc);*/
      if (ics==0) {
	rval = fic = frmsto[fm].ic;
	frc = frmsto[fm].rc;
	ics = 1;
      }
    }

    frmsto[fm].numdeltas = 0;

    for (i=0; i<15; i++) {
      unsigned long pdat;
      int ctype=0;
      register int width=0, slots=0, sl=0;
      int startdeltas;

      /* Flip our data longword around */

      pdat = LocGM68_LONG(mpt->data[i]);

      /* Pull the main code type */

      ctype = hdr[i] * 10;

      if (pass)
	printf("   Word %d - [Code %d",i+1,hdr[i]);

      /* Get the subcode if steim2 */

      if (steim==2) {
	switch (hdr[i]) {
	  /* Type 0 and 1 are same as steim 1 */
	case 0:
	case 1:
	  break;
	  /* Type 2 and 3 have a subcode */
	case 2:
	case 3:
	  ctype += 4 + ((pdat>>30)&0x3);
	  if (pass)
	    printf(" sub %ld",((pdat>>30)&0x3));
	  break;
	}
      }

      /* Build width and slots from codes */

      switch(ctype) {
      case 10:			/* 4 8-bit */
	width = 8;
	slots = 4;
	break;
      case 20:			/* 2 16-bit */
	width = 16;
	slots = 2;
	break;
      case 25:			/* 1 30-bit */
	width = 30;
	slots = 1;
	break;
      case 26:			/* 2 15 bit */
	width = 15;
	slots = 2;
	break;
      case 27:			/* 3 10 bit */
	width = 10;
	slots = 3;
	break;
      case 30:			/* 1 32-bit */
	width = 32;
	slots = 1;
	break;
      case 34:			/* 5 6-bit */
	width = 6;
	slots = 5;
	break;
      case 35:			/* 6 5-bit */
	width = 5;
	slots = 6;
	break;
      case 36:			/* 7 4-bit */
	width = 4;
	slots = 7;
	break;
      case 0:			/* No data or default */
	width = 0;
	slots = 0;
	break;
      default:
	width = 999;
	slots = 0;
	break;
      }

      if (pass)
	printf("] ");

      if (width==999) {
	(*sts) |= STEIM_BADTYPECODE;
	if (pass)
	  printf("\nError code - type %d\n",ctype);
      }

      if (ctype==0) {
	if (pass) {
	  if (fm!=0) printf("Spare ");
	  else printf("Primary ");
	  if (i==0) printf("Forward ");
	  if (i==1) printf("Reverse ");
	  printf("Constant=%ld\n",(long) LocGM68_LONG(frmsto[fm].iframe.data[i]));
	}
      } else
	if (pass)
	  printf("%d %d-bit Deltas=",slots,width);

      /* Pull deltas out of longword */

      startdeltas = frmsto[fm].numdeltas;
      if (slots) {
	for (sl=0; sl<slots; sl++) {

	  /* Extract the bit span */

	  if (sl<slots-1)
	    frmsto[fm].delta[frmsto[fm].numdeltas++] = 
	      (pdat>>width*(slots-sl-1))&masks[width];
	  else
	    frmsto[fm].delta[frmsto[fm].numdeltas++] = 
	      pdat&masks[width];

	  /* Perform sign extension if necessary */

	  if (width<32) {
	    if (frmsto[fm].delta[frmsto[fm].numdeltas-1]&bit[width-1])
	      frmsto[fm].delta[frmsto[fm].numdeltas-1] |= extens[width];
	  }
	}
	for (sl=startdeltas; sl<frmsto[fm].numdeltas; sl++)
	  if (pass)
	    printf("%ld ",frmsto[fm].delta[sl]);
	if (pass)
	  printf("\n");
      }
    }
  }

  if (pass)
    printf("\nSaw %d frames\n",fm);
  for (fr=0; fr<fm; fr++) {
    if (pass && dumpsamps)
      printf("\nFrame = %d: ",fr);
    if (frmsto[fr].hole) {
      lasth = fr;
      fst = 0;
      if (pass && dumpsamps)
	printf("IC=%ld RC=%ld\n",frmsto[fr].ic,frmsto[fr].rc);
    }
    for (del=0; del<frmsto[fr].numdeltas; del++) {
      if (ics++ ==1) {
	if (pass && dumpsamps)
	  printf(" %ld",rval);
	if (fst==0)
	  /*frmsto[lasth].ic = rval;*/
	  fst = 1;
	/*frmsto[lasth].rc = rval;*/
	continue;		/* Throw away interrec delta */
      }
      rval += frmsto[fr].delta[del];
      /*frmsto[lasth].rc = rval;*/
      if (pass && dumpsamps)
	printf(" %ld",rval);
    }
    /*printf("\nRecompute IC=%ld RC=%ld - deltas=%d\n",
      frmsto[lasth].ic,frmsto[lasth].rc,frmsto[lasth].numdeltas);*/

    /*frmsto[lasth].iframe.data[0] = M68GLoc_LONG(frmsto[lasth].ic);
      frmsto[lasth].iframe.data[1] = M68GLoc_LONG(frmsto[lasth].rc);*/
  }

  /* Check to see if the reverse IC is valid */

  if (rval==frc) { 
    if (pass)
      printf("\nDecode OK!\n");
  }
  else {
    (*sts) |= STEIM_RCMISMATCH;
    if (pass)
      printf("\nBAD DECODE--MISMATCH\n");
    if (pass==0)
      printf("Reverse Integration constant mismatch\n");
  }

  /* Do an inter-record context check x(0) = x(-1) + d(0) */

  if (pass==0) {

    if (inctx->init) {
      if (fic!=inctx->prevrc+frmsto[0].delta[0]) {
	if (!(seedr->Activity_Flags&ACTFLAG_BEGEVT)) {
	  (*sts) |= STEIM_INTERCONTEXT;
	  printf("Inter-Record Context Mismatch: %ld != %ld + %ld\n",
		 fic,inctx->prevrc,frmsto[0].delta[0]);
	}
      }
    }

    /* Init context block */

    inctx->init ++;
    inctx->prevrc = frc;
  }

#if 0  
  lasth = 0;
  first = 0;
  for (fr=0; fr<fm; fr++) {
    totdel += frmsto[fr].numdeltas;
    if (frmsto[fr].hole) {
      if (fr>0)
	flushhole(lasth,fr,first,0);
      first=0;
      lasth = fr;
    }
  }
  flushhole(lasth,fr,0,1);
#endif
  if (pass)
    printf("Samps=%d - Deltas=%d\n\n",numsam,totdel);
      
  if (((*sts) & STEIM_BADTYPECODE) && pass==0) 
    printf("Steim data contains bogus type codes\n");

}

#if 0
int main(int argc, char **argv)
{

  FILE *ipf;

  if (argc!=3) {
    printf("Usage: %s 4096-byte-seed-file blocksize\n",argv[0]);
    exit(10);
  }

  if (strcmp(argv[1],"-")!=0) {
    ipf = fopen(argv[1],"r");
    if (ipf==NULL) {
      printf("cutter: Can't read file %s\n",argv[1]);
      exit(10);
    }
  }
  else 
    ipf = stdin;

  blocksize = atoi(argv[2]);
  switch(blocksize) {
  case 256:
  case 512:
  case 4096:
    break;
  default:
    bombout(EXIT_ABORT,"Blocksize is bad %d",blocksize);
  }

  while (!feof(ipf)) {
    int err;

    err = read(fileno(ipf), (void *) recbuf, blocksize);

    if (err==0) break;
    if (err!=blocksize) {
      printf("Got read %d\n",err);
      exit(10);
    }

    inrec++;
    if ( recbuf[6] == 'D' ||  recbuf[6] == 'Q' ||  recbuf[6] == 'R' )
      procbuf(recbuf);
  }

  if (ipf!=stdin)
    fclose(ipf);

  exit(0);

}
#endif

_SUB int seedanalyze(SEED_DATA *seed_rec,int form,char *formtxt,int blocks,
		     int pass,struct steimctx *inctx)
{

  int sts;

  sts = 0;

  blocksize = blocks;

  steim = 0;
  switch(form) {
  case 10:
    steim = 1;
    break;
  case 11:
    steim = 2;
    break;
  default:
    bombout(EXIT_ABORT,"Unsupported format: %s",formtxt);
  };

  procbuf((char *) seed_rec,pass,inctx,&sts);

  return(sts);
  
} 
