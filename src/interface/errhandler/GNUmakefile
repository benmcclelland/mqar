DEPTH = ../../..

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

MLIBS = mqar_err_default
mqar_err_default_SRC = \
    defaulterr.c

mqar_err_default_LIBS +=

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
