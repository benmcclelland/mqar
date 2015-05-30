DEPTH = ../../..

include $(DEPTH)/mk/common.mk

INCS += -I. -I$(DEPTH)/src/include
DEPCFLAGS = $(OSFLAGS) $(INCS)

MLIBS = mqar_config_yaml
mqar_config_yaml_SRC = \
    yamlconfig.c

mqar_config_yaml_LIBS +=  \
$(DYNAMIC_OPT) -lyaml $(LIBSO_OPT)

include $(DEPTH)/mk/targets.mk
include $(DEPTH)/mk/multilib.mk
include $(DEPTH)/mk/depend.mk
