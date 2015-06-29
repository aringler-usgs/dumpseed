/* Processing file seed_data.c */

 void SH_Big_Endian()
;
 void SH_Little_Endian()
;
 WORD SH_SWAP_WORD(WORD i)
;
 LONG SH_SWAP_LONG(LONG i)
;
 WORD SH_SWAP_DATA_WORD(WORD i)
;
 LONG SH_SWAP_DATA_LONG(LONG i)
;
 STDFLT SH_Cvt_Float(NETFLT inflt)
;
 STDTIME SH_Cvt_Time(NETTIME intim)
;
 STDTIME SH_Start_Time(SEED_DATA *pblk)
;
 int SH_Number_Samples(SEED_DATA *pblk)
;
 int SH_Start_Data(SEED_DATA *pblk)
;
 STDFLT SH_Record_Rate(SEED_DATA *pblk)
;
 LONG SH_Record_Time(SEED_DATA *pblk)
;
 LONG SH_Sample_Time(SEED_DATA *pblk)
;
 STDFLT SH_Sample_FltTime(SEED_DATA *pblk)
;
 STDTIME SH_Cal_Samp_Time(SEED_DATA *inrec, int samn)
;
 STDTIME SH_End_Time(SEED_DATA *pblk)
;
 LONG SH_Record_Number(SEED_DATA *pblk)
;
 VOID SH_Get_Idents(SEED_DATA *pblk,
			char *net, char *stat, 
			char *loc, char *chan)
;
 VOID SH_Get_Spankey(SEED_DATA *pblk, char *retspan)
;
 VOID SH_Guess_Endian(SEED_DATA *pblk)
;

/* Found 21 functions */

/* Processing file seed_data_dec.c */

 int SH_Data_Decode(char *format, long data[], SEED_DATA *inrec,int swdata)
;
 WORD SH_DEC_SWAP_WORD(WORD i,int swdata)
;
 LONG SH_DEC_SWAP_LONG(LONG i,int swdata)
;

/* Found 3 functions */

/* Processing file seed_dec_16.c */

 int SH_Decode_16(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

/* Processing file seed_dec_32.c */

 int SH_Decode_32(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

/* Processing file seed_dec_sro.c */

 int SH_Decode_SRO(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

/* Processing file seed_dec_cdsn.c */

 int SH_Decode_CDSN(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

/* Processing file seed_dec_seed.c */

 int SH_Decode_SEED(long data[], SEED_DATA *inrec,int swdata,int level)
;
 int SH_Decode_SEED_S2(long data[], SEED_DATA *inrec,int swdata)
;
 int SH_Decode_SEED_S3(long data[], SEED_DATA *inrec,int swdata)
;
 int SH_Decode_SEED_S1(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 4 functions */

/* Processing file seed_dec_usnsn.c */

 int SH_Decode_SEED_USNSN(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

/* Processing file seed_dec_ascii.c */

 int SH_Decode_ASCII(long data[], SEED_DATA *inrec,int swdata)
;

/* Found 1 functions */

