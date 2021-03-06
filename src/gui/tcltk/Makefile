# Copyright 2004-2016 BitMover, Inc
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

HERE	:= $(shell pwd)
OSTYPE	:= $(shell bash -c 'echo $$OSTYPE')
BKROOT	:= $(shell cd ../../.. && pwd)
BK	:= $(BKROOT)/src/bk

CC ?= gcc
LD ?= gcc
export CC LD

# The Tk version is tied to the Tcl version so you only need to change TCL
TCLVERSION := 8.6
TCLVERSIONNODOTS := 86

# Machines on which we disable XFT
NO_XFT := netbsd4 sunx86

ifeq "$(OSTYPE)" "msys"
	S := win
	EXE := .exe
	SO := .dll
	XFTSUPPORT :=
	SHARED := --enable-shared
	PCRE_OBJ:= bin/tcl$(subst .,,$(TCLVERSION)).dll
else
	S := unix
	EXE :=
	SO  := .so
	XSSSUPPORT := --disable-xss
	ifeq (,$(findstring $(HOSTNAME), $(NO_XFT)))
		XFTSUPPORT := --enable-xft
	endif
	SHARED := --disable-shared
	PCRE_OBJ:= bin/tclsh
	ifeq "$(shell uname)" "Darwin"
		AQUA := 1
		XFTSUPPORT :=
		BKAPP=bin/BitKeeper.app
		SO := .dylib
		SINGLE_MODULE := "LDFLAGS=-single_module"
	endif
	ifeq "$(shell uname -m)" "x86_64"
		ENABLE_64BIT=--enable-64bit
	endif
	# Work around for suspected compiler bug on hp-ia64.
	# Do like a debug build but use -O2.
	ifeq "$(shell uname -s)" "HP-UX"
		SYMBOLS="--enable-symbols=all-with-O2"
	endif
	# No rpath on freebsd 2.2.8
	ifeq "$(shell uname -r)" "2.2.8-RELEASE"
		SHARED := $(SHARED) --disable-rpath
	endif
	ifeq "$(shell uname -s)" "SunOS"
	ifeq "$(shell isainfo -b)" "64"
		CC := gcc -m64
		export CC
		ENABLE_64BIT := --enable-64bit
	endif
	endif
endif

ifdef	DEBUG_TCL
	SYMBOLS	:= --enable-symbols=all
endif

ifdef	Q
	QUIET=-q
endif

DDEVERSION := 1.3
REGVERSION := 1.3
TKTABLE := Tktable2.10

LIBTKTABLE := lib$(TKTABLE)$(SO)
BWIDGET := BWidget1.8
TCLBUILD := ../build/tcl/Tcl.framework
TKBUILD  := ../build/tk/Tk.framework
WISHPATH := build/tk/Wish.app
EXTENSIONS := lib/$(TKTABLE)/$(LIBTKTABLE) \
	      lib/$(BWIDGET)/pkgIndex.tcl \
	      lib/Tkcon/pkgIndex.tcl
AQUAEXTENSIONS := $(BKAPP)/Contents/lib/$(TKTABLE)/$(LIBTKTABLE) \
		  $(BKAPP)/Contents/lib/$(BWIDGET)/pkgIndex.tcl \
		  $(BKAPP)/Contents/lib/Tkcon/pkgIndex.tcl

ifeq "$(OSTYPE)" "openbsd4.2"
LIBTCL=$(BKROOT)/src/gui/tcltk/tcl/$S/libtcl$(TCLVERSIONNODOTS).a
else
LIBTCL=$(BKROOT)/src/gui/tcltk/tcl/$S/libtcl$(TCLVERSION).a
endif
TOMMATH=$(BKROOT)/src/tommath
ifneq ($(PCRE_SYSTEM),1)
	LIBPCRE=$(BKROOT)/src/gui/tcltk/pcre/local/lib/libpcre.a
	PCRE_LDFLAGS ?= $(LIBPCRE)
	export PCRE_LDFLAGS
	PCRE_CFG = "--with-pcre=$(BKROOT)/src/gui/tcltk/pcre/local"
endif

all: version.sh
	$(if $(Q),@echo Building Tcl/Tk,)
	-$(Q)rm -rf bin lib share
	$(Q)TCLVER=`./version.sh`; \
	if [ -n "$$TCLVER" -a -f "$$TCLVER" -a -z "$$FORCE_REBUILD" ]; \
	then	gunzip < "$$TCLVER" | tar -xf -; true; \
	else	unset CC LD CCXTRA CFLAGS; \
		MAKEFLAGS=`echo "$$MAKEFLAGS" | sed 's/ -- .*//'`; \
		$(MAKE) Q=$(Q) populate ; \
		$(MAKE) Q=$(Q) bin/tclsh$(EXE) || exit 1; \
		if [ "X$(AQUA)" = X ] ; \
		then $(MAKE) Q=$(Q) bin/bkgui$(EXE) $(EXTENSIONS) || exit 1; \
		else \
			$(MAKE) Q=$(Q) $(BKAPP)/Contents/MacOS/BitKeeper || exit 1; \
			$(MAKE) Q=$(Q) $(AQUAEXTENSIONS) || exit 1; \
		fi \
	fi
	$(Q)$(MAKE) verify
	$(Q)$(MAKE) rm_doc
	$(Q)$(MAKE) install

srcs: pcre/configure \
	tcl/$S/configure \
	tk/$S/configure \
	tktable/configure \
	lib/$(BWIDGET)/pkgIndex.tcl \
	lib/Tkcon/pkgIndex.tcl

src-check: src-check.sh
	@make -s srcs
	@./src-check.sh

tcl/$S/Makefile: tcl/$S/configure $(LIBPCRE) Makefile
	$(if $(Q),@echo Configuring Tcl,)
	$(Q)cd tcl/$S; \
	./configure $(QUIET) \
	    --enable-pcre=default $(PCRE_CFG) \
	    $(ENABLE_64BIT) $(SHARED) $(SYMBOLS) \
	    --with-tommath=$(TOMMATH)

tk/$S/Makefile: tk/$S/configure Makefile bin/tclsh$(EXE)
	$(if $(Q),@echo Configuring Tk,)
	$(Q)cd tk/$S; \
	./configure $(QUIET) --with-tcl=../../tcl/$S \
	    $(ENABLE_64BIT) $(XSSSUPPORT) $(XFTSUPPORT) $(SHARED) $(SYMBOLS)

populate:
	$(if $(Q), @echo Cloning TclTk repos,)
	-$(Q) $(BK) --config=checkout:get here add $(QUIET) TCLTK

# checkout tcl subdirectory if needed
tcl/$S/configure:
	-$(Q)(cd tcl && bk -Ur get -qST)

# checkout tk subdirectory if needed
tk/$S/configure:
	-$(Q)(cd tk && bk -Ur get -qST)

pcre: $(LIBPCRE)

$(LIBPCRE): pcre/Makefile Makefile
	$(if $(Q), @echo Building PCRE,)
	$(Q)(cd pcre && $(MAKE) && \
	    $(MAKE) prefix= exec_prefix= DESTDIR="`pwd`"/local/ \
		libdir=lib bindir=bin includedir=include install)

pcre/Makefile: pcre/configure Makefile
	$(if $(Q), @echo Configuring PCRE,)
	$(Q)(cd pcre && \
	    ./configure $(QUIET) --disable-cpp --disable-shared --enable-utf8=yes --prefix=)

pcre/configure: Makefile
	-$(Q)(cd pcre && bk -Ur get -qS)

# fetch tktable subdirectory if needed
tktable/configure:
	-$(Q)(cd tktable && bk -Ur get -qS)

# Tktable libs
lib/$(TKTABLE)/$(LIBTKTABLE): tktable/configure bin/bkgui$(EXE)
	cd tktable ; \
	test -f Makefile -a "X$(AQUA)" != X && { \
		$(MAKE) distclean ; \
	}; \
	test -f Makefile || { \
		./configure $(QUIET) --with-tcl=../tcl/$S --with-tk=../tk/$S \
			--with-tclinclude=../tcl/generic \
			--with-tkinclude=../tk/generic \
			--disable-threads $(ENABLE_64BIT) $(SYMBOLS) ; \
	}; \
	$(MAKE) $(SINGLE_MODULE) \
		prefix= exec_prefix= DESTDIR=../ \
		libdir=lib bindir=bin install

$(BKAPP)/Contents/lib/$(TKTABLE)/$(LIBTKTABLE): tktable/configure
	cd tktable; \
	test -f Makefile -a "X$(AQUA)" != X && { \
		$(MAKE) distclean ; \
	}; \
	test -f Makefile || { \
		./configure $(QUIET) \
		    --with-tcl=$(TCLBUILD) \
		    --with-tk=$(TKBUILD) \
		    --with-tclinclude=$(TCLBUILD)/Headers \
		    --with-tkinclude=$(TKBUILD)/Headers $(SYMBOLS) ; \
	}; \
	$(MAKE) prefix= exec_prefix= DESTDIR=../$(BKAPP)/Contents/ \
	    libdir=lib bindir=Resources install

$(BKAPP)/Contents/lib:
	mkdir $(BKAPP)/Contents/lib

lib/$(BWIDGET)/pkgIndex.tcl:
	$(if $(Q), @echo Installing BWidget,)
	$(Q)mkdir -p lib/$(BWIDGET)/lang
	$(Q)mkdir -p lib/$(BWIDGET)/images
	$(Q)cp bwidget/*.tcl lib/$(BWIDGET)
	$(Q)cp bwidget/lang/*.rc lib/$(BWIDGET)/lang
	$(Q)cp bwidget/images/*.gif lib/$(BWIDGET)/images
	$(Q)cp bwidget/images/*.xbm lib/$(BWIDGET)/images

$(BKAPP)/Contents/lib/$(BWIDGET)/pkgIndex.tcl: lib/$(BWIDGET)/pkgIndex.tcl \
	$(BKAPP)/Contents/lib
	cp -fRPH lib/$(BWIDGET) $(BKAPP)/Contents/lib

lib/Tkcon/pkgIndex.tcl:
	$(if $(Q), @echo Installing TkCon,)
	$(Q)mkdir -p lib/Tkcon
	$(Q)cp tkcon/tkcon.tcl lib/Tkcon
	$(Q)cp tkcon/pkgIndex.tcl lib/Tkcon

$(BKAPP)/Contents/lib/Tkcon/pkgIndex.tcl: lib/Tkcon/pkgIndex.tcl \
	$(BKAPP)/Contents/lib
	cp -fRPH lib/Tkcon $(BKAPP)/Contents/lib

$(BKAPP)/Contents/lib/GetOpts/pkgIndex.tcl: lib/GetOpts/pkgIndex.tcl \
	$(BKAPP)/Contents/lib
	cp -fRPH lib/GetOpts $(BKAPP)/Contents/lib

rm_doc: FORCE
	$(Q)rm -rf lib/$(TKTABLE)/html
	$(Q)if [ "X$(AQUA)" != X ]; \
	then \
	    rm -rf \
		$(BKAPP)/Contents/lib/$(TKTABLE)/html \
		$(BKAPP)/Contents/man ; \
	fi

install: ./version.sh
	$(Q)find bin lib share -type d -print | xargs chmod ug+w
	$(Q)rm -rf ../bin ../lib ../share 
	$(Q)tar cf - bin lib share | (cd .. && tar xf -)
	-$(Q)TCLVER=`./version.sh`; \
	if [ -n "$$TCLVER" -a \! -f "$$TCLVER" ]; \
	then	test -d /build/obj || { \
			mkdir /build/obj; \
			chmod 777 /build/obj; \
		}; \
		tar cf - bin lib share | gzip > "$$TCLVER"; \
		chmod 666 "$$TCLVER"; \
	fi

bin/tclsh$(EXE): tcl/$S/Makefile Lversion-L.sh Lversion-C.sh
	rm -rf bin lib usr include share
	mkdir include
	cd tcl && ../Lversion-L.sh >library/Lver.tcl
	cd tcl && ../Lversion-C.sh >generic/Lver.h
	cd tcl/$S && \
	$(MAKE) Q=$(Q) prefix= exec_prefix= INSTALL_ROOT="../.." \
	    XLIBS="$(PCRE_LDFLAGS)" \
	    install-binaries install-libraries install-L-doc
	if [ -x bin/tclsh$(TCLVERSION)$(EXE) ] ; then \
		mv bin/tclsh$(TCLVERSION)$(EXE) bin/tclsh$(EXE) ; \
	fi
	if [ ! -x bin/tclsh$(EXE) ] ; then \
		echo Could not find bin/tclsh$(EXE) ; \
	fi
	if [ -d usr/local/lib/dde$(DDEVERSION) ] ; then \
		mv usr/local/lib/dde$(DDEVERSION) lib/tcl$(TCLVERSION) ; \
	fi
	if [ -d usr/local/lib/reg$(REGVERSION) ] ; then \
		mv usr/local/lib/reg$(REGVERSION) lib/tcl$(TCLVERSION); \
	fi
	rm -rf include usr lib/tcl$(TCLVERSION)/tcltest*

bin/bkgui$(EXE): tk/$S/Makefile bin/tclsh$(EXE)
	rm -rf bin/bkgui$(EXE) lib/tk*
	cd tk/$S && \
	$(MAKE) prefix= exec_prefix= INSTALL_ROOT=../.. \
	    XLIBS="$(PCRE_LDFLAGS)" \
	    BK_TCL_LIB="$(LIBTCL)" \
	    install-binaries install-libraries
	rm -rf include usr lib/tk*/images
	cd lib/tcl$(TCLVERSION)/encoding/; \
	    mkdir tmp; \
	    mv ascii.* cp1252.* iso8859-1.* iso8859-2.* macRoman.* tmp; \
	    rm -f *.enc; \
	    mv tmp/* .; \
	    rmdir tmp
	mv bin/wish* bin/bkgui$(EXE)

mkapp: FORCE
	-bk get mkapp

bin/BitKeeper.app/Contents/MacOS/BitKeeper: bin/tclsh$(EXE) mkapp
	$(MAKE) -C tcl/macosx \
	    EXTRA_CONFIGURE_ARGS="$(SYMBOLS) \
	    --enable-pcre=default $(PCRE_CFG) \
	    --disable-dtrace \
	   --with-tommath=$(TOMMATH) $(SYMBOLS)" embedded
	$(MAKE) -C tk/macosx XLIBS="$(PCRE_LDFLAGS)" \
	    EXTRA_CONFIGURE_ARGS="$(SYMBOLS) --enable-aqua" embedded
	./mkapp BitKeeper 1.0 BkAp "BitKeeper Inc" com.bitmover.bk "$(WISHPATH)"

clean-obj: FORCE
	-test -d tcl && cd tcl/$S && test -f Makefile && $(MAKE) distclean
	-test -d tk && cd tk/$S && test -f Makefile && $(MAKE) distclean
	-test -d tk && rm -f tk/doc/man.macros
	-test -d tktable && cd tktable && test -f Makefile && $(MAKE) distclean
	-test -d pcre && cd pcre && { \
		test -f Makefile && $(MAKE) distclean; \
		rm -rf local; \
	}
	-@$(MAKE) clean-config

clean-config: FORCE
	-for i in Makefile config.cache config.log tclConfig.sh config.status; \
	do	rm -f tcl/$S/$$i tk/$S/$$i; \
	done
	-rm -f tcl/library/Lver.tcl tcl/generic/Lver.h

clean: clean-obj FORCE
	rm -rf bin lib man build include usr share
	-rm -rf pcre/a.out.dSYM
	-rm -rf build

clobber: clean FORCE
	-bk sfiles -r | bk clean -	# clean here and all subrepos

rmcache: version.sh
	rm -f `./version.sh`
	-bk clean -q version.sh

test:
	test -d tk && cd tk/$S && $(MAKE) test

verify: FORCE
	$(Q)if [ "X$(AQUA)" != X ]; \
	then \
	    test -d $(BKAPP)/Contents/lib/$(TKTABLE) || exit 1 ; \
	    test -f $(BKAPP)/Contents/lib/$(BWIDGET)/pkgIndex.tcl \
		|| exit 1 ; \
	else \
		test -d lib/$(TKTABLE) || exit 1 ; \
		test -f lib/$(BWIDGET)/pkgIndex.tcl || exit 1 ; \
	fi

FORCE:

.PHONY: all configure

# print a make variable  'make print-REPO'
#   http://www.cmcrossroads.com/article/printing-value-makefile-variable
print-%:
	@echo $* = \"$($*)\"
