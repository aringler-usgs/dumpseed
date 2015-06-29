/* Processing file sel_mem.c */

	VOID SelectInit(struct sel3_root *root)
;
 VOID SelectList(struct sel3_root *root)
;
	VOID SelectLoadup(struct sel3_root *root,char *infilename)
;
 BOOL SelectStatInteresting(struct sel3_root *root,
				char *network,char *station,
				char *location,char *channel,
				ITEMLIST **taglist)
;
 BOOL SelectChainInteresting(struct sel3_chain *root,
				 char *network,char *station,
				 char *location,char *channel,
				 ITEMLIST **taglist)
;
 BOOL SelectDateInteresting(struct sel3_root *root,
				STDTIME btime,STDTIME etime,
				char *network,char *station,
				char *location,char *channel,
				ITEMLIST **taglist)
;

/* Found 6 functions */

