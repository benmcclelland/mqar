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

MLIBS = mqar_err_default
mqar_err_default_SRC = \
    defaulterr.c

mqar_err_default_LIBS +=

DEPCFLAGS += $(THRCOMP)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
