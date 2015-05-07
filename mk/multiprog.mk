#    VSM_notice_begin
#
#      VSM - Versity Storage Management File System
#
#               Copyright (c) 2007 Sun Microsystems, Inc. All Rights Reserved.
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
