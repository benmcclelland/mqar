#	common.mk - common definitions
#
#	common.mk should be the first inclusion in all Makefiles.

PROJECT = mqar

# Default number of parallel jobs allowed
NJOBS = 4

.PHONY:	all

# Default target
all:

ifndef	DEPTH
  $(error "DEPTH variable is undefined")
endif

include $(DEPTH)/mk/include/defaults.mk

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
# make depend output file
#
DEPFILE = $(OBJ_DIR)/.depend

#
# Default include files
#
INCLUDE = $(DEPTH)/include

#
# Final default CFLAGS settings
#
CFLAGS = $(DEPCFLAGS) $(DEBUGCFLAGS) $(PLATFLAGS) $(CERRWARN)
LDFLAGS = $(PLATFLAGS)
LIBSO = $(LIBSO_OPT)/$(LIBDEST)
