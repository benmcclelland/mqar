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

#	common.mk - common definitions
#
#	common.mk should be the first inclusion in all Makefiles.

# Default number of parallel jobs allowed
NJOBS = 4

.PHONY:	all

# Default target
all:

ifndef	DEPTH
  $(error "DEPTH variable is undefined")
endif

include $(DEPTH)/mk/include/defaults.mk

VSM_VERSION ?= 1.2.0
VSM_PATCH   ?= dev1

SHELL := /bin/sh
OS := $(shell uname -s)
HOSTNAME := $(shell hostname)

#
#     Unset LD_LIBRARY_PATH.
#
LD_LIBRARY_PATH =

#
#	Include OS, Revision, and architecture dependent definitions
#
ifeq ($(OS), Linux)
  include $(DEPTH)/mk/include/linux.mk
else
  $(error "Don't know anything about OS $(OS)")
endif

#
# Default VPATH
#
VPATH = .

#
#	Destination for generated output
#
OBJ_BASE := obj
ifneq	($(DEBUG), yes)
	OBJ_DIR := $(OBJ_BASE)/$(OS_ARCH)
else
	OBJ_DIR := $(OBJ_BASE)/$(OS_ARCH)_DEBUG
endif

#
# Set additional debug options if needed
#
# "DEBUG = yes | no" may be specified globally
#
# "DEBUG_OFF = yes" may be specified in a make file to
# suppress debug mode for that make file only.
#
ifeq ($(DEBUG), yes)
	ifneq ($(DEBUG_OFF), yes)
		DEBUGCDEFS += -DDEBUG
	endif
endif

# Enable code coverage for tests
ifeq ($(COV), yes)
	DEBUGCFLAGS += -g -coverage
endif

DEPCFLAGS += $(DEBUGCDEFS)

#
# enable LIBVSM build toggles
#
DEPCFLAGS += -DLIBVSM

#
# Build with POSIX enabled
#
DEPCFLAGS += -D_POSIX_PTHREAD_SEMANTICS

#
# Build with SAM_TRACE enabled
#
DEPCFLAGS += -DSAM_TRACE

#
# make depend output file
#
DEPFILE = $(OBJ_DIR)/.depend

#
# CFLAGS defines when using threads
#
THRCOMP = -D_REENTRANT

#
# Default libvsm include files
#
INCLUDE = $(DEPTH)/include

#
# Final default CFLAGS settings
#
CFLAGS = $(DEPCFLAGS) $(DEBUGCFLAGS) $(PLATFLAGS) $(CERRWARN)
LDFLAGS = $(PLATFLAGS)
LIBSO = $(LIBSO_OPT)/$(LIBDEST)
