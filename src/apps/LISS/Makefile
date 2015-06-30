# Generated automatically from Makefile.in by configure.
# Generated automatically from Makefile.in by configure.

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

all:
	cd dumpseed; make all
	cd seedport; make all
	cd seedforker; make all
	cd seedfilter; make all

clean: 
	rm -f *.o *.a core *~ \#*\#
	cd dumpseed; make clean
	cd seedport; make clean
	cd seedforker; make clean
	cd seedfilter; make clean

dist:	clean
	rm -f Makefile
	cd dumpseed; make dist
	cd seedport; make dist
	cd seedforker; make dist
	cd seedfilter; make dist

install:
	cd dumpseed; make install
	cd seedport; make install
	cd seedforker; make install
	cd seedfilter; make install

uninstall:
	cd dumpseed; make uninstall
	cd seedport; make uninstall
	cd seedforker; make uninstall
	cd seedfilter; make uninstall
