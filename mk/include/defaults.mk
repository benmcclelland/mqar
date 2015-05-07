#
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
#	defaults.mk - default build parameters
#
#	Variables are assigned via on of three ways:
#
#		Assignment on the make line. For example:
#			make DEBUG=off
#		Assignment on the make line overrides any definitions in
#			the *.mk files.
#
#		Definition in the CONFIG.mk file. This file is created by:
#			make config <par1=value1> ...
#		Definition in CONFIG.mk overrides definitions in defaults.mk.
#
#		Default assignment in defaults.mk.
#
#	DO NOT CHANGE THIS FILE UNLESS DEFAULT BEHAVIOR IS BEING MODIFIED.

#	Assign default values to variables that have not been defined.

DEBUG ?= no
COMPLETE ?= no
