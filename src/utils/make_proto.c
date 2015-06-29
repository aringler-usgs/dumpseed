#include <dcc_std.h>

void dosub(char *arg)
{
  FILE *fop;
  int cnt=0,prc=0;
  char c;

  fprintf(stderr,"Processing %s - ",arg);

  fop = fopen(arg,"r");
  if (fop==NULL) {
    fprintf(stderr,"Cannot open\n");
    exit(10);
  }

  printf("/* Processing file %s */\n\n",arg);

  while(!feof(fop)) {
    c = getc(fop);
    if (c==EOF) break;
    switch (prc) {
    case 0:
      if (c=='_') prc++;
      else prc = 0;
      break;
    case 1:
      if (c=='S') prc++;
      else prc = 0;
      break;
    case 2:
      if (c=='U') prc++;
      else prc = 0;
      break;
    case 3:
      if (c=='B') prc=10;
      else prc = 0;
      break;
    case 10:
      if (c=='{') {
	printf(";\n");
	prc=0;
	cnt++;
      }
      else 
	putchar(c);
      break;
    }
    
  }
  fclose(fop);
  
  printf("\n/* Found %d functions */\n\n",cnt);
  
  fprintf(stderr,"Found %d functions\n",cnt);
}

int main(int argc,char **argv)
{

  int i;

  for (i=1; i<argc; i++) dosub(argv[i]);

  exit(0);

}
