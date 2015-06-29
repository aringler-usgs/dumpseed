/* Filename: dumpseed.c
 *
 * Modifications:
 *   Oct. 28, 2002 GTM - modified to support data quality indicator flags D, Q and R 
 *     and to upgrade the "Usage" statement
 *
 *   Nov. 15, 2006 GTM - modified to handle files with a mix of recordsizes and to eliminate the -r option
 *
 *   Oct. 15, 2008 GTM - modified during port to Linux/Intel.  This was built as a 32-bit program because
 *   of the problems with the Steim code, which is not compatible with compilation in 64-bit mode.
 *
 *   Sept. 1, 2010 GTM - rewrote support for endian byte swaps.  The byte swaps occur one time -- immediately
 *   after the record is read.  The SH_* macros and function calls containing byte swaps were removed.
 *   Macros and constants for host endian testing and byte swapping are in endian_32bit.h
 *   Note - two copies of the seed record are used. A 'raw' copy is kept and not byte-swapped.  This is the
 *   one that is passed to the Steim decode function to get the actual samples.  Those functions still do
 *   their own byte swapping and were left unaltered.
 *
 *   Jan. 15, 2013 - Version 2.01
 *     - bug fix for reversal - added abs() to handle reversals
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>
#include <seed_data_proto.h>
#include <dcc_misc.h>
#include <dcc_seed_select.h>

#include "dumpctx.h"
#include "dumpseed_proto.h"
#include "endian_32bit.h"

#define MAX_FILES 8192
#define PROGNAME "dumpseed"
#define VERSION "2.01"
#define VERSION_DATE "Jan. 15, 2013"

int host_endian = -1;

#define SEED_RECORD_SIZE 4096

UBYTE *tape_read_buff=NULL;
UBYTE *tape_read_buff_raw=NULL;
UBYTE *seed_rec_buf;
SEED_DATA *seed_rec;
SEED_DATA *seed_rec_raw;
extern int data_swap_order;

LONG decomp_samples[SEED_RECORD_SIZE*2];

int decode  = 0;
int export  = 0;
int quiet   = 0;
int silent  = 0;
int analyze = 0;
int gaplist = 0;

int record_length;
int bufsize;

long recnum = -1;

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

char *pidfile=NULL;

char defaultformat[20];
char defaultformnum=0;
char decodeformat[20];
char chanpick[20];
char locpick[20];
int chanpickp=0;
int locpickp=0;
int telemetry=0;
int ignore_blockettes=0;
int tel_port=4000;
char *select_file = NULL;
int caldelay = 0;

struct sel3_root dumproot;

FILE *seediov;

ITEMLIST *taglist=NULL;

BOOL fixed_recsize = FALSE;

struct chancontext {
  char chank[15];
  int infochan;
  STDTIME earliest;
  STDTIME latest;
  STDTIME stspan;
  STDTIME last;
  int totalrec;
  struct steimctx stoctx;
  struct chancontext *next;
} *chanroot=NULL,*chantail=NULL,*currentchan=NULL;


/* ===================================================== */

STDTIME Start_Time(SEED_DATA *pblk) {

  STDTIME blktim;

  blktim.year = pblk->Start_Time.year;
  blktim.day = pblk->Start_Time.day;
  blktim.hour = pblk->Start_Time.hour;
  blktim.minute = pblk->Start_Time.minute;
  blktim.second = pblk->Start_Time.seconds;
  blktim.msec = pblk->Start_Time.fracs / 10;

  return(blktim);
}


/* ===================================================== */
LONG Record_Time(SEED_DATA *pblk) {

  LONG numsam,msdif;
  STDFLT samrat,samtim;
  WORD factor,rate;

  numsam = pblk->Number_Samps;

  factor = pblk->Rate_Factor; 
  rate = pblk->Rate_Mult;

  if (rate == 0) {                /* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = rate;
  }

  msdif = 0;
  samrat = 0.0;
  samtim = 0.0;

  if (rate != 0 && factor != 0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else        samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else        samrat /= ((STDFLT) (-msdif));

    samrat = 1.0/samrat;
    samrat*=1000.0;             /* Ms/Sam */
    samtim = samrat * ((STDFLT) numsam);
  }

  msdif = samtim;

  return(msdif);

}

/* ===================================================== */
long half_sample(SEED_DATA *pblk) {

  LONG msdif;
  STDFLT samrat, samtim;
  WORD factor,rate;
  long half_samp;

  factor = pblk->Rate_Factor; 
  rate = pblk->Rate_Mult;

  if (rate == 0) {                /* SIO Data bug */
    rate = 1;
    pblk->Rate_Mult = rate;
  }

  samrat = 0.0;
  samtim = 0.0;

  if (rate != 0 && factor != 0) {
    msdif = factor;
    if (msdif>=0) samrat = msdif;
    else        samrat = 1.0 / ((STDFLT) (-msdif));
    msdif = rate;
    if (msdif>=0) samrat *= ((STDFLT) (msdif));
    else        samrat /= ((STDFLT) (-msdif));

    samrat = 1.0/samrat;
    samrat*=1000.0;             /* Ms/Sam */
  }

  half_samp =  (long)(0.5 * samrat);
  return(half_samp);

}

/* ===================================================== */

STDTIME End_Time(SEED_DATA *pblk) {

  return(ST_AddToTime(Start_Time(pblk), 0, 0, 0, 0, Record_Time(pblk)));

}

/* ===================================================== */
/* look in 1st 64 bytes of record to get Data Record Length from B1000 */
int get_reclen_from_B1000(SEED_DATA *seedrec) {
  int reclen = 0;
  int index;
  char scan;
  struct gen_blk_hdr *blockette_header;
  struct  Data_only *B1000;
  char *buffer = (char *) seedrec;
  int type, next_begin;

  if (seedrec->Total_Blockettes > 0) {   /* blockettes are present, look for B1000 */
    index = 48;
    scan = TRUE;
    while ( scan ) {
      if (index <= 0 || index > record_length) {
        break;
      }
      blockette_header = (struct gen_blk_hdr *) &buffer[index];
      if (host_endian == ENDIAN_LITTLE) {
	type = SwapTwoBytes(blockette_header->Blockette_Type);
      } else {
        type = blockette_header->Blockette_Type;
      }
      if (type == 1000) {
        B1000 = (struct  Data_only *) &buffer[index];
        reclen = B1000->Length;
        scan = FALSE;
        continue;
      }
      if (host_endian == ENDIAN_LITTLE) {
	next_begin = SwapTwoBytes(blockette_header->Blockette_Type);
      } else {
	next_begin = blockette_header->Blockette_Type;
      }

      if (next_begin == 0) { /* no more blockettes after this one */
        scan = FALSE;
      }
      index = next_begin;
    }
  }
  return(reclen);
}  /* matches sub get_recordlength */


/* ===================================================== */
/* If host is little endian, do the byte swapping for fixed header and blockettes  */
void swap_endian(SEED_DATA *seedrec) {
  int index;
  char scan;
  struct gen_blk_hdr *blockette_header;
  char *buffer = (char *) seedrec;

  if (host_endian == ENDIAN_BIG) {  /* data already in big endian */
    return;
  }

  /* fixed header fields requiring byte swapping:                                           */
  /*    record_start_time (year, day and fractional (.0001) seconds fields are 16bit        */
  /*    number of samples - 16 bit                                                          */
  /*    sample rate factor - 16 bit                                                         */
  /*    time correction - 32 bit                                                            */
  /*    beginning of daa - 16 bit                                                           */
  /*    first blockette - 16 bit                                                            */

  seedrec->Start_Time.year = SwapTwoBytes(seedrec->Start_Time.year);
  seedrec->Start_Time.day = SwapTwoBytes(seedrec->Start_Time.day);
  seedrec->Start_Time.fracs = SwapTwoBytes(seedrec->Start_Time.fracs);
  seedrec->Number_Samps = SwapTwoBytes(seedrec->Number_Samps);
  seedrec->Rate_Factor = SwapTwoBytes(seedrec->Rate_Factor);
  seedrec->Rate_Mult = SwapTwoBytes(seedrec->Rate_Mult);
  seedrec->Data_Start = SwapTwoBytes(seedrec->Data_Start);
  seedrec->First_Blockette = SwapTwoBytes(seedrec->First_Blockette);
  seedrec->Time_Correction = SwapFourBytes(seedrec->Time_Correction);

  /* run through the blockettes present and swap endians accordingly */
  if (seedrec->Total_Blockettes > 0) {   /* blockettes are present, look for B1000 */
    index = 48;
    scan = TRUE;
    while ( scan ) {
      if (index <= 0 || index > record_length) {
        break;
      }
      blockette_header = (struct gen_blk_hdr *) &buffer[index];

      /* all blockettes have Blockette type and next blockette's byte number */
      blockette_header->Blockette_Type = SwapTwoBytes(blockette_header->Blockette_Type);
      blockette_header->Next_Begin = SwapTwoBytes(blockette_header->Next_Begin);

      /* swap blockette-specific field according to type */     
      switch (blockette_header->Blockette_Type)  {
      case 1000:  /* no multi-byte fields */
	break;
      case 1001:  /* no multi-byte fields */
	break;
      default:
        printf("Blockette %d present\n", blockette_header->Blockette_Type);
        break;
      }  /* matches switch */

      if (blockette_header->Next_Begin == 0) { /* no more blockettes after this one */
        scan = FALSE;
      }
      index = blockette_header->Next_Begin;

    } /* matches while */
  }
  return;
}  /* matches void swap_endian */


/* ===================================================== */
void setchan(SEED_DATA *seedr)
{

  char *sr = (char *) seedr;
  struct chancontext *loop;

  char chank[15];

  strncpy(chank, &sr[8], 12);
  chank[12] = '\0';

  for (loop = chanroot; loop != NULL; loop = loop->next) {
    if (strcmp(chank,loop->chank) == 0) {
      currentchan = loop;
      return;
    }
  }

  loop = (struct chancontext *) malloc(sizeof(struct chancontext));
  if (loop==NULL) {
    fprintf(stderr,"Cannot allocate for channel context\n");
    exit(20);
  }

  strcpy(loop->chank, chank);
  loop->next = NULL;

  loop->infochan = 0;
  
  if ( seedr->Rate_Factor == 0 || seedr->Rate_Mult ==0 ) {
    loop->infochan = 1;
  }

  loop->earliest = Start_Time(seedr);
  loop->latest   = End_Time(seedr);
  loop->stspan   = Start_Time(seedr);
  loop->last     = End_Time(seedr);
  
  loop->stoctx.init = 0;

  currentchan = loop;

  if (chanroot)
    chantail->next = loop;
  else
    chanroot = loop;

  chantail = loop;

  return;
}

void flushgaps()
{

  struct chancontext *loop;

  for (loop=chanroot; loop!=NULL; loop=loop->next) {

    printf("%s/",  pullasc(&loop->chank[10],2));
    printf("%s/",  pullasc(&loop->chank[0],5));
    printf("%s/",  pullasc(&loop->chank[5],2));
    printf("%s: ", pullasc(&loop->chank[7],3));

    if (!loop->infochan) {

      printf("%s-->", ST_PrintDate(loop->stspan,TRUE));
      printf("%s", ST_PrintDate(loop->last,TRUE));

    }

    printf(" - records %d\n", loop->totalrec);

  }
}

void dumpout(long ms, int form)
{

  int a;
  ITEMLIST *tagloop = NULL;
  int anyact = 0;
  
  if (!export && !gaplist) {

    prtseedrec(seed_rec, !ignore_blockettes, stdout, ms, recnum);

    anyact = 0;
    for (tagloop = taglist; tagloop != NULL; tagloop = tagloop->next) {
      if (!anyact) 
	printf("Select Tags:");
      anyact = 1;
      printf(" %s", tagloop->item);
    }
    if (anyact) 
      printf("\n");

  }
  if (export) {

    /* Write the data as BDF */
  
    prtseedbdf(seed_rec,!ignore_blockettes,stdout);

    for (tagloop=taglist; tagloop!=NULL; tagloop=tagloop->next)
      printf("TAG %s\n", tagloop->item);

  }
  
  if ( (seed_rec->Record_Type == 'D' || seed_rec->Record_Type == 'Q'  || seed_rec->Record_Type == 'R' )
       && analyze && (seed_rec->Number_Samps> 0))
    if (!silent)
      (void) seedanalyze(seed_rec, form, decodeformat, bufsize, 1, &currentchan->stoctx);

  if (decode) {
    if (export) {
      printf("FORM%s\n", decodeformat);
      printf("DATA\n");
    }

    if (streq(decodeformat,"LOG")) {
      for (a=0; a<seed_rec->Number_Samps; a++) {
	char *dc;
	dc = (char *) &seed_rec_buf[seed_rec->Data_Start];
	dc += a;
	if (export) 
	  printf("%d\n",*dc);
	else
	  if (*dc!=0) {
	    if (*dc=='\r') {
	      putchar('\n');
	      if (a+1 < seed_rec->Number_Samps)
		if (*(dc+1)=='\r' || *(dc+1)=='\n')
		  *(dc+1) = '\0';
	    }
	    else
	    if (*dc=='\n') {
	      putchar('\n');
	      if (a+1 < seed_rec->Number_Samps)
		if (*(dc+1)=='\r' || *(dc+1)=='\n')
		  *(dc+1) = '\0';
	    }
	    else
	      putchar(*dc);
	  }
      }
      printf("\n");
    } else 
      if (!streq(decodeformat, "UNKNOWN")) {
	int actual,numsamps;
	numsamps = seed_rec->Number_Samps;
	actual = SH_Data_Decode(decodeformat, (long *) decomp_samples, 
				seed_rec_raw, data_swap_order);
	for (a = 0; a < min(numsamps,actual); a++) 
	  if (export) 
	    printf("%d\n", decomp_samples[a]);
	  else
	    printf("%d ", decomp_samples[a]);
	if (!export) 
	  printf("\n");
	if (actual < numsamps) {
	  if (export)
	    printf("ERR Record had fewer samples (%d) than header (%d)\n",
		   actual,numsamps);
	  else
	    fprintf(stderr,
		    "%%%%Record had fewer samples (%d) than header (%d)\n",
		   actual,numsamps);
	}
      }
  }
}

void dumpseed()
{
  
  UBYTE 	buff[100];
  UBYTE *membf;
  char chan[20];
  int form;
  STDTIME rec_begin,rec_end;
  long ms;
  int anstat;
  
  membf = (UBYTE *) seed_rec;

  if (seed_rec->Record_Type != 'D' && seed_rec->Record_Type != 'Q' && seed_rec->Record_Type != 'R') return;

  memcpy(buff, seed_rec->Channel_ID,3);
  buff[3] = '\0';
  TrimString(buff);
  strcpy(chan,buff);

  if (chanpickp)
    if (!WildMatch(chanpick, buff)) return;

  memcpy(buff,seed_rec->Location_ID,2);
  buff[2] = '\0';
  TrimString(buff);

  if (locpickp)
    if (!WildMatch(locpick, buff)) return;

  rec_begin = Start_Time(seed_rec);
  rec_end = End_Time(seed_rec);

  if (select_file) {
    int interesting;
    char n[10],s[10],l[10],c[10];

    SH_Get_Idents(seed_rec,n,s,l,c);
    TrimString(n);
    Upcase(n);
    TrimString(s);
    Upcase(s);
    TrimString(l);
    Upcase(l);
    TrimString(c);
    Upcase(c);

    interesting = SelectDateInteresting(&dumproot,
					rec_begin,
					rec_end,
					n,s,l,c,
					&taglist);

    if (!interesting) return;

  }

  if (seed_rec->Record_Type == 'D' || seed_rec->Record_Type == 'Q' || seed_rec->Record_Type == 'R') 
    setchan(seed_rec);

  currentchan->totalrec ++;
  currentchan->earliest = 
    ST_TimeMin(currentchan->earliest,rec_begin);
  currentchan->latest = 
    ST_TimeMax(currentchan->latest,rec_end);

  ms = 0;

  if (currentchan->totalrec  > 1) {
    ms = ST_DeltaToMS(ST_DiffTimes(rec_begin, currentchan->last));
  }

  /* GTM Aug. 31 2010 - threshold for gap is now 1/2 sample in milliseconds */
  if ( abs(ms) < half_sample(seed_rec) ) {
    ms = 0;
  }

  if (gaplist && currentchan->totalrec > 1 && ms && !currentchan->infochan) {

    printf("%s/",  pullasc(&currentchan->chank[10],2));
    printf("%s/",  pullasc(&currentchan->chank[0],5));
    printf("%s/",  pullasc(&currentchan->chank[5],2));
    printf("%s: ", pullasc(&currentchan->chank[7],3));

    printf("%s-->", ST_PrintDate(currentchan->stspan,TRUE));
    printf("%s",    ST_PrintDate(currentchan->last,TRUE));

    printf(" (%ld ms %s)\n",  ms,ms>0?"gap":"rev");

    currentchan->stspan = rec_begin;
  }
    
  currentchan->last = rec_end;

  /* Get the format from the record */

  form= dataformat(seed_rec);

  if (form<0) form = defaultformnum;

  if (streq(chan,"LOG")) 
    form=50;

  switch(form) {
  case 0:  strcpy(decodeformat,"LOG"); break;
  case 1:  strcpy(decodeformat,"DWWSSN"); break;
  case 3:  strcpy(decodeformat,"32-BIT"); break;
  case 10:   strcpy(decodeformat,"STEIM1"); break;
  case 11:   strcpy(decodeformat,"STEIM2"); break;
  case 15:   strcpy(decodeformat,"USNSN"); break;
  case 19:   strcpy(decodeformat,"STEIM3"); break;
  case 16:   strcpy(decodeformat,"CDSN"); break;
  case 20:   strcpy(decodeformat,"STEIM3"); break;
  case 30:   strcpy(decodeformat,"SRO"); break;
  case 32:   strcpy(decodeformat,"DWWSSN"); break;
  case 33:   strcpy(decodeformat,"CDSN"); break;
  case 50:  strcpy(decodeformat,"LOG"); break;
  default:
    strcpy(decodeformat,"UNKNOWN");
    printf("Don't know how to decode format %d\n",form);
  }

  if (silent && analyze) {

    anstat = 0;

    if (seed_rec->Number_Samps > 0)
      anstat = seedanalyze(seed_rec, form, decodeformat, bufsize, 0, &currentchan->stoctx);

    if (anstat==0) return;

  }

  dumpout(ms,form);

}

/* Nov. 15, 2006 - GTM - modified to support dynamically determined record size */
BOOL get_nexrec() {
  int bytes_read;
  long file_pos_ind;

  if (feof(seediov)) {
    return(FALSE);
  }

  if (tape_read_buff == NULL) {
    tape_read_buff = (UBYTE *) malloc(bufsize);
    tape_read_buff_raw = (UBYTE *) malloc(bufsize);
    if (tape_read_buff == NULL) {
      bombout(EXIT_INSFMEM, "Cannot allocate buffer %d",bufsize);
    }
  }


  /* if the recordsize has been fixed via the command line, observe it */
  if ( fixed_recsize == TRUE ) {
    bytes_read = fread( (void *)tape_read_buff, record_length, 1, seediov);
    if (bytes_read <= 0) {
      return(FALSE);
    }
  } else {
    /* save the current file position indicator and read in 64 bytes  */
    file_pos_ind = ftell(seediov);
    bytes_read = fread( (void *)tape_read_buff, 64, 1, seediov);

    if (bytes_read <= 0) {
      return(FALSE);
    }

    /* Look for Blockette 1000, if present use record lenth from there (256) and 9 (512) and 12 (4096) */
    switch( get_reclen_from_B1000((SEED_DATA *)tape_read_buff) ) {
     case 0:
       printf("Error - Blockette 1000 not found, cannot determine record length \n");
       exit(1);
     case 8:  
       record_length = 256; 
       break;
     case 9:  
       record_length = 512; 
       break;
     case 12:  
       record_length = 4096; 
       break;
     default:
       printf("Error - unsupported record length in B1000 (%d) \n", get_reclen_from_B1000((SEED_DATA *)tape_read_buff));
       printf("(Can autodetect record length only for 256, 512 or 4096 byte formats)\n");
       exit(1);
    }

    /* restore file position indicator and read in the record with the indicated length */
    if (fseek(seediov, file_pos_ind, SEEK_SET) != 0) {
      fprintf(stdout, "fseek error = %s\n", strerror(errno));
      exit(1);
    }
    bytes_read = fread( (void *)tape_read_buff, record_length, 1, seediov);
    if (bytes_read <= 0) {
      return(FALSE);
    }
  }

  /* save a raw, unaltered version of the mini-SEED record which is used when decompressing */
  memcpy( &tape_read_buff_raw[0], &tape_read_buff[0], bufsize);
  seed_rec_buf = &tape_read_buff_raw[0];
  seed_rec_raw = (SEED_DATA *) seed_rec_buf; 

  /* process the record */
  seed_rec_buf = &tape_read_buff[0];
  seed_rec = (SEED_DATA *) seed_rec_buf; 
  /* Perform endian swaps if needed */
  swap_endian(seed_rec);

  recnum++;
  dumpseed();
  fflush(stdout);
  fflush(stderr);
  if (pidfile) {
    watchdog(pidfile);
  }

  return(TRUE);

}


/* process_record is only used when -t option specified.  It is just a transfer function to dumpseed() */
void process_record(char *seedrec) {

  if (tape_read_buff_raw == NULL) {
    tape_read_buff_raw = (UBYTE *) malloc(bufsize);
    if (tape_read_buff_raw == NULL) {
      bombout(EXIT_INSFMEM, "Cannot allocate buffer %d",bufsize);
    }
  }

  /* save an unaltered version of the mini-SEED record which is used when decompressing */
  memcpy( &tape_read_buff_raw[0], &seedrec[0], 512);
  seed_rec_buf = &tape_read_buff_raw[0];
  seed_rec_raw = (SEED_DATA *) seed_rec_buf;

  /* recast argument as a SEED_DATA struct and perform endian swaps if needed */
  seed_rec_buf = (UBYTE *) seedrec;
  seed_rec = (SEED_DATA *) seed_rec_buf;
  swap_endian(seed_rec);

  dumpseed();
  fflush(stdout);
  fflush(stderr);
  if (pidfile) {
    watchdog(pidfile);
  }
  return;
}

void usage(void)
{
  fprintf(stderr, "%s Version: %s Version Date: %s\n", PROGNAME, VERSION, VERSION_DATE);
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "   %s [-a] [-b <recordsize>] [-c <channel_spec>] [-d] [-D] [-e] [-f <comp_format>] [-E]\n", PROGNAME);
  fprintf(stderr, "   [-g] [-i] [-l <loc_code_spec>] [-p <port>] [-P <pidfile>] [-q]\n");
  fprintf(stderr, "   [-s <selectfile>] [-S] ([-t <HOSTNAME>] || FILESPEC)\n\n");
  fprintf(stderr, "Where:\n");
  fprintf(stderr, "   -a                   Analyze data record frames\n");
  fprintf(stderr, "   -b <recordsize>      SEED record length, in bytes.  Defaults to 512 if -t <HOSTNAME> used.\n");
  fprintf(stderr, "                        If omitted, record lengths are automatically detected via Blockette 1000.\n");
  fprintf(stderr, "                        This version can handle files with mixed record lengths of 256, 512 and 4096.\n");
  fprintf(stderr, "   -c <channel_spec>    Channel specification, wildcards are supported (e.g. -c BH? or -c *Z)\n");
  fprintf(stderr, "   -d                   Decode Records\n");
  fprintf(stderr, "   -D                   Calculate delay of first and last sample in each record\n");
  fprintf(stderr, "   -e                   Export as BDF (ASCII) Files\n");
  fprintf(stderr, "   -f <comp_format>     Compression format, default is STEIM1.  Possible values are:\n");
  fprintf(stderr, "                         ASRO, SRO, RSTN, CDSN, DWWSSN, 16-BIT, 32-BIT, HARV, SEED, SEEDS1,\n");
  fprintf(stderr, "                         STEIM, STEIM1, STEIM2, SEEDS2, SEEDS3, STEIM3, LOG, USNSN\n");
  fprintf(stderr, "   -g                   List gaps (only) in data\n");
  fprintf(stderr, "   -i                   Ignore (do not process) blockettes present in data records\n");
  fprintf(stderr, "   -l <loc_code_spec>   Location Code specification, wildcards are supported (e.g. -l 00 or -l 1?)\n");
  fprintf(stderr, "   -p <port>            Port number.  Use with -t to specify connection port, default is port 4000\n");
  fprintf(stderr, "   -P <pidfile>         Write a watchdog file named /tmp/pidfile.tmp each time a record is processed\n");
  fprintf(stderr, "   -q                   Quiet mode skips printing of non-critical information (e.g. which file is being processed)\n");
  fprintf(stderr, "   -s <selectfile>      Name of select file (see manpage for selectfile)\n");
  fprintf(stderr, "   -S                   Enable Silent mode\n");
  fprintf(stderr, "   -t <HOSTNAME>        IP address or URL of host from which data stream originates, port defaults to 4000)\n");
  fprintf(stderr, "   -E                   Print Endian-ness of host computer and exit\n");
  fprintf(stderr, "   FILESPEC             Specification of input file(s), wildcards are supported (e.g. 00_B??.seed or *.seed)\n");
 
  fprintf(stderr, "Notes:  -t <HOSTNAME> and FILESPEC are mutually exclusive and must appear as last argument\n");
  fprintf(stderr, "        -c <channel_spec> and -l <loc_code_spec> are mutually exclusive\n\n");
  fprintf(stderr, "Examples:\n");
  fprintf(stderr, "   dumpseed -p 4001 -t anmo.iu.liss.org\n");
  fprintf(stderr, "     Input:  port 4001 of a host named anmo.iu.liss.org\n");
  fprintf(stderr, "     Output: stdout\n");
  fprintf(stderr, "     Notes:  when -t <HOSTNAME> is used, record size defaults to 512 bytes\n\n"); 

  fprintf(stderr, "   dumpseed -d -e *.seed > output.dat\n");
  fprintf(stderr, "     Input:  all files in the current directory ending with '.seed'\n");
  fprintf(stderr, "     Output: a file in the current directory named output.dat\n");
  fprintf(stderr, "     Notes:  data is decoded and exported in BDF format, input record size defaults to 4096 bytes\n\n");

  fprintf(stderr, "   dumpseed -i -e -D -c BHZ -t anmo.iu.liss.org > 00_BHZ.delay\n");
  fprintf(stderr, "     Input:  port 4000 of a host named anmo.iu.liss.org\n");
  fprintf(stderr, "     Output: a file named 00_BHZ.delay\n");
  fprintf(stderr, "     Notes:  time delay for each record is exported in BDF format\n\n");

  fprintf(stderr, "   dumpseed -d -e -c BH? IU_ANMO.seed > IU_ANMO_BH.dat\n");
  fprintf(stderr, "     Input:  a file named IU_ANMO.seed\n");
  fprintf(stderr, "     Output: a file named IU_ANMO_BH.dat\n");
  fprintf(stderr, "     Notes:  data for channels BH? is decoded and exported in BDF format\n\n");  

  fprintf(stderr, "   dumpseed -l 10 -p 4004 -t anmo.iu.liss.org\n");
  fprintf(stderr, "     Input:  port 4004 of a host named anmo.iu.liss.org\n");
  fprintf(stderr, "     Output: stdout\n");
  fprintf(stderr, "     Notes:  Only records with a location code of 10 are displayed\n\n");    
  exit(2);
}


char *ifil[MAX_FILES];
int num_files=0;

int main(int argc,char **argv) {

  int c;
  extern char *optarg;
  extern int optind;
  int errflg=0;
  int i;

  /* GTM Oct. 2, 2008 - determine host endian */
  if (HOST_ORDER == ENDIAN_BIG) {
    host_endian = ENDIAN_BIG;
  } else if (HOST_ORDER == ENDIAN_LITTLE) {
    host_endian = ENDIAN_LITTLE;
  } else {
    fprintf(stdout, "Unknown Host Order - cannot proceed\n");
    exit(1);
  }    

  record_length = 0;
  bufsize = 0;
  strcpy(defaultformat,"STEIM1");
  strcpy(chanpick,"*");

  SelectInit(&dumproot);

  while ((c = getopt(argc, argv, "ab:c:dDef:gil::p:P:qs:StE")) != EOF)
    switch (c) {
    case 'a':
      analyze++;
      break;
    case 'b':
      record_length = atoi(optarg);
      bufsize = record_length;
      fixed_recsize = TRUE;
      break;
    case 'c':
      strcpy(chanpick, optarg);
      Upcase(chanpick);
      chanpickp=1;
      break;
    case 'd':
      decode++;
      break;
    case 'D':
	caldelay++;
	break;
    case 'e':
      export++;
      break;
    case 'f':
      strcpy(defaultformat, optarg);
      Upcase(defaultformat);
      break;
    case 'g':
      gaplist++;
      break;
    case 'i':
      ignore_blockettes ++;
      break;
    case 'l':
      strcpy(locpick,optarg);
      Upcase(locpick);
      locpickp=1;
      break;
    case 'p':
      tel_port = atoi(optarg);
      break;
    case 'P':
      pidfile = strdup(optarg);
      break;
    case 'q':
      quiet++;
      break;
    case 's':
      select_file = strdup(optarg);
      break;
    case 'S':
      silent++;
      break;
    case 't':
      telemetry ++;
      break;
    case 'E':
      if (host_endian == ENDIAN_BIG) {
        fprintf(stdout, "Host is Big Endian\n");
      } else {
        fprintf(stdout, "Host is Little Endian\n");
      }
      exit(0);
    case '?':
      errflg++;
    }

  if (errflg) {
    usage();
  }

  if (telemetry) {
    if (record_length == 0) {
      record_length = 512;
      bufsize = 512;
    }
  }
  
  if (record_length == 0) {
    record_length = 4096;
    bufsize = 4096;
  }

  defaultformnum = 0;

  if (streq(defaultformat,"ASRO")) defaultformnum = 30;
  if (streq(defaultformat,"SRO")) defaultformnum = 30;

  if (streq(defaultformat,"RSTN")) defaultformnum = 33;
  if (streq(defaultformat,"CDSN")) defaultformnum = 33;

  if (streq(defaultformat,"DWWSSN")) defaultformnum = 1;
  if (streq(defaultformat,"16-BIT")) defaultformnum = 1;

  if (streq(defaultformat,"32-BIT")) defaultformnum = 3;

  if (streq(defaultformat,"HARV")) defaultformnum = 10;
  if (streq(defaultformat,"SEED")) defaultformnum = 10;
  if (streq(defaultformat,"SEEDS1")) defaultformnum = 10;
  if (streq(defaultformat,"STEIM")) defaultformnum = 10;
  if (streq(defaultformat,"STEIM1")) defaultformnum = 10;

  if (streq(defaultformat,"STEIM2")) defaultformnum = 11;
  if (streq(defaultformat,"SEEDS2")) defaultformnum = 11;

  if (streq(defaultformat,"USNSN")) defaultformnum = 15;

  if (streq(defaultformat,"SEEDS3")) defaultformnum = 20;
  if (streq(defaultformat,"STEIM3")) defaultformnum = 20;

  if (streq(defaultformat,"LOG")) defaultformnum = 50;

  if (!telemetry && pidfile)
    printf("Pidfile not recommended without telemetry\n");

  if (select_file && (chanpickp || locpickp)) 
    bombout(EXIT_ABORT,"Select file not compatible with channel or location select");

  if (select_file) {
    SelectLoadup(&dumproot, select_file);
    if (!quiet) 
      SelectList(&dumproot);
  }

  if (pidfile)
    watchdog(pidfile);

  for (; optind < argc; optind++) {
    ifil[num_files++] = strdup(argv[optind]);
    if (num_files+1>MAX_FILES) 
      bombout(EXIT_ABORT,"Too many files - only supports %d",MAX_FILES);
  }
  
  if (num_files<=0) usage();

  if (!telemetry) {

    for (i=0; i<num_files; i++) {

      int dash=0;

      if (strcmp(ifil[i], "-")==0) dash=1;

      recnum = -1;

      if (dash) 
	seediov = stdin;
      else {
	seediov = fopen(ifil[i],"r");
	if (seediov==NULL) {
	  fprintf(stderr,"Couldn't open seed file %s: %s\n",
		  ifil[i],
		  strerror(errno));
	  continue;
	}
      }

      if (!quiet) {
	if (!dash) 
	  fprintf(stderr,"[Process seed file %s]\n",ifil[i]);
	else
	  fprintf(stderr,"[Process standard input]\n");
      }

      while (get_nexrec());

      if (!dash)
	fclose(seediov);

      if (gaplist)
	flushgaps();

    }

    exit(EXIT_NORMAL);
  }

  if (num_files > 1) {
    bombout(EXIT_ABORT,"Can't open more than one host");
  }

  process_telemetry(ifil[0], tel_port, record_length);

  exit(EXIT_NORMAL);

}	
