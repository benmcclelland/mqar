#	linux.mk - definitions for a Linux build environment


#
# Linux common commands
#
PERL = /usr/bin/perl
BISON_KLUDGE = /usr/bin/bison

MAKEDEPEND = /usr/bin/makedepend

CMDECHO = /bin/echo
CMDWHOAMI = /usr/bin/whoami

OS_CHECK := $(shell grep -is suse /etc/issue)

ifndef OS_CHECK
  # RHEL
  OS_DIST := $(shell awk -F '.' '/^%dist/ {print $$2}' /etc/rpm/macros.dist)
  OSFLAGS= -DRHE_LINUX
  export OS_TYPE=redhat
endif

#
# libraries to link with when using threads
#
THRLIBS = -lpthread

PROC = $(shell uname -m)

ifdef OS_CHECK
  # SUSE
  OSFLAGS= -DSUSE_LINUX
  export OS_TYPE=suse
endif

OSFLAGS += -D${PROC}
ifeq (x86_64, ${PROC})
  OSFLAGS += -D_ASM_X86_64_SIGCONTEXT_H -mno-red-zone
endif

#
# turn compiler warnings into errors
#
CERRWARN := -Werror

OS_ARCH= $(OS)_$(OS_TYPE)_$(PROC)
export OS_ARCH

# Debugger
OSFLAGS += -g

LIBSO_OPT = -Wl,-R
STATIC_OPT = -Wl,-Bstatic
DYNAMIC_OPT = -Wl,-Bdynamic
SHARED_CFLAGS = -fPIC -shared
DEPCFLAGS = -I$(DEPTH)/include $(OSFLAGS)
CMDECHO = /bin/echo
CMDWHOAMI = /usr/bin/id -nu
CMDIDGRP = /usr/bin/id -ng

CSCOPE = cscope

# Installation directories.
# DESTDIR is the start of the path for install
DESTDIR ?=
INCDIR ?= $(DESTDIR)/usr/include
MANDIR ?= $(DESTDIR)/usr/share/man
ifeq (x86_64,${PROC})
  LIBDIR ?= $(DESTDIR)/usr/lib64
else
  LIBDIR ?= $(DESTDIR)/usr/lib
endif

CC = /usr/bin/gcc
GCC = /usr/bin/gcc
LD = $(CC)
AWK = /bin/awk
PERL= $(shell which perl)
MAKEDEPEND = /usr/X11R6/bin/makedepend -Dlinux
INSTALL = /usr/bin/install -D
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
