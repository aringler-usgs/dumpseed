/*
 * client program to catch data from dpnetport
 */

#include <dcc_std.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#include <dcc_time.h>
#include <seed_data.h>

#include "dumpctx.h"

#include "dumpseed_proto.h"

int seed_size;

unsigned char buf[8192];
int bufpop=0,jettison=0;

void process_record(char *inseed);

_SUB void process_telemetry(char *host, int port, int bufsize)
{
  struct sockaddr_in sin;
  struct hostent *hp, *gethostbyname();
  int fd, count;

  seed_size = bufsize;

  printf("[Connecting to remote host: %s (port %d) blocksize %d]\n",
	 host,port,bufsize);

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
    perror("socket");
    exit(1);
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
    perror("connect");
    exit(1);
  }

  fprintf(stderr,"Connected to remote host %s port %d\n",host,port);
  fflush(stderr);

  bufpop = 0;

  printf("Connected to %s\n",hp->h_name);
  for (;;) {

    /*
     * Read from the network
     */
    count = recv(fd, &buf[bufpop], 256, 0);
    if(count < 0) {
      /* Unable to read from the terminal */
      perror("network read");
      exit(1);
    }
    if (count==0) {
      printf("End of file\n");
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

  return;
}





