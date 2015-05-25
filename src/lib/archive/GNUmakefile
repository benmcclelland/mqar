#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#		Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#    VSM_notice_end

DEPTH = ../../..

LIB_HOME = ../src/lib

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

LIB = mqar_archive
LIB_SRC = \
    diskarchive.c

LIB_LIBS +=  \
    $(DYNAMIC_OPT) -larchive $(LIBSO_OPT)

DEPCFLAGS += $(THRCOMP)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
