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

ELK_DIR                    ?= $(shell dirname `pwd`)
MOOSE_REPO                 ?= $(shell dirname $ELK_DIR)
MOOSE_DIR                  ?= $(MOOSE_REPO)/moose

DEP_APPS                   ?= $(shell $(MOOSE_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))

STORK_DIR ?= $(ELK_DIR)/stork
include $(MOOSE_DIR)/build.mk
include $(MOOSE_DIR)/moose.mk
include $(STORK_DIR)/stork.mk

###############################################################################
# Additional special case targets should be added here
