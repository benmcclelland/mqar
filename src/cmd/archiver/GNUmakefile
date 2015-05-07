#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#		Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#    VSM_notice_end

DEPTH = ../../..

include $(DEPTH)/mk/common.mk

PROG = archiver
PROG_SRC = archiver.c

INCS += -I. -I$(DEPTH)/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

PROG_LIBS = \
    $(STATIC_OPT) -L../../lib/$(OBJ_DIR) -lmdp \
    $(DYNAMIC_OPT) -L $(DEPTH)/lib64 -lzmq -lczmq $(LIBSO_OPT)
install: all

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
