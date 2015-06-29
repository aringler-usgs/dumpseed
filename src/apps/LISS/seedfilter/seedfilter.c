#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>
#include <seed_data_proto.h>
#include <dcc_misc.h>
#include <dcc_seed_select.h>

#include "seedfilter_proto.h"

time_t now;

int extendedoutput=0;
int verbose=0;
int ignoreeof=0;
int blocksize=0;
char *select_file=NULL;
char *retain="D";
char *remrec=NULL;

int invert=0;      /* 3-Jun-98/SH - Add invert */

struct sel3_root dumproot;

#define MAX_FILES 10
char *ifil[MAX_FILES];
int num_files=0;

int seed_size = 512;

unsigned char buf[8192];
int bufpop=0,jettison=0;

void writerec(unsigned char *inseed)
{

  int i;
  char rec_type;
  ITEMLIST *taglist=NULL;

  for (i=0; i<20; i++) {

    buf[i] = toupper(buf[i]);

    if (!isprint(buf[i]))
      buf[i] = ' ';
  }

  rec_type = buf[6];

  if (retain)
    if (!strchr(retain, rec_type)) return;

  if (remrec)
    if (strchr(remrec, rec_type)) return;

  if (select_file) {
    int interesting;
    char n[10],s[10],l[10],c[10];

    SH_Get_Idents((SEED_DATA *) inseed,n,s,l,c);
    TrimString(n);
    Upcase(n);
    TrimString(s);
    Upcase(s);
    TrimString(l);
    Upcase(l);
    TrimString(c);
    Upcase(c);

    interesting = SelectDateInteresting(&dumproot,
					SH_Start_Time((SEED_DATA *) inseed),
					SH_End_Time((SEED_DATA *) inseed),
					n,s,l,c,
					&taglist);

    if (invert) interesting = !interesting;  /* 3-Jun-98/SH - Add invert */

    if (!interesting) return;

  }

  if (!extendedoutput) {
    i = write(fileno(stdout), inseed, seed_size);
    if (i!=seed_size) {
      fprintf(stderr,"Cannot write to output\n");
      exit(30);
    }
  }

}

_SUB int procrec()
{

  int count;

  bufpop = 0;

  for (;;) {

    /*
     * Read from the network
     */
    count = read(fileno(stdin), &buf[bufpop], 256);
    if(count < 0) {
      /* Unable to read from the terminal */
      fprintf(stderr,"Read error from server - %s\n",
	      strerror(errno));
      return(3);
    }
    if (count==0) {
      if (verbose)
	fprintf(stderr,"[End of file]\n");
      break;
    }
	
    bufpop += count;

    while (bufpop>=seed_size) {

      writerec((char *) buf);
      bufpop -= seed_size;
      if (bufpop>0)
	memcpy(buf,&buf[seed_size],bufpop);
    }


    /*
     * Continue the loop
     */
  }                  

  return(0);
}


void usage(int argc,char **argv)
{

  fprintf(stderr,"Usage: %s -x -v -b seedrecsize -s selectfile -t types\n",
	 argv[0]);
  fprintf(stderr,"\n");
  fprintf(stderr,"    -x         output in extended (LISS2) mode\n");
  fprintf(stderr,"    -v         emit diagnostic messages\n");
  fprintf(stderr,"    -i         invert sense of select file\n");
  fprintf(stderr,"    -b recsiz  seed record size - defaults to 512 - ignored in extended mode\n");
  fprintf(stderr,"    -s file    use select file for filtering\n");
  fprintf(stderr,"    -t types   list of seed record types to keep (defaults to 'D'\n");
  fprintf(stderr,"    -r types   list of seed record types to remove (overrides -t)\n\n");

  exit(20);
}

int main(int argc,char **argv)
{

  int c;
  extern char *optarg;
  extern int optind;
  int errflg=0;
  int i;

  while ((c = getopt(argc, argv, "xvb:s:t:r:")) != EOF)
    switch (c) {
    case 'x':
      extendedoutput++;
      break;
    case 'v':
      verbose++;
      break;
    case 'b':
      i = atoi(optarg);
      switch(i) {
      case 256:
      case 512:
      case 4096:
	seed_size = i;
	break;
      default:
	fprintf(stderr,"Block size must be 256, 512, or 4096\n");
	errflg++;
	break;
      }
      break;
    case 's':
      select_file = strdup(optarg);
      break;
    case 't':
      retain = strdup(optarg);
      Upcase(retain);
      break;
    case 'r':
      remrec = strdup(optarg);
      Upcase(remrec);
      break;
    case 'i':
      invert++;
      break;
    case '?':
      errflg++;
    }

  if (extendedoutput) {
    printf("Extended (LISS2) mode not yet supported\n");
    errflg++;
  }

  if (remrec) 
    retain = NULL;

  if (errflg) 
    usage(argc,argv);

  SelectInit(&dumproot);

  if (select_file) {
    SelectLoadup(&dumproot, select_file);
    if (verbose) 
      SelectList(&dumproot);
  }

  for (; optind < argc; optind++) {
    ifil[num_files++] = strdup(argv[optind]);
    if (num_files+1>MAX_FILES) 
      bombout(EXIT_ABORT,"Too many files - only supports %d",MAX_FILES);
  }
  
  if (num_files>0) usage(argc,argv);

  i = procrec();

  exit(0);

}
