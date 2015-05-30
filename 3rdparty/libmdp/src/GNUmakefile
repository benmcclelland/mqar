DEPTH = ../../..
LIB_HOME = ../3rdparty/libmdp/src

include $(DEPTH)/mk/common.mk

BUILD_64BIT = yes

LIB = mdp
LIB_SRC = \
	mdp_client.c \
	mdp_worker.c

PROG = mqar_broker
PROG_SRC = mdp_broker.c

LIB_LIBS +=  -lzmq -lczmq

PROG_LIBS = -lzmq -lczmq

DEPCFLAGS += $(THRCOMP)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/depend.mk
