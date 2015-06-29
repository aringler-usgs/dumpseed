/*
 *	
 *	Misc structures used in the data block
 *	
 */

typedef struct	net_time {
	UWORD	year;
	UWORD	day;
	UBYTE	hour;
	UBYTE	minute;
	UBYTE	seconds;
	UBYTE	dummy;
	UWORD	fracs;		/* Number of .0001 seconds */
} NETTIME;

typedef ULONG NETFLT;

/*
 *	
 *	Fixed portion of the data block
 *	
 */

typedef struct _fixed_block {
	UBYTE	Seq_ID[6];		/* Sequence number of record */
	UBYTE	Record_Type;		/* Always a 'D' */
	UBYTE	Filler;			/* Always a space */

	UBYTE	Station_ID[5];		/* Station id (space filled) */
	UBYTE	Location_ID[2];		/* Array/Extended Station (filled) */
	UBYTE	Channel_ID[3];		/* Channel Id (space filled) */
	UBYTE	Network_ID[2];		/* Extended Network Type */

	NETTIME	Start_Time;		/* Start time of record */
	UWORD	Number_Samps;		/* Number of samples in record */

	WORD	Rate_Factor;		/* Sample rate factor */
	WORD	Rate_Mult;		/* Rate Multiplier */

	UBYTE	Activity_Flags;		/* Activity Information */

#define ACTFLAG_CALSIG 0x01		/* Calibration signals this record */
#define ACTFLAG_CLKFIX 0x02		/* Error caused by clock correction */
#define ACTFLAG_BEGEVT 0x04		/* Beginning of event */
#define ACTFLAG_ENDEVT 0x08		/* End of event */
#define ACTFLAG_EVTPRG 0x40             /* Event in progress */

	UBYTE	IO_Flags;		/* I/O Information */

#define IOFLAG_ORGPAR 0x01		/* Original tape had parity error */
#define IOFLAG_LONGRC 0x02		/* Original read long record */
#define IOFLAG_SHORTR 0x04		/* Original had short record */
#define IOFLAG_GOODTM 0x20              /* Good external time mark */

	UBYTE	Qual_Flags;		/* Data Quality Information */

#define QULFLAG_AMPSAT 0x01		/* Amplifier saturation detected */
#define QULFLAG_SIGCLP 0x02		/* Signal clipping detected */
#define QULFLAG_SPIKES 0x04		/* Signal spiking detected */
#define QULFLAG_GLITCH 0x08		/* Signal glitch detected */
#define QULFLAG_PADDED 0x10		/* Data missing or padded */
#define QULFLAG_TLMSNC 0x20		/* Telemetry sync error/dropout */
#define QULFLAG_CHARGE 0x40		/* Digitial filter charging */
#define QULFLAG_TIMERR 0x80		/* Time tag questionable */

	UBYTE	Total_Blockettes;	/* Number blockettes to follow */
	LONG	Time_Correction;	/* Number of .0001 sec correction */
	UWORD	Data_Start;		/* Byte where data starts */
	UWORD	First_Blockette;	/* Byte of first blockette */
} SEED_DATA;

/* Generic blockette header */

struct gen_blk_hdr {
  UWORD Blockette_Type;
  UWORD Next_Begin;
};

/*
 *
 *     Sample Rate Blockette
 * 
 */

#define BLK_SAMRATE 100
struct samp_rate {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETFLT  Samp_Rate;              /* Actual sample rate */

	UBYTE   Flags;                  /* Flags - reserved */
	UBYTE   Resrv1;
};
#define SAMRATE_SZ (sizeof (struct samp_rate))


/*
 *	
 *	Generic Event Detection Blockette [200]
 *	
 */

#define BLK_EVTDET 200
struct event_detect {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETFLT	Signal_Amplitude;	/* Event signal amplitude */
	NETFLT	Signal_Period;		/* Period of signal */
	NETFLT	Background_Estimate;	/* Estimation of background */

	UBYTE	Event_Flags;		/* Event detection flags */

#define EVENT_DILAT 0x01	/* Set is dilatation, unset is compression */
#define EVENT_DECON 0x02	/* When set results are deconvolved, if unset
					results are digital counts */

	UBYTE	Rsrvd[1];		/* Reserved (must be 0) */

	NETTIME	Onset_Time;		/* Onset Time of Event */
	UBYTE   Detector_Name[24];      /* Detector Name */
};
#define EVTDET_SZ (sizeof (struct event_detect))

/*
 *	
 *	Murdock Event Detection Blockette [201]
 *	
 */

#define BLK_MURDET 201

struct murdock_detect {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETFLT	Signal_Amplitude;	/* Event signal amplitude */
	NETFLT	Signal_Period;		/* Period of signal */
	NETFLT	Background_Estimate;	/* Estimation of background */

	UBYTE	Event_Flags;		/* Event detection flags */

#define MEVENT_DILAT 0x01	/* Set is dilatation, unset is compression */

	UBYTE	Rsrvd[1];		/* Reserved (must be 0) */

	NETTIME	Onset_Time;		/* Onset Time of Event */
	UBYTE	SNR_Qual[6];		/* Quality values */

	UBYTE	Lookback;		/* Lookback type */
	UBYTE	Pick;			/* Pick algorithm */
	UBYTE   Detector_Name[24];      /* Detector Name */

};
#define MURDET_SZ (sizeof (struct murdock_detect))

/*
 *	
 *	Step Cal Blockette [300]
 *	
 */

#define BLK_STEPCAL 300
struct step_cal {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETTIME	First_Cal;		/* Time of first cal in sequence 
					   (might be in previous records) */
	UBYTE   Number_Steps;           /* Number of steps */
	UBYTE	Cal_Flags;		/* Cal information */

#define STEP_POSITI 0x01	/* First step was positive */
#define STEP_ALTERN 0x02	/* Steps alternate sign */
#define STEP_AUTOMA 0x04	/* Cal was automatic, otherwise manual */
#define STEP_CONTIN 0x08	/* Cal begin in a previous record */

	ULONG	Duration;		/* Number of .1ms step duration */
	ULONG	Interval;		/* Number of .1ms tween steps */
	NETFLT	Amplitude;		/* Amp of signal in units */

	UBYTE	CalChan[3];             /* Channel with cal input */

/* Extensions */

	NETFLT  RefAmp;                 /* Reference Amplitude */
	UBYTE   Coupling[12];           /* Coupling (Resistive/Capacitive) */
	UBYTE   Rolloff[12];

};
#define STEPCAL_SZ (sizeof (struct step_cal))

/*
 *	
 *	Sine Wave Cal Blockette [310]
 *	
 */

#define BLK_SINECAL 310
struct sine_cal {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETTIME	Cal_Start;		/* Time when cal begins
					   (might be in previous records) */

	UBYTE   Resrv1[1];              

	UBYTE	Sine_Flags;		/* Sine cal flags */

#define SINE_AUTOMA 0x04	/* Cal automatic, otherwise manual */
#define SINE_CONTIN 0x08	/* Cal began in a previous record */
#define SINE_PKTOPK 0x10	/* Peak to peak amplitude */
#define SINE_ZRTOPK 0x20	/* Zero to peak amplitude */
#define SINE_RMSAMP 0x40	/* RMS amplitude */

	ULONG	Duration;		/* Number of .1ms cal duration */

	NETFLT	Period;			/* Sine period in seconds */
	NETFLT	Amplitude;		/* Amp of cal sig in units */

	UBYTE	CalChan[3];             /* Channel with cal input */

	UBYTE	Reserved[1];

/* Extensions */

	NETFLT  RefAmp;                 /* Reference Amplitude */
	UBYTE   Coupling[12];           /* Coupling (Resistive/Capacitive) */
	UBYTE   Rolloff[12];

};
#define SINECAL_SZ (sizeof (struct sine_cal))

/*
 *	
 *	Pseudo Random Cal Blockette [320]
 *	
 */

#define BLK_RANDOMCAL 320
struct random_cal {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	NETTIME	Cal_Start;		/* Time when cal begins
					   (might be in previous records) */

	UBYTE Resrv1[1];

	UBYTE	Random_Flags;		/* Cal Flags */

#define RANDOM_AUTOMA 0x04	/* Automatic cal, otherwise manual */
#define RANDOM_CONTIN 0x08	/* Cal began in previous record */
#define RANDOM_RANAMP 0x10	/* Amps are random too, see cal in chan */

	ULONG	Duration;		/* Number of .1ms cal duration */
	NETFLT	Amplitude;		/* Amplitude of steps in units */

	UBYTE	CalChan[3];             /* Channel with cal input */

	UBYTE	Reserved[1];

/* Extensions */

	NETFLT  RefAmp;                 /* Reference Amplitude */
	UBYTE   Coupling[12];           /* Coupling (Resistive/Capacitive) */
	UBYTE   Rolloff[12];     
	UBYTE   Noise_Type[8];          /* Type WHITE, RED */

};
#define RANDOMCAL_SZ (sizeof (struct random_cal))

/*
 *	
 *	Generic Cal Blockette [390]
 *	
 */

#define BLK_GENCALCAL 390
struct Generic_cal {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	UWORD	Cal_In_Chan;		/* Channel ID containing cal input 
						must be present */
	NETTIME	Cal_Start;		/* Time when cal begins
					   (might be in previous records) */
	ULONG	Duration;		/* Number of .1ms cal duration */
	NETFLT	Amplitude;		/* Amplitude of steps in units */

	UBYTE	Gencal_Flags;		/* Cal Flags */

#define GENCAL_AUTOMA 0x04	/* Automatic cal, otherwise manual */
#define GENCAL_CONTIN 0x08	/* Cal began in previous record */

	UBYTE	Reserved[3];
};
#define GENCALCAL_SZ (sizeof (struct Generic_cal))

/*
 *
 *      Data Only Blockette [1000]
 * 
 */

#define BLK_DATAONLY 1000
struct Data_only {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	UBYTE	Encoding;
	UBYTE   Order;
	UBYTE   Length;
	UBYTE   Resv1;
};
#define DATAONLY_SZ (sizeof (struct Data_only))

#define FMT_LOG 0
#define	FMT_16_WORD	1
#define	FMT_24_WORD	2
#define	FMT_32_WORD	3
#define	FMT_IEEE_SP	4
#define	FMT_IEEE_DP	5
#define	FMT_STEIM_1	10
#define	FMT_STEIM_2	11
#define	FMT_GEOSCOPE_MPX24	12
#define	FMT_GEOSCOPE_16_3	13
#define	FMT_GEOSCOPE_16_4	14
#define	FMT_USNSN	15
#define	FMT_CDSN	16
#define	FMT_GRAEF	17
#define	FMT_IPG	18
#define FMT_STEIM_3     19
#define	FMT_SRO_ASRO	30
#define	FMT_HGLP	31
#define	FMT_DWWSSN	32
#define	FMT_RSTN	33

/*
 *
 *      Quanterra Data Extension Blockette [1001]
 * 
 */

#define BLK_DATAEXT 1001
struct Data_ext {
	UWORD	Blockette_Type;		/* Blockette identifier */
	UWORD	Next_Begin;		/* Byte where next blockette begins */

	UBYTE	Timing;
	BYTE   Usec;
	UBYTE   Resv1;
	UBYTE   Frames;
};
#define DATAEXT_SZ (sizeof (struct Data_ext))

#include "seed_data_proto.h"
