#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#               Copyright (c) 2007 Sun Microsystems, Inc. All Rights Reserved.
#               Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#               U.S. Government Rights - Commercial software. Government users are
#       subject to the Sun Microsystems, Inc. standard license agreement and applicable
#       provisions of the FAR and its supplements.
#
#       Use is subject to license terms. Sun, Sun Microsystems and the Sun logo
#       are trademarks or registered trademarks of Sun Microsystems, Inc. in the U.S.
#       and other countries.
#
#    VSM_notice_end
#
#
#       LGPL NOTICE
#
#       This library is free software; you can redistribute it and/or
#       modify it under the terms of the GNU Lesser General Public
#       License as published by the Free Software Foundation; either
#       version 2.1 of the License, or (at your option) any later version.
#
#       This library is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#       Lesser General Public License for more details.
#
#       You should have received a copy of the GNU Lesser General Public
#       License along with this library; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#	targets.mk - target definition
#
#	The following variables can be defined in a Makefile:
#
#		DIRS		sub directories (can be a list)
#		PROG		executable program
#		PROG_SRC	program source list
#		PROG_LIBS	program library list (for linking)
#		LIB			library
#		LIB_SRC		library source list
#		LIB_LIBS	library list for linking
#
#	The following targets and dependencies are defined:
#
#		all			$(DIRS) $(PROG) $(LIB) $(MODULE)
#		clean		$(DIRS)
#		clobber		$(DIRS)
#		clobberall	$(DIRS)
#		install
#		depend
#
#	targets.mk should be included after all definitions and includes

.PRECIOUS:  .o %.o $(OBJ_DIR)/%.o

.PHONY:	all clean clobber clobberall install info depend .INIT

###############################################################################
# Make sure OBJ_DIRs exists when required

X_CHECK_OBJDIR = \
	if [ ! -z "$(LIB)" -o ! -z "$(MLIBS)" -o ! -z "$(PROG)" \
		  -o ! -z "$(MPROGS)" -o ! -z "$(MODULE)" ]; then \
			[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR); \
	fi
__dummy := $(shell $(X_CHECK_OBJDIR))

X_CHECK_LIBDIR = \
	if [ ! -z "$(LIB_HOME)" -o ! -z "$(JAR_HOME)" ]; then \
			[ -d $(DEPTH)/lib/$(OBJ_DIR) ] || mkdir -p $(DEPTH)/lib64; \
	fi
__dummy := $(shell $(X_CHECK_LIBDIR))

#
# Are we building multiple libraries from a single makefile?
#
ifneq ($(MLIBS), )
  include $(DEPTH)/mk/multilib.mk
endif

#
# Are we building multiple programs from a single makefile?
#
ifneq ($(MPROGS), )
  include $(DEPTH)/mk/multiprog.mk
endif

###############################################################################
#	Handle the DIRS variable. This is a list of directories to
#	enter and make.

STRIP_DIRS = $(strip $(DIRS))
ifneq ($(STRIP_DIRS),)
  .PHONY:	$(STRIP_DIRS)

  .WAIT:
	echo "Waiting for background processes to finish..."; \
	wait `jobs -p`

  $(STRIP_DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
endif

###############################################################################
#	Handle the PROG variable. PROG designates an executable that
#	is compiled from the sources in PROG_SRC. (See also MPROGS in multiprog.mk)

STRIP_PROG = $(strip $(PROG))
ifneq ($(STRIP_PROG),)
  .PHONY: $(STRIP_PROG)

  PROG_OBJS = $(PROG_SRC:%.c=$(OBJ_DIR)/%.o)

  #
  # Create a variable with the path to the dependency libraries.
  # This only catches changes to existing libraries.  If the library
  # doesn't exist, this does nothing.
  X_PROG_LIBDEP	:= $(foreach lib, $(patsubst -l%, lib%.so, $(PROG_LIBS)),\
			$(foreach dir, $(patsubst -L%, %,$(PROG_LIBS)),\
				$(wildcard $(dir)/$(lib))))
  DEPLIBS = $(strip $(X_PROG_LIBDEP))

  $(OBJ_DIR)/$(PROG):	$(PROG_OBJS) $(DEPLIBS)
	$(LD) -o $@ $(LDFLAGS) $(PROG_OBJS) $(PROG_LIBS)

  $(STRIP_PROG):	$(addprefix $(OBJ_DIR)/, $(PROG))

endif

###############################################################################
#	Handle the LIB variable. LIB designates a library that
#	is compiled from the sources in LIB_SRC. (See also MLIBS in multilib.mk)

STRIP_LIB = $(strip $(LIB))
ifneq ($(STRIP_LIB), )
  .PHONY: $(STRIP_LIB)

  CFLAGS += $(SHARED_CFLAGS)

  LIB_OBJS = $(LIB_SRC:%.c=$(OBJ_DIR)/%.o)

  #
  # Create a variable with the path to the dependency libraries.
  # This only catches changes to existing libraries.  If the library
  # doesn't exist, this does nothing.
  #
  X_LIB_LIBDEP	:= $(foreach lib, $(patsubst -l%, lib%.a, $(LIB_LIBS)),\
			$(foreach dir, $(patsubst -L%, %,$(LIB_LIBS)),\
				$(wildcard $(dir)/$(lib))))

  DEPLIBS = $(strip $(X_LIB_LIBDEP))

  # Generate both static (.a) and shared (.so) formats
  #
  ALL_LIBS = $(addsuffix .a, $(STRIP_LIB)) $(addsuffix .so, $(STRIP_LIB))
  $(STRIP_LIB):  $(addprefix $(OBJ_DIR)/lib, $(ALL_LIBS))

  $(OBJ_DIR)/lib%.a:   $(LIB_OBJS) $(DEPLIBS)
	-rm -f $@
	ar ruc $@ $(LIB_OBJS)
	-ranlib $@
	-@if [ ! -z "$(LIB_HOME)" ]; then \
		echo rm -f $(DEPTH)/lib/$(OBJ_DIR)/$(notdir $@); \
		rm -f $(DEPTH)/lib/$(OBJ_DIR)/$(notdir $@); \
		echo ln -s $(LIB_HOME)/$(OBJ_DIR)/$(notdir $@) $(DEPTH)/lib64/$(notdir $@); \
		ln -fs $(LIB_HOME)/$(OBJ_DIR)/$(notdir $@) $(DEPTH)/lib64/$(notdir $@); \
	fi

  $(OBJ_DIR)/lib%.so:   $(LIB_OBJS) $(DEPLIBS)
	-rm -f $@
	$(LD) $(SHARED_CFLAGS) -o $@ $(LDFLAGS) $(LIB_OBJS) $(LIB_LIBS)
	-@if [ ! -z "$(LIB_HOME)" ]; then \
		echo rm -f $(DEPTH)/lib/$(OBJ_DIR)/$(notdir $@); \
		rm -f $(DEPTH)/lib/$(OBJ_DIR)/$(notdir $@); \
		echo ln -s $(LIB_HOME)/$(OBJ_DIR)/$(notdir $@) $(DEPTH)/lib64/$(notdir $@); \
		ln -fs $(LIB_HOME)/$(OBJ_DIR)/$(notdir $@) $(DEPTH)/lib64/$(notdir $@); \
	fi

endif

.INIT:

all:	.INIT $(STRIP_DIRS) $(STRIP_LIB) $(STRIP_PROG)

#
# Make sure .INIT gets called before anything else
#
$(STRIP_LIB) $(STRIP_PROG): .INIT


clean:	$(STRIP_DIRS)
	-rm -rf $(OBJ_DIR) *.ln

clobber:	clean

clobberall:	$(STRIP_DIRS)
	-rm -rf $(OBJ_BASE) *.ln

install:

$(OBJ_DIR)/%.o:		%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
ifeq ($(BUILD_STABS), yes)
	$(CTFCONVERT_CMD)
endif

$(OBJ_DIR)/%.o:		%.S
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
ifeq ($(BUILD_STABS), yes)
	$(CTFCONVERT_CMD)
endif

%.cat:	$(CATMSGS)
	-rm -f             $@
	/usr/bin/gencat   $@ $<
	/usr/bin/chmod -w $@

ifneq ($(DEPLIBS),)
  $(DEPLIBS):
	# Need to add something here to build dependent libs
	@echo "Warning: one or more of the required libraries do not exist."
	@echo "Run 'gmake' at the top level to rebuild the libraries."
	exit 1
endif


MARKER = $(shell cat $(DEPTH)/MARKER)

info:	$(DEPTH)/MARKER
	@echo
	@echo "BUILD PARAMETERS"
	@echo "           DEBUG: " $(DEBUG)
	@echo "        COMPLETE: " $(COMPLETE)
	@echo
	@echo "BUILD ENVIROMENT"
	@echo "  SAMQFS_VERSION: " $(SAMQFS_VERSION)
	@echo "          MARKER: " $(MARKER)
	@echo "        HOSTNAME: " $(HOSTNAME)
	@echo "              OS: " $(OS)
	@echo "         OS_TYPE: " $(OS_TYPE)
	@echo "       PROCESSOR: " $(PROC)
	@echo "         OBJ_DIR: " $(OBJ_DIR)

$(DEPTH)/MARKER:
	@if [ "$(shell /bin/basename $(shell cd $(DEPTH); /bin/pwd))" = "sam-qfs" ] ; then	\
		echo "$(shell /bin/basename $(shell cd $(DEPTH)/..; /bin/pwd))" > $@;	\
	else	\
		echo "$(shell /bin/basename $(shell cd $(DEPTH); /bin/pwd))" > $@;	\
	fi

#
# Special top level target to build Linux client package
#
.PHONY:	linuxpkg
linuxpkg:
	$(MAKE) -C pkg-linux
