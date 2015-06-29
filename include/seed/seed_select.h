struct	sel3_stat {
	struct sel3_stat *next;
	char            *network;
	char		*station;
	unsigned int	addnot : 1;
	unsigned int	class  : 1;
};

struct sel3_chan {
	struct sel3_chan *next;
	char		*location;
	char		*channel;
	unsigned int	addnot : 1;
};

struct sel3_date {
	struct sel3_date *next;
	ULONG		sjday,ejday;
	STDTIME		sdate,edate;
	unsigned int	addnot : 1;
};

extern STDTIME _ztime;

char *filename;

struct sel3_chain {
	struct sel3_chain *next;
	struct sel3_stat *root_stat,*tail_stat;
	struct sel3_chan *root_chan,*tail_chan;
	struct sel3_date *root_date,*tail_date;	
	ITEMLIST	*taglist;
};

struct sel3_root {
  struct sel3_chain *root_chain,*tail_chain;
  ULONG jdmax,jdmin;
};


#include <seed/select_proto.h>
