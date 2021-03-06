#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_seed.h>
#include <seed_data.h>
#include <dcc_misc.h>

_SUB int SH_Decode_ASCII(long data[], SEED_DATA *inrec,int swdata)
{

  register int lp,ct,ds;
  register LONG *oarray;
  register UBYTE *bytarr;
  register int numsam;

  numsam = SH_Number_Samples(inrec);
  
  bytarr= (UBYTE *) inrec;
  ds = SH_Start_Data(inrec);
  numsam = SH_Number_Samples(inrec);

  if (ds==0) return(-1);
  if (numsam==0) return(0);

  oarray=(LONG *) data;
  ct=0;
  
  for (lp=0; lp<numsam; lp++) {

    *oarray++ = bytarr[ds+lp];
    
    ct++;
  }
  return(ct);
}



