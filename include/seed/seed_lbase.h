/* Structure/linked list to contain network/station/location/channel spans */

struct lbase_sdb {
	STDTIME	start;
	STDTIME	end;
	int	interesting;
	int	update;
	struct	lbase_sdb *next;
};

#include <seed/loadbase_proto.h>
