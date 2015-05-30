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
