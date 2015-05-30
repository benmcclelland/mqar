DEPTH = ../../..

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

MLIBS = mqar_archive_disk
mqar_archive_disk_SRC = \
    diskarchive.c

mqar_archive_disk_LIBS +=  \
    $(DYNAMIC_OPT) -larchive $(LIBSO_OPT)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
