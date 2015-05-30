DEPTH = ../../..

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

MLIBS = mqar_csum_sha1
mqar_csum_sha1_SRC = \
    sha1.c

mqar_csum_sha1_LIBS +=  \
    $(DYNAMIC_OPT) -lcrypto $(LIBSO_OPT)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
