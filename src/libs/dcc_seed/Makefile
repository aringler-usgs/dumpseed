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

INSTALL = ../../../install
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
OBJS =	diemsg.o	\
	seed_blockettes.o	\
	seed_comments.o	\
	seed_comp.o	\
	seed_data.o	\
	seed_data_dec.o	\
	seed_dec_16.o	\
	seed_dec_32.o	\
	seed_dec_ascii.o	\
	seed_dec_cdsn.o	\
	seed_dec_seed.o	\
	seed_dec_usnsn.o	\
	seed_dec_sro.o	\
	seed_dicts.o	\
	seed_membase.o	\
	seed_misc.o	\
	seed_responses.o	\
	seed_statchan.o	\
	seed_write.o	\
	sel_mem.o

all:	proto $(DCC_SEED)

proto: 
	$(PROTO) sel_*.*c >$(DCC_INCLUDE)/seed/select_proto.h
	$(PROTO) seed_comp.c >$(DCC_INCLUDE)/seed/seed_comp_proto.h
	$(PROTO) \
		diemsg.c        \
        	seed_comments.c         \
                seed_dicts.c            \
                seed_membase.c          \
                seed_misc.c             \
                seed_responses.c        \
                seed_statchan.c         \
                seed_write.c            \
                seed_blockettes.c       \
	                   >$(DCC_INCLUDE)/seed/seed_proto.h
	$(PROTO) \
                seed_data.c \
                seed_data_dec.c \
                seed_dec_16.c \
                seed_dec_32.c \
                seed_dec_sro.c \
                seed_dec_cdsn.c \
                seed_dec_seed.c \
                seed_dec_usnsn.c \
                seed_dec_ascii.c >$(DCC_INCLUDE)/seed_data_proto.h

$(DCC_SEED): $(OBJS)
	$(AR) rv $@ $(OBJS)
	${RANLIB} $(DCC_SEED)

clean:
	rm -f *.o *.a core *~ \#*\#

dist:	clean
	rm -f	$(DCC_INCLUDE)/seed/select_proto.h
	rm -f	$(DCC_INCLUDE)/seed/seed_comp_proto.h
	rm -f 	$(DCC_INCLUDE)/seed/seed_proto.h
	rm -f 	$(DCC_INCLUDE)/seed_data_proto.h
	rm -f Makefile
	rm -f $(DCC_SEED)

install:
	${INSTALL_DATA} $(DCC_SEED) $(libdir)/libdcc_seed.a
	${RANLIB} $(libdir)/libdcc_seed.a

uninstall:
	rm -f $(libdir)/libdcc_seed.a
