#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#		Copyright (c) 2015 Versity, Inc. All Rights Reserved.
#
#    VSM_notice_end
#
#
#	LGPL NOTICE
#
#	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation; either
#	version 2.1 of the License, or (at your option) any later version.
#
#	This library is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#	Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public
#	License along with this library; if not, write to the Free Software
#	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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
