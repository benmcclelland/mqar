DEPTH = ../../..

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

MLIBS = mqar_log_zsys
mqar_log_zsys_SRC = \
    zsyslog.c

mqar_log_zsys_LIBS +=  \
$(DYNAMIC_OPT) -lzmq -lczmq $(LIBSO_OPT)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
