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

#	targets_man.mk - manual/docs target definitions
#
#	The following variables can be defined in a Makefile:
#
#		MAN_SRCS	  	manual source files
#		MAN_RELEASED_SRCS	manual source files targeted for release
#		QFS_RELEASED_SRCS	qfs manual source files targeted for release
#		MIG_RELEASED_SRCS	mig manual source files targeted for release
#		REM_RELEASED_SRCS	rem manual source files targeted for release
#
###############################################################################

STRIP_DIRS = $(strip $(DIRS))
ifneq ($(STRIP_DIRS),)
  .PHONY: $(STRIP_DIRS)

  $(STRIP_DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
endif

.INIT:

all:

clean:	$(STRIP_DIRS)
	-@rm -f *.ps

clobber: clean
clobberall: clobber

depend:

install:	$(STRIP_DIRS)
ifneq ($(MAN_SRCS),)
	@ for f in $(MAN_SRCS); do                            \
		echo $$f;                                     \
		$(INSTALL_DATA) $$f $(MANDIR)/$(MANSECT)/$$f; \
	done
endif

print_all:	$(STRIP_DIRS)
ifneq ($(MAN_SRCS),)
	@ for f in $(MAN_SRCS); do                        \
		troff -man $$f | dpost -m .95 | lp -o nb; \
	done
endif

print:	$(STRIP_DIRS)
ifneq ($(MAN_RELEASED_SRCS),)
	@ banner User Commands | lp -o nb
	@ for f in $(MAN_RELEASED_SRCS); do               \
		troff -man $$f | dpost -m .95 | lp -o nb; \
	done
endif

print_mig:	$(STRIP_DIRS)
ifneq ($(MIG_RELEASED_SRCS),)
	@ banner User Commands | lp -o nb
	@ for f in $(MIG_RELEASED_SRCS); do               \
		troff -man $$f | dpost -m .95 | lp -o nb; \
	done
endif

print_qfs:	$(STRIP_DIRS)
ifneq ($(QFS_RELEASED_SRCS),)
	@ banner User Commands | lp -o nb
	@ for f in $(QFS_RELEASED_SRCS); do               \
		troff -man $$f | dpost -m .95 | lp -o nb; \
	done
endif

print_rem:	$(STRIP_DIRS)
ifneq ($(REM_RELEASED_SRCS),)
	@ banner Admin Commands | lp -o nb
	@ for f in $(REM_RELEASED_SRCS); do                     \
		tbl $$f | troff -man | dpost -m .95 | lp -o nb; \
	done
endif

ps:		$(STRIP_DIRS)
ifneq ($(MAN_RELEASED_SRCS),)
	@ for f in $(MAN_RELEASED_SRCS); do             \
		troff -man $$f | dpost -m .95 > $$f.ps; \
	done
endif

ps_mig:	$(STRIP_DIRS)
ifneq ($(MIG_RELEASED_SRCS),)
	@ for f in $(MIG_RELEASED_SRCS); do             \
		troff -man $$f | dpost -m .95 > $$f.ps; \
	done
endif

ps_qfs:	$(STRIP_DIRS)
ifneq ($(QFS_RELEASED_SRCS),)
	@ for f in $(QFS_RELEASED_SRCS); do             \
		troff -man $$f | dpost -m .95 > $$f.ps; \
	done
endif

ps_rem:	$(STRIP_DIRS)
ifneq ($(REM_RELEASED_SRCS),)
	@ for f in $(REM_RELEASED_SRCS); do             \
		troff -man $$f | dpost -m .95 > $$f.ps; \
	done
endif

pstar: $(STRIP_DIRS) ps
ifneq ($(MAN_RELEASED_SRCS),)
	@ for f in $(MAN_RELEASED_SRCS); do         \
		echo man1/$$f.ps >> ../relsrc.list; \
	done
endif

pstar_mig:	$(STRIP_DIRS)
ifneq ($(MIG_RELEASED_SRCS),)
	@ for f in $(MIG_RELEASED_SRCS); do         \
		echo man1/$$f.ps >> ../relsrc.list; \
	done
endif

pstar_qfs: $(STRIP_DIRS) ps_qfs
ifneq ($(QFS_RELEASED_SRCS),)
	@ for f in $(QFS_RELEASED_SRCS); do            \
		echo man1/$$f.ps >> ../qfsrelsrc.list; \
	done
endif

pstar_rem:	$(STRIP_DIRS) ps_rem
ifneq ($(REM_RELEASED_SRCS),)
	@ for f in $(REM_RELEASED_SRCS); do            \
		echo man1/$$f.ps >> ../qfsrelsrc.list; \
	done
endif
