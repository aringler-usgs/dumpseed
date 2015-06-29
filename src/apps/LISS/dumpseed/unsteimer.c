#include <dcc_std.h>
#include <dcc_time.h>
#include <seed_data.h>

/* All data in Steim compression are stored in BIG_ENDIAN or a 68000/sun
   byte swapping order.  The following macros are defined to perform byte
   swapping in the event that the software is running on a LITTLE_ENDIAN
   machine such as the VAX or IBM-PC */

#define LW(a) a /* LocGM68_WORD(a) */
#define LL(a) a /* LocGM68_LONG(a) */

/* Steim Compression Description - composed of a series of 64 byte frames,
   each beginning with a 4 byte control header and followed by 15 4 bytes
   "chunks".  Each chunk can contain 4 byte values, 2 word values, or 1
   long, depending on the flags contained in the control header */

static struct steimformat {
	struct {
		ULONG	ctlflags;
		union {
			struct { BYTE byteval[4]; } type_1;
			struct { WORD wordval[2]; } type_2;
			struct { LONG longval;    } type_3;
		} chunkstore[15];
	} framestore[64];
} *compress_arena;

/* SEED steim information - the first 8 bytes of the first frame (after
   the control header) always contains the following header information */

static struct steimheader1 {
	LONG	ctlflags;
	LONG	forward_integrating_const;
	LONG	reverse_integrating_const;
} *headerval;

/* Access a given 2-bit control nibble in the control longword of a
   given frame, the 2 bit values start on the left (bits 30 & 31) and
   end on the right */

int SteimCtlHeader(inlong,pos)
ULONG inlong;
int pos;
{

	int sf,out;

	sf = (15-(pos+1))*2;
	if (sf>0) inlong >>= sf;

	out = inlong&0x03;
	return(out);
}

/* Decompress a series of steim frames, the program will count the
   'sams_to_go' variable down to 0 and will stop, disreguarding any
   later data present in the record.  Data will be returned in the 
   long array 'out_array'.  Note that 'out_array' should always be at
   least as large as 'sams_to_go' is. */

VOID SEED_ConvSteim(UBYTE *inbuf,int sams_to_go,LONG *out_array)
{

	register BYTE i,frame,chunk,frtype;
	register LONG running_sum;
	LONG answer[4];
	int sams_decoded,numans=0;
	BOOL ir_delta_skip;

	sams_decoded = 0;

	/* Set up pointers into our data structures */

	compress_arena = (struct steimformat *) inbuf;
	headerval = (struct steimheader1 *) inbuf;

	/*printf("Forward = %d, Reverse = %d\n",
	       LL(headerval->forward_integrating_const),
	       LL(headerval->reverse_integrating_const));*/

	/* Retreive the first sample and store it...  We must later
	   remove the first delta because it is the delta between
	   the records */
	
	running_sum = LL(headerval->forward_integrating_const);
	*out_array++ = running_sum;

	/* Increment samples decoded, decrement samples to go */

	sams_decoded++;
	sams_to_go--;

	/* Flag to skip the interrecord delta */

	ir_delta_skip = TRUE;	/* Toss the delta between records */

	/* Loop through possible data - note that we will decode
	   till we get to sams_to_go and ignore anything after that */

	for (frame=0;frame<64; frame++) {	/* Usually only 63 */
		for (chunk=0; chunk<15; chunk++) {

			/* We're counting down from sams_to_go */
			if (sams_to_go<=0) break;	/* Out of samples */

			/* Header chunk type from header in this frame */

   	    		frtype = SteimCtlHeader(LL(
                            compress_arena->framestore[frame].ctlflags),chunk);

			/* Deal with each chunk, assemble data into the
			   'answer' array and give us a count */

			if (frtype==0) continue; /* No-data type */

			switch (frtype) {

		        case 1:			/* 4 bytes type */
				answer[0] = compress_arena->framestore[frame].
				   chunkstore[chunk].type_1.byteval[0];
				answer[1] = compress_arena->framestore[frame].
				   chunkstore[chunk].type_1.byteval[1];
				answer[2] = compress_arena->framestore[frame].
				   chunkstore[chunk].type_1.byteval[2];
				answer[3] = compress_arena->framestore[frame].
				   chunkstore[chunk].type_1.byteval[3];
				numans = 4;
				break;

			case 2:			/* 2 word type */
				answer[0]=LW(compress_arena->framestore[frame].
				   chunkstore[chunk].type_2.wordval[0]);
				answer[1]=LW(compress_arena->framestore[frame].
				   chunkstore[chunk].type_2.wordval[1]);
				numans = 2;
				break;

			case 3:			/* 1 long type */
				answer[0]=LL(compress_arena->framestore[frame].
				   chunkstore[chunk].type_3.longval);
				numans = 1;
				break;
			}

			/* Store away the data in 'answer' */

			for (i=0; i<numans; i++) {
				/* Skip the interrecord delta when
				   ir_delta_skip is set */
				if (ir_delta_skip) ir_delta_skip=FALSE;
				else {
					running_sum += answer[i];
					*out_array++ = running_sum;
					sams_decoded++;
					sams_to_go--;
					if (sams_to_go<=0) break;
				}
			}
		}
	}

	/* Check to see if we have counted 'sams_to_go' to zero.
	   this could happen if the data contained more than 64 
	   frames.  This would usually mean that the 'sams_to_go'
	   value is bad. */

	if (sams_to_go>0) printf("Sams_To_Go = %d??\n",sams_to_go);

	/* Check to see if the reverse integrating constant in the
	   record is correct -- this is only a double check */

	/*if (running_sum!=LL(headerval->reverse_integrating_const)) {
		printf("Reverse integration constant not right!\n");
		printf("%ld (rev) is not equal to %ld (actual)\n",
		       LL(headerval->reverse_integrating_const),
		       running_sum);
	}*/

}
