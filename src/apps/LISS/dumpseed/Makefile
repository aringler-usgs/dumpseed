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
top_srcdir = ../../../..
CFLAGS = -g -O2
CPPFLAGS = 
CXXFLAGS = 
DEFS = -DHAVE_CONFIG_H
LDFLAGS = 
LIBS = -lnsl 
CHMOD = chmod
DIRMOD = go+w

DCC_INCLUDE = ../../../../include
DCC_LIB = ../../../../lib
DCC_BIN = ${prefix}/bin

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
CC = gcc
RANLIB = ranlib
PROTO = ../../../../bin/make_proto

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

all:	proto dumpseed 

dumpseed: dumpseed.o unsteimer.o readseedport.o steimdump.o \
		prtseedrec.o prtseedbdf.o
	$(CC)  -o dumpseed dumpseed.o unsteimer.o readseedport.o \
	    steimdump.o prtseedrec.o prtseedbdf.o \
		$(DCC_SEED) $(STEIM) $(DCC_TIME) $(DCC_MISC) -lnsl  -lm

proto:
	$(PROTO) *.c >dumpseed_proto.h

clean:
	rm -f *.o *.a core *~ \#*\#

dist:	clean
	rm -f dumpseed_proto.h
	rm -f dumpseed

install:
	${INSTALL_PROGRAM} dumpseed $(bindir)/dumpseed
	${INSTALL_DATA} dumpseed.1 $(mandir)/man1/dumpseed.1
	${INSTALL_DATA} selectfile.4 $(mandir)/man4/selectfile.4
	${INSTALL_DATA} BDF.4 $(mandir)/man4/BDF.4
	${INSTALL_DATA} SEED.4 $(mandir)/man4/SEED.4

uninstall:
	rm -f $(bindir)/dumpseed
	rm -f $(mandir)/man1/dumpseed.1
	rm -f $(mandir)/man4/selectfile.4
	rm -f $(mandir)/man4/BDF.4
	rm -f $(mandir)/man4/SEED.4

#dumpseed.1.html: dumpseed.1
#	rman -f HTML dumpseed.1 >dumpseed.1.html

#selectfile.4.html: selectfile.4
#	rman -f HTML selectfile.4 >selectfile.4.html

#BDF.4.html: BDF.4
#	rman -f HTML BDF.4 >BDF.4.html

#SEED.4.html: SEED.4
#	rman -f HTML SEED.4 >SEED.4.html

