#ifndef ITEMLIST_DEF

#define ITEMLIST struct _itemlist

struct _itemlist {
	struct	_itemlist	*next;
	char	*item;
};

#define ITEMLIST_DEF
#endif
