#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>
#include <seed_data_proto.h>
#include <dcc_misc.h>
#include <dcc_seed_select.h>

#include "seedport_proto.h"

/*
 * client program to catch data from dpnetport
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>

time_t now;

int extendedoutput=0;
int verbose=0;
int liss2=0;
int ignoreeof=0;
int blocksize=0;
int tel_port=4000;
char *select_file=NULL;
char *pidfile=NULL;
char *capture=NULL;
int flushint=3600;
int startint=1;

struct sel3_root dumproot;

#define MAX_FILES 10
char *ifil[MAX_FILES];
int num_files=0;

int seed_size = 512;  /* OK default for now, since we're just
			 sending all data to stdout -- at this
			 point we don't actually care how big it
			 is - later, with extendedoutput format, we will */

unsigned char buf[8192];
int bufpop=0,jettison=0;

char outf_name[500];
char outf_dest[500];
FILE *outf=NULL;

void process_record(char *inseed)
{

  int i;
  time_t curr;
  FILE *outputfile;
  ITEMLIST *taglist=NULL;

  outputfile = stdout;

  if (pidfile)
    watchdog(pidfile);

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

    if (!interesting) return;

  }

  /* Are we capturing? */

  if (capture) {

    /* See if flush interval is over - if so, we start new file */

    curr = time(NULL);
    if (curr>now+flushint) {

      /* Close old file and rename it - if any */

      if (outf) {
	fclose(outf);
	
	/* mv temp name to permanent name */

	unlink(outf_dest);
	i = link(outf_name, outf_dest);
	if (i!=0) 
	  fprintf(stderr,"Couldn't link new file %s - %s\n",
		  outf_dest, strerror(errno));
	else
	  unlink(outf_name);

	outf = NULL;
      }

      /* Open new file */

      sprintf(outf_dest, capture, startint++);
      strcpy(outf_name, outf_dest);
      strcat(outf_name, "_live");

      outf = fopen(outf_name,"w");
      if (outf==NULL) {
	printf("Couldn't write file %s\n",outf_name);
	exit(20);
      }
      if (verbose) 
	fprintf(stderr,"[Open capture file %s]\n",
		outf_dest);

    now = curr; /* When did we open the file? */

    }

    outputfile = outf;

  }

  if (!extendedoutput) {
    i = write(fileno(outputfile), inseed, seed_size);
    if (i!=seed_size) {
      fprintf(stderr,"Cannot write to output\n");
      exit(30);
    }
    fflush(outputfile);
    return;
  }

}

_SUB int process_telemetry(char *host, int port)
{
  struct sockaddr_in sin;
  struct hostent *hp, *gethostbyname();
  int fd, count, optval, i;

  if (verbose)
    fprintf(stderr,"[Connecting to remote host: %s (port %d) blocksize %d]\n",
	    host,port,seed_size);

  /*
   * Translate the hostname into an internet address
   */
  hp = gethostbyname(host);
  if(hp == 0) {
    fprintf(stderr, "%s: unknown host\n",host);
    exit(1);
  }
  /*
   * We now create a socket which will be used for our connection.
   */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd < 0) {
    fprintf(stderr,"Couldn't open socket on client socket - %s\n",
	    strerror(errno));
    return(1);
  }

  /*
   *  Before we attempt a connect, we fill up the "sockaddr_in" structure
   */
  memcpy((char *)&sin.sin_addr, hp->h_addr , hp->h_length);
  sin.sin_family      = AF_INET;
  sin.sin_port        = htons(port);
  /*
   *  Now attempt to connect to the remote host .....
   */
  if(connect(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    fprintf(stderr,"Couldn't connect to server %s - %s\n",host,
	    strerror(errno));
    return(2);
  }

  if (verbose)
    fprintf(stderr,"Connected to remote host %s port %d (%s)\n",
	    host,port,hp->h_name);
  fflush(stderr);

  /* Turn on keepalive on socket so we won't hang if it gets stuck */

  optval = 1;
  i = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(int));
  
  if (i==(-1)) 
    fprintf(stderr,"Couldn't set keepalive on client socket - ignored - %s\n",
	    strerror(errno));

  bufpop = 0;

  for (;;) {

    /*
     * Read from the network
     */
    count = recv(fd, &buf[bufpop], 256, 0);
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

      process_record((char *) buf);
      bufpop -= seed_size;
      if (bufpop>0)
	memcpy(buf,&buf[seed_size],bufpop);
    }

    /*
     * Continue the loop
     */
  }                  

  close(fd);

  return(0);
}

void usage(int argc,char **argv)
{

  fprintf(stderr,"Usage: %s -x -v -b seedrecsize -s selectfile -L -p x -P pidfile -c mask -f secs -i x host\n",
	 argv[0]);
  fprintf(stderr,"\n");
  fprintf(stderr,"    -x         output in extended (LISS2) mode\n");
  fprintf(stderr,"    -v         emit diagnostic messages\n");
  fprintf(stderr,"    -b recsiz  seed record size - defaults to 512 - ignored in extended mode\n");
  fprintf(stderr,"    -s file    filter selections - sends to LISS if enabled\n");
  fprintf(stderr,"    -p port    port to connect to - defaults to 4000\n");
  fprintf(stderr,"    -E         ignore EOF - keep reconnecting to port\n");
  fprintf(stderr,"    -c mask    capture to a series of files named by mask (out%%d)\n");
  fprintf(stderr,"    -f secs    flush interval of capture in seconds (defaults to 3600)\n");
  fprintf(stderr,"    -i init    Initial capture file # (normally 1)\n");

  fprintf(stderr,"    -P file    write a PIDfile\n\n");

  exit(20);
}

int main(int argc,char **argv)
{

  int c;
  extern char *optarg;
  extern int optind;
  int errflg=0;
  int i;
  char test1[500],test2[500];

  while ((c = getopt(argc, argv, "xvLEb:s:p:c:f:i:P:")) != EOF)
    switch (c) {
    case 'x':
      extendedoutput++;
      break;
    case 'v':
      verbose++;
      break;
    case 'L':
      liss2++;
      break;
    case 'E':
      ignoreeof++;
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
    case 'p':
      tel_port = atoi(optarg);
      break;
    case 's':
      select_file = strdup(optarg);
      break;
    case 'P':
      pidfile = strdup(optarg);
      break;
    case 'c':
      capture = strdup(optarg);
      break;
    case 'f':
      flushint = atoi(optarg);
      if (flushint<=5) {
	fprintf(stderr,"Flush interval must be >= 5 seconds\n");
	errflg++;
      }
      break;
    case 'i':
      startint = atoi(optarg);
      if (startint<0) {
	fprintf(stderr,"Starting sequence must be >= 0\n");
	errflg++;
      }
      break;
    case '?':
      errflg++;
    }

  if (capture) {
    sprintf(test1,capture,1);
    sprintf(test2,capture,9999);

    if (strcmp(test1,test2)==0) {
      fprintf(stderr,"You must specify capture masks with the %%d operator - like out%%d.seed\n");
      errflg++;
    }
  }

  if (liss2) {
    printf("LISS2 server connections not supported yet\n");
    errflg++;
  }

  if (extendedoutput) {
    printf("Extended (LISS2) mode not yet supported\n");
    errflg++;
  }

  if (errflg) 
    usage(argc,argv);

  if (pidfile)
    watchdog(pidfile);

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
  
  if (num_files<=0) usage(argc,argv);
  if (num_files>1) usage(argc,argv);

  now = 0;

  while(1) {
    i = process_telemetry(ifil[0], tel_port);
    if (!ignoreeof) break;
    sleep(20);
  }

  exit(0);

}
