# Generated automatically from Makefile.in by configure.
# Main Makefile

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
top_srcdir = .
CFLAGS = -g -O2
CPPFLAGS = 
CXXFLAGS = 
DEFS = -DHAVE_CONFIG_H
LDFLAGS = 
LIBS = -lnsl 
CHMOD = chmod
DIRMOD = go+w

DCCPACKAGE=LISS-utils
DCCVERSION=2.0.0

all:	
	cd src/utils; make all
	cd src/libs; make all
	cd src/apps; make all

install: installdirs
	cd src/libs; make install
	cd src/apps; make install

uninstall: 
	cd src/libs; make uninstall
	cd src/apps; make uninstall

clean: 
	rm -f *.o *.a core *~ \#*\#
	cd src/libs; make clean
	cd src/apps; make clean
	cd src/utils; make clean

dist:	clean
	rm -f include/dcc_config.h
	rm -f config.* Makefile
	rm -f libs/*.a
	cd src/libs; make dist
	cd src/apps; make dist
	cd src/utils; make dist

distribution: dist
	rm -f ../${DCCPACKAGE}-${DCCVERSION}.tar*
	cd ..; tar -cvf ${DCCPACKAGE}-${DCCVERSION}.tar \
		${DCCPACKAGE}-${DCCVERSION}
	gzip -9vc ../${DCCPACKAGE}-${DCCVERSION}.tar \
		>../${DCCPACKAGE}-${DCCVERSION}.tar.gz
	compress ../${DCCPACKAGE}-${DCCVERSION}.tar

release: distribution
	cp -f ../${DCCPACKAGE}-${DCCVERSION}.tar.* /r08/ftp/pub/LISS/dist

installdirs: mkinstalldirs
	$(srcdir)/mkinstalldirs $(bindir) $(datadir) $(libdir) $(infodir) \
		$(mandir) $(mandir)/man1 $(mandir)/man4

configure: configure.in
	autoconf

include/dcc_include.h.in: configure.in
	autoheader

