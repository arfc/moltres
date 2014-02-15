###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Required Make variables
# APPLICATION_NAME  - the name of this application (all lower case)
# CURR_DIR	    - current directory (DO NOT MODIFY THIS VARIABLE)
#
# Optional Environment variables
# MOOSE_REPO        - location of the cloned MOOSE repository
# MOOSE_DIR	    - location of the MOOSE framework
# ELK_DIR	    - location of ELK (if required)
#
###############################################################################
APPLICATION_NAME           := stork
CURR_DIR                   := $(shell pwd)
STORK_DIR                  := $(CURR_DIR)
ROOT_DIR                   := $(shell dirname `pwd`)

MOOSE_DIR                  ?= $(ROOT_DIR)/moose

DEP_APPS                   ?= $(shell $(MOOSE_DIR)/framework/scripts/find_dep_apps.py $(APPLICATION_NAME))

include $(MOOSE_DIR)/framework/build.mk
include $(MOOSE_DIR)/framework/moose.mk
include $(STORK_DIR)/stork.mk

###############################################################################
# Additional special case targets should be added here
