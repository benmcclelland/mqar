DEPTH = ../../..

include $(DEPTH)/mk/common.mk

PROG = scheduler
PROG_SRC = scheduler.c

INCS += -I. -I$(DEPTH)/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

PROG_LIBS = \
    $(STATIC_OPT) -L../../lib/$(OBJ_DIR) -lmdp \
    $(DYNAMIC_OPT) -L $(DEPTH)/lib64 -lzmq -lczmq $(LIBSO_OPT)
install: all

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
