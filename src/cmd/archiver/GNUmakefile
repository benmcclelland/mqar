DEPTH = ../../..

include $(DEPTH)/mk/common.mk

PROG = archiver
PROG_SRC = archiver.c

INCS += -I. -I$(DEPTH)/include -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

PROG_LIBS = \
    $(STATIC_OPT) -L../../lib/$(OBJ_DIR) -lmdp -lmqar \
    $(DYNAMIC_OPT) -L $(DEPTH)/lib64 -lzmq -lczmq -levent_pthreads -levent -larchive $(LIBSO_OPT)
install: all

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
