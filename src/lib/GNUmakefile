DEPTH = ../..

LIB_HOME = ../src/lib

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

LIB = mqar
LIB_SRC = \
    progress_threads.c

LIB_LIBS +=  \
    $(DYNAMIC_OPT) -levent $(LIBSO_OPT)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
