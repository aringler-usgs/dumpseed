# Generated automatically from Makefile.in by configure.
# DCC makefile

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
top_srcdir = ../..
CFLAGS = -g -O2
CPPFLAGS = 
CXXFLAGS = 
DEFS = -DHAVE_CONFIG_H
LDFLAGS = 
LIBS = -lnsl 
CHMOD = chmod
DIRMOD = go+w

DCC_INCLUDE = ../../include
DCC_LIB = ../../lib
DCC_BIN = ${prefix}/bin

INSTALL = ../../install
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
CC = gcc
RANLIB = ranlib
PROTO = ../../bin/make_proto

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

all:	make_proto

make_proto:	make_proto.o
	$(CC) -o make_proto make_proto.o
	$(INSTALL_PROGRAM) make_proto $(PROTO)

clean:
	rm -f *.o *.a core *~ \#*\#

dist:	clean
	rm -f $(PROTO) make_proto
	rm -f Makefile

install:

uninstall:
