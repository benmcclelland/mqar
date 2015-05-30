#
# define source list for 'make depend'
#
ifeq ($(DEPEND_SRC), )
  ifeq ($(strip $(SRC_VPATH)), )
	X_DEP_SRC = $(PROG_SRC) $(LIB_SRC) $(MODULE_SRC) $(PROGS_SRC) $(LIBS_SRC)
  else
	#
	# if we're using vpath, we need to figure out where all
	# the source actually lives.
	#
	X_DEP_SRC = $(foreach file, $(PROG_SRC) $(LIB_SRC) $(MODULE_SRC) $(PROGS_SRC) $(LIBS_SRC), \
					$(foreach dir, . $(SRC_VPATH), \
						$(wildcard $(dir)/$(file))))
  endif
  DEPEND_SRC = $(strip $(X_DEP_SRC))
endif

depend:	.INIT $(STRIP_DIRS)

ifneq ($(DEPEND_SRC), )
  depend: 	$(DEPFILE)

  $(DEPFILE): 	GNUmakefile $(DEPEND_SRC)
		@touch $(DEPFILE); \
		echo $(MAKEDEPEND) -f $(DEPFILE) -p$(OBJ_DIR)/ -- ${DEPCFLAGS} -- $(DEPEND_SRC); \
		$(MAKEDEPEND) -f $(DEPFILE) -p$(OBJ_DIR)/ -- -DMAKEDEPEND ${DEPCFLAGS} -- $(DEPEND_SRC)

  X_DEPFILE_EXISTS = [ ! -f $(DEPFILE) ] || echo yes
  DEPFILE_EXISTS = $(shell $(X_DEPFILE_EXISTS))
  ifeq ($(DEPFILE_EXISTS), yes)
    include $(DEPFILE)
  endif
else
  depend: $(STRIP_DIRS)
endif
