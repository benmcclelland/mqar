#
#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#               Copyright (c) 2007 Sun Microsystems, Inc. All Rights Reserved.
#               Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#               U.S. Government Rights - Commercial software. Government users are
#       subject to the Sun Microsystems, Inc. standard license agreement and applicable
#       provisions of the FAR and its supplements.
#
#       Use is subject to license terms. Sun, Sun Microsystems and the Sun logo
#       are trademarks or registered trademarks of Sun Microsystems, Inc. in the U.S.
#       and other countries.
#
#    VSM_notice_end
#
#
#       LGPL NOTICE
#
#       This library is free software; you can redistribute it and/or
#       modify it under the terms of the GNU Lesser General Public
#       License as published by the Free Software Foundation; either
#       version 2.1 of the License, or (at your option) any later version.
#
#       This library is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#       Lesser General Public License for more details.
#
#       You should have received a copy of the GNU Lesser General Public
#       License along with this library; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

MSGDEST = $(DESTDIR)/usr/share/locale/C/LC_MESSAGES

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
DEPCFLAGS = -I$(DEPTH)/include $(OSFLAGS) \
	-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
	-D_BIT_FIELDS_LTOH

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
