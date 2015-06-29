#ifndef DEF_STATIONIDX
#define DEF_STATIONIDX

#ifndef TMLB_DEFINED
#include <dcc_time.h>
#endif

struct	station_index	{

	char	SI_station[5];		/* Station Code (SF) */
	char	SI_location[2];		/* Location Code (SF) */
	char	SI_channel[3];		/* Channel Code (SF) */

	UBYTE	SI_format_subclass;	/* Subcode for data format */

	STDTIME	SI_record_start_time;	/* Record starting time */
	STDTIME	SI_record_end_time;	/* Record ending time */

	ULONG	SI_sample_rate;		/* Sample rate * 10000 */
	UWORD	SI_number_samples;	/* Number of samples this record */

	UBYTE	SI_number_channels;	/* The number of channels 1-16 */

	DSKREC	SI_seed_blockette;	/* Pointer into blockette file */

	WORD	SI_time_correction;	/* delta change in milliseconds +/- */

	ULONG	SI_status_flags;	/* Flags for this record */

	DSKREC	SI_station_record;	/* Pointer to station record */

};

#define CORR_TIME  0x0001		/* Time is corrected */
#define CORR_DEL   0x0002		/* Record is deleted */
#define CORR_OUTS  0x0004		/* Record is out of sequence */
#define CORR_EMPTY 0x0008		/* Record contains empty space */
#define CORR_HARDE 0x0010		/* Hard parity error */
#define CORR_SHORT 0x0020		/* Short record (padded) */
#define CORR_LONG  0x0040		/* Long record (truncated) */
#define CORR_BFOR  0x0080		/* Header contains bad bits */
#define CORR_BTIM  0x0100		/* Bad time fields in header */
#define CORR_BEVEN 0x0200		/* Beginning of an event here */
#define CORR_EEVEN 0x0400		/* End of an event here */
#define CORR_CALI  0x0800		/* Calibration present */
#define CORR_NOACC 0x1000		/* No time accounting - log channel */
#define CORR_TEAR  0x2000		/* Time Correction is for a tear */

#endif
