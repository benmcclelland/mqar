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

#	targets.mk - target definition
#
#	The following variables can be defined in a Makefile:
#
#		MLIBS		  	program library name list 
#		<libname>_SRC	program library source list
#		<libname>_LIB	program library list for linking 
#
#	multilib.mk must be included after targets.mk


###############################################################################
#	Handle the MLIBS variable. MLIBS designates a list of libraries that
#	are compiled from the sources in <libname>_SRC.

STRIP_LIBS = $(strip $(MLIBS))
.PHONY: $(STRIP_LIBS)

CFLAGS += $(SHARED_CFLAGS) 

#
# Generate both static (.a) and shared (.so) formats
#
ALL_LIBS = $(addsuffix .a, $(STRIP_LIBS)) $(addsuffix .so, $(STRIP_LIBS))
$(STRIP_LIBS):  $(addprefix $(OBJ_DIR)/lib, $(ALL_LIBS))

define LIBS_template
ifneq ($$(STRIP_LIBS), )

  LIBS_SRC += $$($(1)_SRC)

  $(1)_OBJS := $$($(1)_SRC:%.c=$(OBJ_DIR)/%.o)

  $(OBJ_DIR)/lib$(1).a:   $$($(1)_OBJS)
	rm -f $$@
	ar ruc $$@ $$^
	@if [ ! -z "$(LIB_HOME)" ]; then \
		echo rm -f $(DEPTH)/lib/$$(OBJ_DIR)/lib$(1).a; \
		rm -f $(DEPTH)/lib/$$(OBJ_DIR)/lib$(1).a; \
		echo ln -s $(LIB_HOME)/$$(OBJ_DIR)/lib$(1).a $(DEPTH)/lib64/lib$(1).a; \
		ln -s $(LIB_HOME)/$$(OBJ_DIR)/lib$(1).a $(DEPTH)/lib64/lib$(1).a; \
	fi

  $(OBJ_DIR)/lib$(1).so:  $$($(1)_OBJS)
	rm -f $$@
	$(CC) $(SHARED_CFLAGS) -o $$@ $$(LDFLAGS) $$^ $($(1)_LIBS)
	@if [ ! -z "$(LIB_HOME)" ]; then \
		echo rm -f $$(DEPTH)/lib/$$(OBJ_DIR)/lib$(1).so; \
		rm -f $$(DEPTH)/lib/$$(OBJ_DIR)/lib$(1).so; \
		echo ln -s $$(LIB_HOME)/$$(OBJ_DIR)/lib$(1).so $$(DEPTH)/lib64/lib$(1).so; \
		ln -s $$(LIB_HOME)/$$(OBJ_DIR)/lib$(1).so $$(DEPTH)/lib64/lib$(1).so; \
	fi

endif
endef

$(foreach lib,$(STRIP_LIBS),$(eval $(call LIBS_template,$(lib))))


###############################################################################

all:	$(STRIP_LIBS)
