#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#		Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#    VSM_notice_end

DEPTH = ../..

LIB_HOME = ../src/lib

include $(DEPTH)/mk/common.mk

BUILD_64BIT = yes

LIB = mqar
LIB_SRC = \

LIB_LIBS +=  -larchive

DEPCFLAGS += $(THRCOMP)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
