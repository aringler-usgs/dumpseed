#include <dcc_std.h>

#ifdef HAVE_SYS_MTIO_H
# include <sys/mtio.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include <errno.h>

#include <tapeinfo.h>


ULONG MT_stat=0;

_SUB int mag_openr(char *name)
{
  int newf;

  newf = open(name,O_RDONLY);
  if (newf<0) {
    perror("Could not open tape device");
    return(newf);
  }

  return(newf);

}

_SUB int mag_openw(char *name)
{
  int newf;

  newf = open(name,O_RDWR);
  if (newf<0) {
    perror("Could not open tape device");
    return(newf);
  }

  return(newf);

}

_SUB int mag_close(int chan)
{

  return(close(chan));

}

_SUB int mag_read(int chan,char *buffer,unsigned maxm,int *rlen)
{

  int rdsts;

  rdsts = read(chan,buffer,maxm);

  *rlen = rdsts;
  if (rdsts>=0) return(rdsts);

  *rlen = 0;
  return(rdsts);

}

_SUB int mag_readsts(int chan,char *buffer,unsigned maxm,	int *rlen,
		     unsigned *rstat,unsigned *rerr)
{

  int rdsts;

  *rerr = 0;
  *rstat = 0;

  rdsts = read(chan,buffer,maxm);

  *rlen = rdsts;
  if (rdsts>=0) return(rdsts);

  *rerr = errno;
  *rlen = 0;
  return(rdsts);

}

_SUB int mag_rd(int chan,char *buff,unsigned maxm)
{
  int rbf;

  return(mag_read(chan,buff,maxm,&rbf));
}

_SUB int mag_write(int chan,char *buffer,unsigned maxm)
{

  return(write(chan,buffer,maxm));

}

_SUB int mag_write_eof(int chan)
{
  struct mtop mtcmd;

  mtcmd.mt_op = MTWEOF;
  mtcmd.mt_count = 1;
  ioctl(chan, MTIOCTOP, &mtcmd);
  return(0);
}

_SUB int mag_eof(int chan) 
{ 
  return(mag_write_eof(chan)); 
}

_SUB int mag_write_eoi(int chan)
{

  int a,b;

  for (a=0; a<6; a++) {
    b = mag_write_eof(chan);
  }

  return(b);
}	

_SUB void mag_flap(int chan)
{

  struct mtop mtcmd;
  mtcmd.mt_op = MTOFFL;
  mtcmd.mt_count = 1;
  ioctl(chan, MTIOCTOP, &mtcmd);

}

_SUB void mag_rewind(int chan)
{

  struct mtop mtcmd;
  mtcmd.mt_op = MTREW;
  mtcmd.mt_count = 1;
  ioctl(chan, MTIOCTOP, &mtcmd);

}

_SUB void mag_rew(int chan) 
{ 
  mag_rewind(chan); 
}

_SUB void mag_skipf(int chan,int files)
{

  struct mtop mtcmd;
  if (files>0) {
    mtcmd.mt_op = MTFSF;
    mtcmd.mt_count = files;
  } else {
    mtcmd.mt_op = MTBSF;
    mtcmd.mt_count = -files;
  }

  ioctl(chan, MTIOCTOP, &mtcmd);

}

_SUB void mag_skipr(int chan,int files)
{
  struct mtop mtcmd;
  if (files>0) {
    mtcmd.mt_op = MTFSR;
    mtcmd.mt_count = files;
  } else {
    mtcmd.mt_op = MTBSR;
    mtcmd.mt_count = -files;
  }

  ioctl(chan, MTIOCTOP, &mtcmd);

}

LOCAL char _magerr[60];

_SUB char *mag_error(int errn)
{

  if (errn<0) {
    sprintf(_magerr,"ERROR: Unknown error code %%x%x",errn);
    return(_magerr);
  }

  return(strerror(errn));

}
