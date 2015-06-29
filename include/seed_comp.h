#include <seed_data.h>

struct _steim_frame {		
	ULONG	ctlflags;		/* Control headers */
	union long_element {
		struct { BYTE byteval[4]; } type_1;
		struct { WORD wordval[2]; } type_2;
		struct { LONG longval;    } type_3;
	} chunkstore[15];
};

#define NUMFRAM 64		/* 64*64, 4096 byte records */
struct steim_frames {
	struct _steim_frame framestore[NUMFRAM];
};

struct _sd_context {
	UBYTE	Network[3];
	UBYTE	Station[6];		/* Id stuff */
	UBYTE	Location[3];
	UBYTE	Channel[4];

	LONG	ms_sam;			/* Sample size in ms (0 for logs) */
	STDTIME	begtime;		/* Time of start this record */
	STDTIME	marktime;		/* Last time given (for logs) */
	int	samp_pop;		/* Current number samps */

	int	frmidx;			/* Ring buffer index */
	int	frmpop;                 /* Current ring buffer population */
	int	blocksz;		/* How big is the data rec */
	LONG	frmbuf[4];		/* Ring buffer of deltas */
	LONG	tstbuf[4];              /* Ring buffer of abs-deltas */
	int	frmwt[4];		/* Frame weight */
	LONG	sambuf[4];		/* Buffer for samples */
	int	frame,chkidx;		/* Current frame/chunk in record */
	int	sframe;			/* Starting frame number */
	LONG	movic;			/* Moving constant (rev ic) */
	LONG	psam;			/* Previous Sample - forward ic */

	UBYTE	databuf[4096];
	SEED_DATA *outrec;		/* Body of our seed record */
	struct	steim_frames *out_data;	/* Pointer to frames of prev */

	VOID	(*flushoutput)(VOID *datrec);	/* User dump routine */
};

typedef struct _sd_context SD_Context;

#define SD_FORMAT_STEIM 1 	/* Standard 68000 order format */
#define SD_FORMAT_STEIMV 2	/* VAX order format */
#define SD_FORMAT_WORD 3	/* 68000 2 byte format (DWWSSN) */
#define SD_FORMAT_LONG 4	/* 68000 4 byte format */
#define SD_FORMAT_CDSN 5	/* CDSN/RSTN gain ranged fmt */
#define SD_FORMAT_SRO 6		/* SRO gain ranged fmt */	

#include <seed/seed_comp_proto.h>
