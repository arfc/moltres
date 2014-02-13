###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Required Make variables
# APP_NAME	- the name of this application (all lower case)
# MOOSE_DIR	- location of the MOOSE framework
# ELK_DIR	- location of ELK (if enabled)
# BISON_DIR	- location of BISON
# MARMOT_DIR	- location of MARMOT
#
# Optional Environment variables
# CURR_DIR	- current directory (DO NOT MODIFY THIS VARIABLE)
#
#
# Note: Make sure that there is no whitespace after the word 'yes' if enabling
# an application
###############################################################################
CURR_DIR        ?= $(shell pwd)
ROOT_DIR        ?= $(shell dirname `pwd`)

ifeq ($(MOOSE_DEV),true)
	MOOSE_DIR ?= $(ROOT_DIR)/devel/moose
else
	MOOSE_DIR ?= $(ROOT_DIR)/moose
endif

ELK_DIR         ?= $(ROOT_DIR)/elk
STORK_DIR     ?= $(ROOT_DIR)/stork

APPLICATION_NAME := stork

DEP_APPS    ?= $(shell $(MOOSE_DIR)/scripts/find_dep_apps.py $(APPLICATION_NAME))

################################## ELK MODULES ################################
ALL_ELK_MODULES := yes
###############################################################################


include $(MOOSE_DIR)/build.mk

include $(MOOSE_DIR)/moose.mk
include $(ELK_DIR)/elk.mk
include $(STORK_DIR)/stork.mk

###############################################################################
# Additional special case targets should be added here
