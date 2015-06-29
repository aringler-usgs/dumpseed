#define STEIM_BADTYPECODE 0x01
#define STEIM_RCMISMATCH 0x02
#define STEIM_INTERCONTEXT 0x04

struct steimctx {
  int init;
  long prevrc;
};


