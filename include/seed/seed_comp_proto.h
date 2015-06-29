/* Processing file seed_comp.c */

 ULONG _SD_SetCtlType(int pos,ULONG val,ULONG *outctl)
;
 VOID _SD_ClearContext(SD_Context *context)
;
 SD_Context *SD_CreateChannel(UBYTE *network,
			     UBYTE *station,
			     UBYTE *location,
			     UBYTE *channel,
			     int subchan,
			     VOID (*flushroutine)(VOID *),
			     int format,
			     int len,
			     LONG ms_sam)
;
 VOID SD_DestroyChannel(SD_Context *context)
;
 VOID SD_PutSample(SD_Context *context, LONG delta, LONG sample, BOOL flush)
;
 VOID SD_FlushRecord(SD_Context *context)
;
 VOID SD_PutData(SD_Context *context, STDTIME datetime, int numsamps,
	LONG *dataarray)
;

/* Found 7 functions */

