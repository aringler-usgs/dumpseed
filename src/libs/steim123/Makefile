# Generated automatically from Makefile.in by configure.
# DCC makefile
#

# BEGIN TEMPLATE SECTION

bindir = ${exec_prefix}/bin
datadir = ${prefix}/share
exec_prefix = ${prefix}
includedir = ${prefix}/include
infodir = ${prefix}/info
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
oldincludedir = /usr/include
prefix = /usr/local
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
srcdir = .
sysconfdir = ${prefix}/etc
top_srcdir = ../../..
CFLAGS = -g -O2
CPPFLAGS = 
CXXFLAGS = 
DEFS = -DHAVE_CONFIG_H
LDFLAGS = 
LIBS = -lnsl 
CHMOD = chmod
DIRMOD = go+w

DCC_INCLUDE = ../../../include
DCC_LIB = ../../../lib
DCC_BIN = ${prefix}/bin

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
CC = gcc
RANLIB = ranlib
PROTO = ../../../bin/make_proto

WARN = -Wall
INCS = -I$(DCC_INCLUDE)

CFLAGS += $(INCS) $(PROF) $(WARN)

# DCC library definitions

DCC_SEED = $(DCC_LIB)/libdcc_seed.a

DCC_MISC = $(DCC_LIB)/libdcc_misc.a

DCC_SCRN = $(DCC_LIB)/libdcc_scrn.a

STEIM = $(DCC_LIB)/libsteim123.a

DCC_STATIONS = $(DCC_LIB)/libdcc_stations.a

DCC_TIME = $(DCC_LIB)/libdcc_time.a

DCC_JUKE = $(DCC_LIB)/libdcc_juke.a

DCC_BASES = $(DCC_LIB)/libdcc_bases.a

DCC_PLOT = $(DCC_LIB)/libdcc_plot.a

DCC_MD = $(DCC_LIB)/libmd.a

DCC_LIBS = $(DCC_STATIONS) $(DCC_BASES) $(DCC_TIME) $(DCC_SCRN) \
	$(DCC_PLOT) $(DCC_MISC) $(SOCKETS) $(STEIM)

# END TEMPLATE SECTION

PROF = -O2

OBJS=	steimlib.o	\
	steim_decode.o	\
	steim_encode1.o	

all:	$(STEIM)

$(STEIM): $(OBJS)
	$(AR) rv $@ $(OBJS)
	${RANLIB} $(STEIM)

clean:
	rm -f *.o *.a core *~ \#*\#

dist:	clean
	rm -f Makefile
	rm -f $(STEIM)

install:
	${INSTALL_DATA} $(STEIM) $(libdir)/libsteim123.a
	${RANLIB} $(libdir)/libsteim123.a

INCLUDE= .

steimlib.o:	$(INCLUDE)/steim.h steimlib.c
		$(CC) -c $(CFLAGS) -I$(INCLUDE) steimlib.c

steim_decode.o:	$(INCLUDE)/steim.h steim_decode.c
		$(CC) -c $(CFLAGS) -I$(INCLUDE) steim_decode.c

steim_encode1.o:	$(INCLUDE)/steim.h steim_encode1.c
		$(CC) -c $(CFLAGS) -I$(INCLUDE) steim_encode1.c

sprofile.o:	$(INCLUDE)/steim.h $(INCLUDE)/sprofile.h sprofile.c
		$(CC) -c $(CFLAGS) -I$(INCLUDE) sprofile.c


uninstall:
	rm -f $(libdir)/libsteim123.a


