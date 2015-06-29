#include <dcc_std.h>

#if VAXC

FILE *fix_create(filename,rsize,alloc,extend)
UBYTE *filename;
int rsize,alloc,extend;
{

	UBYTE mrs[20],deq[20],alq[20];
	FILE *newfile;

	sprintf(mrs,"mrs=%d",rsize);
	sprintf(deq,"deq=%d",extend);
	sprintf(alq,"alq=%d",alloc);

	newfile = fopen(filename,"wb+",
		"rfm=fix","fop=tef,cbt","rop=wbh",
		mrs,deq,alq);

	return(newfile);
}

FILE *fix_open(filename)
UBYTE *filename;
{

	FILE *newfile;

	newfile = fopen(filename,"rb",
		"rfm=fix","ctx=rec","shr=get","rop=wbh");

	return(newfile);
}

#else

_SUB FILE *fix_create(char *filename,int rsize,int alloc,int extend)
{
	FILE *newfile;

	newfile = fopen(filename,"w+");

	if (newfile)
	  setvbuf(newfile, NULL, _IONBF, 0);	/* Make file unbuffered */

	return(newfile);
}

_SUB FILE *fix_open(char *filename)
{

	FILE *newfile;

	newfile = fopen(filename,"r");

	if (newfile)
	  setvbuf(newfile, NULL, _IONBF, 0);	/* Make file unbuffered */

	return(newfile);
}

#endif

/* Replacement for fread (which is really slow on sun) */

_SUB int fix_readr(void *ptr, int size, int nitems, FILE *stream)
{

  int rt;

  rt = read(fileno(stream), ptr, nitems * size);
  if (rt<=0) return(rt);

  return(rt/size);

}

