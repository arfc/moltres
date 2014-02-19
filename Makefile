###############################################################################
################### MOOSE Application Standard Makefile #######################
###############################################################################
#
# Required Make variables
# APPLICATION_NAME  - the name of this application (all lower case)
#
# Optional Environment variables
# MOOSE_DIR         - location of the cloned MOOSE repository
#
###############################################################################
ROOT_DIR                   := $(shell dirname `pwd`)
MOOSE_DIR                  ?= $(ROOT_DIR)/moose	

# framework
include $(MOOSE_DIR)/framework/build.mk
include $(MOOSE_DIR)/framework/moose.mk

# dep apps
APPLICATION_DIR    := $(ROOT_DIR)/stork
APPLICATION_NAME   := stork
BUILD_EXEC         := yes
DEP_APPS           := $(shell $(MOOSE_DIR)/framework/scripts/find_dep_apps.py $(APPLICATION_NAME))
include            $(MOOSE_DIR)/framework/app.mk

###############################################################################
# Additional special case targets should be added here
