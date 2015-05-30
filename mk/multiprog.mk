#	multiprog.mk - target definition
#
#	The following variables can be defined in a Makefile:
#
#		MPROGS		executable program (can be a list)
#		<progname>_SRC	program source list
#		<progname>_LIB	program library list for linking
#
#	multiprog.mk must be included after targets.mk

###############################################################################
#	Handle the MPROGS variable. MPROGS designates a list of executables that
#	are compiled from the sources in <progname>_SRC.

STRIP_PROGS = $(strip $(MPROGS))
.PHONY: $(STRIP_PROGS)

define PROGS_template
ifneq ($$(STRIP_PROGS),)
  $(1)_OBJS = $$($(1)_SRC:%.c=$(OBJ_DIR)/%.o)

  PROGS_SRC += $$($(1)_SRC)

  $(OBJ_DIR)/$(1):		$$($(1)_OBJS)
	$(CC) $$(CFLAGS) -o $$@ $$(LDFLAGS) $$($(1)_OBJS) $$($(1)_LIBS)

endif
endef

$(foreach prog,$(STRIP_PROGS),$(eval $(call PROGS_template,$(prog))))

$(STRIP_PROGS):		$(addprefix $(OBJ_DIR)/, $(MPROGS)) 


###############################################################################

all:	$(STRIP_PROGS)
