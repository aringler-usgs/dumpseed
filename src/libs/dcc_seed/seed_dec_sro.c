#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_seed.h>
#include <seed_data.h>
#include <dcc_misc.h>

_SUB int SH_Decode_SRO(long data[], SEED_DATA *inrec,int swdata)
{

  register int lp,ct,ds;
  register LONG l_data,*oarray;
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

  for (lp=0; lp<(numsam*2); lp+=2) {

    register int j, gr;
    
    j  = (bytarr[ds+lp] & 0x0F) << 8;
    j |= (bytarr[ds+lp+1] & 0xFF);
    
    if (j >= 0x800) j -= 4096;
    
    gr = (bytarr[ds+lp] & 0xF0) >> 4;
    
    l_data = (LONG) j;
    l_data <<= (10-gr);
    
    *oarray++ = l_data;
    ct++;
  }

  return(ct);

}

